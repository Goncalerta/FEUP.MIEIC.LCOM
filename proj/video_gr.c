#include <lcom/lcf.h>
#include <math.h>

#include "video_gr.h"
#include "vbe.h"

/** @defgroup video_gr video_gr
 * @{
 *
 */

static frame_buffer_t buf1; /**< @brief First video graphics frame buffer */
static frame_buffer_t buf2; /**< @brief Second video graphics frame buffer */
static bool buf1_is_primary = true; /**< @brief Whether buf1 is the primary (front) buffer */

frame_buffer_t vg_get_back_buffer() {
    if (buf1_is_primary) {
        return buf2;
    } else {
        return buf1;
    }
}


/**
 * @brief Initialize video graphics buffers from the given vbe_mode_info_t.
 * 
 * @param vmi memory address to the vbe_mode_info_t used to initialize the buffers
 */
static void vg_set_global_var_screen(vbe_mode_info_t *vmi) {
    buf1.h_res = buf2.h_res = vmi->XResolution;
    buf1.v_res = buf2.v_res = vmi->YResolution;
    buf1.bytes_per_pixel = buf2.bytes_per_pixel = ceil(vmi->BitsPerPixel / 8.0);
}

/**
 * @brief Initialize video graphics with the given mode.
 * 
 * @param mode mode to be set.
 * @return Memory adress to the first video graphics frame buffer, or NULL if an error occurs
 */
void *(vg_init)(uint16_t mode) {
    struct minix_mem_range mr1;
    struct minix_mem_range mr2;
    unsigned int vram_base, vram_size;

    /* Use VBE function 0x01 to initialize vram_base and vram_size */ 
    vbe_mode_info_t vmi;
    if (vbe_get_mode_inf(mode, &vmi)) {
        printf("Error while getting mode.\n");
        return NULL;
    }

    vg_set_global_var_screen(&vmi);

    vram_base = vmi.PhysBasePtr;
    vram_size = buf1.h_res * buf1.v_res * buf1.bytes_per_pixel;

    /* Allow memory mapping */
    mr1.mr_base = vram_base;
    mr1.mr_limit = mr1.mr_base + vram_size;
    mr2.mr_base = mr1.mr_limit;
    mr2.mr_limit = mr2.mr_base + vram_size;

    int r;
    if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr1)) != OK)
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr2)) != OK)
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    /* Map memory */
    buf1.buf = vm_map_phys(SELF, (void *) mr1.mr_base, vram_size);
    if (buf1.buf == MAP_FAILED)
        panic("couldn't map video memory");

    buf2.buf = vm_map_phys(SELF, (void *) mr2.mr_base, vram_size);
    if (buf1.buf == MAP_FAILED)
        panic("couldn't map video memory");

    if (vbe_change_mode(mode)) {
        printf("Error while changing mode.\n");
        return NULL;
    }

    return buf1.buf;
}

uint16_t vg_get_hres() {
    return buf1.h_res;
}

uint16_t vg_get_vres() {
    return buf1.v_res;
}

uint16_t vg_get_bytes_per_pixel() {
    return buf1.bytes_per_pixel;
}

int vg_flip_page() {
    if (buf1_is_primary) {
        if (vbe_set_display_start(0, buf1.v_res) != OK)
            return 1;
    } else {
        if (vbe_set_display_start(0, 0) != OK)
            return 1;
    }
    buf1_is_primary = !buf1_is_primary;
    return 0;
}

/**@}*/
