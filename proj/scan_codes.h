#ifndef __SCAN_CODES_H
#define __SCAN_CODES_H

#include <lcom/lcf.h>

/** @file 
 * @brief PC's keyboard scan codes.
 */

/** @defgroup scan_codes scan_codes
 * @{
 *
 * @brief PC's keyboard scan codes.
 */

#define BREAK_CODE(x) ((x) | BREAK_CODE_BIT)  /**< @brief Converts a key MAKE CODE to it's BREAK CODE */

#define MAKE_A 0x1E /**< @brief 'A' key MAKE CODE */
#define MAKE_B 0x30 /**< @brief 'B' key MAKE CODE */
#define MAKE_C 0x2E /**< @brief 'C' key MAKE CODE */
#define MAKE_D 0x20 /**< @brief 'D' key MAKE CODE */
#define MAKE_E 0x12 /**< @brief 'E' key MAKE CODE */
#define MAKE_F 0x21 /**< @brief 'F' key MAKE CODE */
#define MAKE_G 0x22 /**< @brief 'G' key MAKE CODE */
#define MAKE_H 0x23 /**< @brief 'H' key MAKE CODE */
#define MAKE_I 0x17 /**< @brief 'I' key MAKE CODE */
#define MAKE_J 0x24 /**< @brief 'J' key MAKE CODE */
#define MAKE_K 0x25 /**< @brief 'K' key MAKE CODE */
#define MAKE_L 0x26 /**< @brief 'L' key MAKE CODE */
#define MAKE_M 0x32 /**< @brief 'M' key MAKE CODE */
#define MAKE_N 0x31 /**< @brief 'N' key MAKE CODE */
#define MAKE_O 0x18 /**< @brief 'O' key MAKE CODE */
#define MAKE_P 0x19 /**< @brief 'P' key MAKE CODE */
#define MAKE_Q 0x10 /**< @brief 'Q' key MAKE CODE */
#define MAKE_R 0x13 /**< @brief 'R' key MAKE CODE */
#define MAKE_S 0x1F /**< @brief 'S' key MAKE CODE */
#define MAKE_T 0x14 /**< @brief 'T' key MAKE CODE */
#define MAKE_U 0x16 /**< @brief 'U' key MAKE CODE */
#define MAKE_V 0x2F /**< @brief 'V' key MAKE CODE */
#define MAKE_W 0x11 /**< @brief 'W' key MAKE CODE */
#define MAKE_X 0x2D /**< @brief 'X' key MAKE CODE */
#define MAKE_Y 0x15 /**< @brief 'Y' key MAKE CODE */
#define MAKE_Z 0x2C /**< @brief 'Z' key MAKE CODE */

#define MAKE_0 0x0B /**< @brief '0' key MAKE CODE */
#define MAKE_1 0x02 /**< @brief '1' key MAKE CODE */
#define MAKE_2 0x03 /**< @brief '2' key MAKE CODE */
#define MAKE_3 0x04 /**< @brief '3' key MAKE CODE */
#define MAKE_4 0x05 /**< @brief '4' key MAKE CODE */
#define MAKE_5 0x06 /**< @brief '5' key MAKE CODE */
#define MAKE_6 0x07 /**< @brief '6' key MAKE CODE */
#define MAKE_7 0x08 /**< @brief '7' key MAKE CODE */
#define MAKE_8 0x09 /**< @brief '8' key MAKE CODE */
#define MAKE_9 0x0A /**< @brief '9' key MAKE CODE */

#define MAKE_SPACE 0x39 /**< @brief SPACE key MAKE CODE */
#define MAKE_ENTER 0x1C /**< @brief ENTER key MAKE CODE */
#define MAKE_BACK_SPACE 0x0E /**< @brief BACKSPACE key MAKE CODE */
#define MAKE_CTRL 0x1D /**< @brief LEFT CTRL key MAKE CODE */
#define MAKE_ESC 0x01 /**< @brief ESC key MAKE CODE */

#define MAKE_DEL 0xE053 /**< @brief DEL key MAKE CODE */

#define MAKE_ARROW_UP 0xE048 /**< @brief ARROW UP key MAKE CODE */
#define MAKE_ARROW_DOWN 0xE050 /**< @brief ARROW DOWN key MAKE CODE */
#define MAKE_ARROW_LEFT 0xE04B /**< @brief ARROW LEFT key MAKE CODE */
#define MAKE_ARROW_RIGHT 0xE04D /**< @brief ARROW RIGHT key MAKE CODE */

/**@}*/

#endif /* __SCAN_CODES_H */

