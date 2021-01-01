#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @file 
 * @brief Constants for programming the i8042 Keyboard controller (KBC).
 */

/** @defgroup i8042 i8042
 * @{
 *
 * @brief Constants for programming the i8042 Keyboard controller (KBC).
 */

#define KEYBOARD_IRQ 1 /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */

#define FIRST_BYTE_TWO_BYTE_SCANCODE 0xE0 /**< @brief First byte of a two bytes long scan code */
#define BREAK_CODE_BIT BIT(7) /**< @brief Bit that signals that a given scan code is a break code */

#define ESC_BREAK_CODE 0x81 /**< @brief ESC key BREAK CODE */

#define KBC_OUT_BUF 0x60 /**< @brief KBC output buffer */
#define KBC_ARG_REG 0x60 /**< @brief KBC argument register */
#define KBC_ST_REG 0x64 /**< @brief KBC status register */
#define KBC_CMD_REG 0x64 /**< @brief KBC command register */

// KBC COMMANDS
#define CMD_READ_BYTE 0x20 /**< @brief Read KBC command byte */
#define CMD_WRITE_BYTE 0x60 /**< @brief Write KBC command byte */
#define CMD_CHECK_KBC 0xAA /**< @brief KBC Self-Test */
#define CMD_CHECK_IF 0xAB /**< @brief KBC Interface-Test */
#define CMD_DISABLE_IF 0xAD /**< @brief KBC disable keyboard feature */
#define CMD_ENABLE_IF 0xAE /**< @brief KBC enable keyboard interface */

// MOUSE COMMANDS
#define CMD_DISABLE_MS 0xA7 /**< @brief Inhibits the pointing device */
#define CMD_ENABLE_MS 0xA8 /**< @brief  De-inhibits the pointing device */
#define CMD_CHECK_MS_IF 0xA9 /**< @brief Mouse Interface-Test */
#define CMD_WRITE_BYTE_TO_MS 0xD4 /**< @brief Write byte to mouse */

// KBC Command byte
#define CMD_BYTE_DISABLE_MS BIT(5) /**< @brief Disable mouse */
#define CMD_BYTE_DISABLE_KBD_IF BIT(4) /**< @brief Disable keyboard feature */
#define CMD_BYTE_ENABLE_MS_INT BIT(1) /**< @brief Enable mouse interrupts */
#define CMD_BYTE_ENABLE_KBD_INT BIT(0) /**< @brief Enable keyboard interrupts */

// STATUS REGISTER
#define KBC_ST_IBF BIT(1) /**< @brief Input buffer status */
#define KBC_OBF BIT(0) /**< @brief Output buffer status */
#define KBC_AUX BIT(5) /**< @brief Auxiliar output buffer full */
#define KBC_PAR_ERR BIT(7) /**< @brief Status register parity error */
#define KBC_TO_ERR BIT(6) /**< @brief Status register time out error */

// MOUSE PACKETS
#define MS_PACKET_LB BIT(0) /**< @brief Mouse packet left button */
#define MS_PACKET_RB BIT(1) /**< @brief Mouse packet right button */
#define MS_PACKET_MB BIT(2) /**< @brief Mouse packet middle button */
#define MS_PACKET_XDELTA_MSB BIT(4) /**< @brief Mouse packet MSB X delta */
#define MS_PACKET_YDELTA_MSB BIT(5) /**< @brief Mouse packet MSB Y delta */
#define MS_PACKET_XOVFL BIT(6) /**< @brief Mouse packet X overflow */
#define MS_PACKET_YOVFL BIT(7) /**< @brief Mouse packet Y overflow */

#define MS_ENABLE_DATA_REPORTING 0xF4 /**< @brief Enable mouse data reporting */
#define MS_DISABLE_DATA_REPORTING 0xF5 /**< @brief Disable mouse data reporting */
#define MS_READ_DATA 0xEB /**< @brief Read data */
#define MS_SET_STREAM_MODE 0xEA /**< @brief Sets mouse stream mode */

#define ACK 0xFA /**< @brief Acknowledge (everything OK) */
#define NACK 0xFE /**< @brief Non-acknowledge (invalid byte) */
#define ERROR 0xFC /**< @brief Error (second consecutive invalid byte) */

#define KBC_DELAY_US 20000 /**< @brief Keyboard response time */

/**@}*/

#endif /* _LCOM_I8042_H */
