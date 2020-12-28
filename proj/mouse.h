#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>

/** @defgroup mouse mouse
 * @{
 *
 * Module to interact with the PS/2 mouse.
 */

/**
 * @brief Subscribes mouse interrupts.
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe_int();

/**
 * @brief Checks if there is a mouse packet ready to be processed.
 * 
 * @return Return true if a packet is ready and false otherwise
 */
bool mouse_is_packet_ready();

/**
 * @brief Retrieves a mouse packet.
 * 
 * @param packet address of memory to be initialized with the mouse packet
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_retrieve_packet(struct packet *packet);

/**
 * @brief Issues a command to the mouse using the kbc.
 * 
 * @param cmd command to be issued
 * @return Return 0 upon success and non-zero otherwise
 */
int write_byte_to_mouse(uint8_t cmd);

/**
 * @brief Enables mouse data reporting.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_enable_dr();

/**
 * @brief Disables mouse data reporting.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_disable_dr();

/**
 * @brief Sets the mouse to stream mode.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_set_stream_mode();

/**@}*/

#endif /* __MOUSE_H */
