#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */

#define KEYBOARD_IRQ 1 /**< @brief Keyboard IRQ line */

#define FIRST_BYTE_TWO_BYTE_SCANCODE 0xE0
#define BREAKCODE_BIT BIT(7)

#define ESC_BREAK_CODE 0x81

#define KBC_OUT_BUF 0x60
#define KBC_CMD_ARGUMENTS_REG 0x60
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64

#define CMD_READ_BYTE 0x20
#define CMD_WRITE_BYTE 0x60
#define CMD_CHECK_KBC 0xAA
#define CMD_CHECK_IF 0xAB
#define CMD_DISABLE_IF 0xAD
#define CMD_ENABLE_IF 0xAE

#define CMD_BYTE_DISABLE_KBD_IF BIT(4)
#define CMD_BYTE_ENABLE_KBD_INT BIT(0)

#define KBC_ST_IBF BIT(1)
#define KBC_OBF BIT(0)
#define KBC_AUX BIT(5)
#define KBC_PAR_ERR BIT(7)
#define KBC_TO_ERR BIT(6)

#define DELAY_US 20000

#endif /* _LCOM_I8042_H */
