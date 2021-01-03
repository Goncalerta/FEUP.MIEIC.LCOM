#include <lcom/lcf.h>

#include <stdint.h>

/** @defgroup utils utils
 * @{
 *
 * @brief Utilities module from lab2.
 */

#ifdef LAB3
extern uint32_t cnt;
#endif

/**
 * @brief Gets the least significant byte of a given half-word
 * 
 * @param val half-word to get the LSB from
 * @param lsb memory address to be initialized with the LSB
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
    if (lsb == NULL) 
        return 1;
    
    *lsb = (uint8_t) val;
    return 0;
}

/**
 * @brief Gets the most significant byte of a given half-word
 * 
 * @param val half-word to get the MSB from
 * @param msb memory address to be initialized with the MSB
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb) {
    if (msb == NULL) 
        return 1;
        
    *msb = (uint8_t) (val >> 8);
    return 0;
}

/**
 * @brief Wraps sys_inb so that it may be called with a uint8_t argument.
 * 
 * @param port port to execute the call to
 * @param value memory address to be initialized with the result from the call to sys_inb
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_sys_inb)(int port, uint8_t *value) {
    uint32_t value_32b;
    
    if(sys_inb(port, &value_32b) != OK) 
        return 1;

    *value = value_32b & 0xff;
    
#ifdef LAB3
    cnt++;
#endif

    return 0;
}

/**@}*/
