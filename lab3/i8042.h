#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */

#define KEYBOARD_IRQ 1 /**< @brief Keyboard IRQ line */

#define DELAY_US 20000

#define ESC_BREAK_CODE 0x81

#define FIRST_BYTE_TWO_BYTE_SCANCODE 0xE0

#define KBC_CMD_REG 0x64

#define KBC_CMD_ARGUMENTS_REG 0x60

#define KBC_ST_REG 0x64

#define KBC_OUT_BUF 0x60

#define KBC_ST_IBF BIT(1)

#define KBC_OBF BIT(0)

#define KBC_PAR_ERR BIT(7)

#define KBC_TO_ERR BIT(6)

#define BREAKCODE_BIT BIT(7)

#endif /* _LCOM_I8042_H */
