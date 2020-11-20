#include <lcom/lcf.h>
#include <math.h>

#include "defines_graphic.h"


static void *video_mem;         /* frame-buffer VM address */
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

//OWN FUNCTION
void (vg_set_global_var_screen)(unsigned x_r, unsigned y_r, unsigned bits_pp) {
  h_res = x_r;
  v_res = y_r;
  bits_per_pixel = bits_pp;
}

//OWN FUNCTION
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


int vbe_get_mode_inf(uint16_t mode, vbe_mode_info_t *vmi) {
  //TODO fazer própria função de vbe_get_mode_info()
  return 1;
}


void *(vg_init)(uint16_t mode) {
  struct minix_mem_range mr;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_size;  /* VRAM's size, but you can use the frame-buffer size, instead */
  int r;			    

  /* Use VBE function 0x01 to initialize vram_base and vram_size */ 
  vbe_mode_info_t vmi;
  if (vbe_get_mode_info(mode, &vmi)) {
    printf("Error while getting mode.\n");
    return NULL;
  }

  vg_set_global_var_screen(vmi.XResolution, vmi.YResolution, vmi.BitsPerPixel);

  vram_base = vmi.PhysBasePtr;

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

  if(vg_vbe_change_mode(mode)) {
    printf("Error while changing mode.\n");
    return NULL;
  }

  return video_mem;
}

//OWN FUNCTION
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if ((x >= h_res) || (y >v_res)) {
    printf("Error trying to print outside the screen limits.\n");
    return 1;
  }
  uint8_t bytes_per_pixel = ceil((double)bits_per_pixel/8);

  uint8_t *pixel_mem_pos = (uint8_t *)video_mem + (y*h_res*bytes_per_pixel) + (x*bytes_per_pixel);

  if (color > COLOR_CAP_BYTES_NUM(bytes_per_pixel)) {
    printf("Invlid color argument. To large\n");
    return 1;
  }
  for (uint8_t i = 0; i < bytes_per_pixel; i++) {
    *(pixel_mem_pos+i) = (uint8_t)COLOR_BYTE(color,i);
  }
  return 0;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (uint16_t i = 0; i < len; i++) {
    if (vg_draw_pixel(x + i, y, color)) {
      return 1;
    }
  }
  return 0;
}


int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (uint16_t i = 0; i < height; i++) {
    if(vg_draw_hline(x, y+i, width, color)) {
      return 1;
    }
  }
  return 0;
}
