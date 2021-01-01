#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>

/** @file 
 * @brief File dedicated to interaction with the keyboard controller (KBC).
 */

/** @defgroup kbc kbc
 * @{
 *
 * @brief Module to interact with the keyboard controller (KBC).
 */

/**
 * @brief Writes a byte to a kbc register.
 * 
 * @param reg register of kbc to write on
 * @param write byte to write
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_write_reg(int reg, uint8_t write);

/**
 * @brief Issues a command to the kbc.
 * 
 * @param cmd command to issue
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_issue_command(uint8_t cmd);

/**
 * @brief Issues an argument to the kbc.
 * 
 * @param arg argument to issue
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_issue_argument(uint8_t arg);

/**
 * @brief Reads data from the kbc output buffer.
 * 
 * @param data address of memory to be initialized with the data that was read
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_read_data(uint8_t *data);

/**
 * @brief Reads the kbc command byte.
 * 
 * @param byte address of memory to be initialized with the command byte
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_read_command_byte(uint8_t *byte);

/**
 * @brief Writes the kbc command byte.
 * 
 * @param byte command byte
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_write_command_byte(uint8_t byte);

/**
 * @brief Flushes the kbc output buffer.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_flush();

/**@}*/

#endif /* __KBC_H */
