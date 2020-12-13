#ifndef __SCAN_CODES_H
#define __SCAN_CODES_H

#include <lcom/lcf.h>


#define BREAK_CODE(x) ((x) | BREAK_CODE_BIT)

#define MAKE_A 0x1E
#define MAKE_B 0x30
#define MAKE_C 0x2E
#define MAKE_D 0x20
#define MAKE_E 0x12
#define MAKE_F 0x21
#define MAKE_G 0x22
#define MAKE_H 0x23
#define MAKE_I 0x17
#define MAKE_J 0x24
#define MAKE_K 0x25
#define MAKE_L 0x26
#define MAKE_M 0x32
#define MAKE_N 0x31
#define MAKE_O 0x18
#define MAKE_P 0x19
#define MAKE_Q 0x10
#define MAKE_R 0x13
#define MAKE_S 0x1F
#define MAKE_T 0x14
#define MAKE_U 0x16
#define MAKE_V 0x2F
#define MAKE_W 0x11
#define MAKE_X 0x2D
#define MAKE_Y 0x15
#define MAKE_Z 0x2C

#define MAKE_0 0x0B
#define MAKE_1 0x02
#define MAKE_2 0x03
#define MAKE_3 0x04
#define MAKE_4 0x05
#define MAKE_5 0x06
#define MAKE_6 0x07
#define MAKE_7 0x08
#define MAKE_8 0x09
#define MAKE_9 0x0A

#define MAKE_SPACE 0x39
#define MAKE_ENTER 0x1C
#define MAKE_BACK_SPACE 0x0E
#define MAKE_CTRL 0x1D
#define MAKE_ESC 0x01

#define MAKE_DEL 0xE053

#define MAKE_ARROW_UP 0xE048
#define MAKE_ARROW_DOWN 0xE050
#define MAKE_ARROW_LEFT 0xE04B
#define MAKE_ARROW_RIGHT 0xE04D

#endif /* __SCAN_CODES_H */

