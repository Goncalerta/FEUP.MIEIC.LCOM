#include <lcom/lcf.h>

#include "vbe.h"
#include "graphics.h"

/** @defgroup vbe vbe
 * @{
 *
 */

int vbe_change_mode(uint16_t mode) {
    struct reg86 r86;
    
    /* Specify the appropriate register values */
    
    memset(&r86, 0, sizeof(r86));	/* zero the structure */

    r86.intno = BIOS_VIDEO_SERVICES;
    r86.ah = VBE_CALL_AH;
    r86.al = VBE_FUNCTION_SET_MODE;
    r86.bx = mode | VBE_LINEAR_FRAME_BUFFER_MODEL;

    if(sys_int86(&r86) != OK) {
        printf("Error in %s",__func__);
        return 1;
    }
    if (r86.ah != VBE_FUNCTION_AH_SUCCESS) {
        printf("Error in %s",__func__);
        return 1;
    }
    
    return 0;
}

int vbe_set_display_start(uint16_t first_pixel_in_scanline, uint16_t first_scanline) {
    struct reg86 r86;
    
    /* Specify the appropriate register values */
    
    memset(&r86, 0, sizeof(r86));	/* zero the structure */

    r86.intno = BIOS_VIDEO_SERVICES;
    r86.ah = VBE_CALL_AH;
    r86.al = VBE_FUNCTION_SET_GET_DISPLAY_START;
    r86.bl = SET_DISPLAY_START_DURING_VERTICAL_RETRACE;
    r86.cx = first_pixel_in_scanline;
    r86.dx = first_scanline;
    
    if(sys_int86(&r86) != OK) {
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
    mmap_t map;
    if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL)
        return 1;

    struct reg86 r86;
    memset(&r86, 0, sizeof(r86));

    r86.intno = BIOS_VIDEO_SERVICES;
    r86.ah = VBE_CALL_AH;
    r86.al = VBE_FUNCTION_RETURN_MODE_INFO;
    r86.cx = mode;
    r86.es = PB2BASE(map.phys);
    r86.di = PB2OFF(map.phys);

    if(sys_int86(&r86) != OK) {
        printf("Error in %s",__func__);
        if (!lm_free(&map))
            panic("couldn't free memory");
        return 1;
    }

    if (memcpy(vmi, map.virt, sizeof(vbe_mode_info_t)) == NULL) {
        printf("Error copying info in %s\n", __func__);
        if (!lm_free(&map))
            panic("couldn't free memory");
        return 1;
    }

    if (!lm_free(&map))
        panic("couldn't free memory");

    if (r86.ah != VBE_FUNCTION_AH_SUCCESS) {
        printf("Error in %s",__func__);
        return 1;
    }
    return 0;
}

/**@}*/
