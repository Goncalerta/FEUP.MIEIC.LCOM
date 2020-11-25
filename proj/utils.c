#include <lcom/lcf.h>

#include <stdint.h>

#ifdef LAB3
extern uint32_t cnt;
#endif

int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
    if (lsb == NULL) 
        return 1;
    
    *lsb = (uint8_t) val;
    return 0;
}

int (util_get_MSB)(uint16_t val, uint8_t *msb) {
    if (msb == NULL) 
        return 1;
        
    *msb = (uint8_t) (val >> 8);
    return 0;
}

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
