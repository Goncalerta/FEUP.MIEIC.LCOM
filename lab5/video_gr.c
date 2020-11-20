#include <lcom/lcf.h>
#include <math.h>

#include "defines_graphic.h"


static void *video_mem;         /* frame-buffer VM address */
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */


void (vg_set_global_var_screen)(unsigned x_r, unsigned y_r, unsigned bits_pp) {
  h_res = x_r;
  v_res = y_r;
  bits_per_pixel = bits_pp;
}


int (vg_vbe_change_mode)(uint16_t mode) {
  struct reg86 r86;
   
  /* Specify the appropriate register values */
  
  memset(&r86, 0, sizeof(r86));	/* zero the structure */

  r86.intno = BIOS_VIDEO_SERVICES;
  r86.ah = VBE_CALL_AH;
  r86.al = VBE_FUNCTION_SET_MODE;
  r86.bx = mode | VBE_LINEAR_FRAME_BUFFER_MODEL;

  if(sys_int86(&r86)) {
    printf("Error in %s",__func__);
    return 1;
  }
  if (r86.ah != VBE_FUNCTION_AH_SUCCESS) {
    printf("Error in %s",__func__);
    return 1;
  }
  
  return 0;
}


void *(vg_init)(uint16_t mode) {
  struct minix_mem_range mr;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_size;  /* VRAM's size, but you can use the frame-buffer size, instead */
  int r;
  vbe_mode_info_t *vmi_p = (vbe_mode_info_t *)malloc(sizeof(vbe_mode_info_t));				    

  if(vg_vbe_change_mode(mode)) {
    printf("Error while changing mode.\n");
    return NULL;
  }

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  if (vbe_get_mode_info(mode, vmi_p)) {
    printf("Error while getting mode.\n");
    return NULL;
  }

  vg_set_global_var_screen(vmi_p->XResolution, vmi_p->YResolution, vmi_p->BitsPerPixel);

  vram_base = vmi_p->PhysBasePtr;
  free(vmi_p);
  vram_size = h_res * v_res * ceil((double)bits_per_pixel/8);

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  return video_mem;
}
