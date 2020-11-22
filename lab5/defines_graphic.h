#ifndef _DEFINES_GRAPHIC_H_
#define _DEFINES_GRAPHIC_H_

#include <lcom/lcf.h>

#define SECONDS_TO_MICROS 1000000

#define VBE_LINEAR_FRAME_BUFFER_MODEL BIT(14)
#define VBE_CALL_AH 0x4F
#define VBE_FUNCTION_RETURN_MODE_INFO 0x01
#define VBE_FUNCTION_SET_MODE 0x02
#define VBE_FUNCTION_AH_SUCCESS 0x00

#define BIOS_VIDEO_SERVICES 0x10

#define COLOR_BYTE(val,n) (val>>(n*8))
#define COLOR_CAP_BYTES_NUM(n) (0xFFFFFFFF >> 2*(4-n)) /* 1 <= n <= 4*/

#endif /* _DEFINES_GRAPHIC_H */
