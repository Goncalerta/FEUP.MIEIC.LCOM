#include <lcom/lcf.h>
#include "kbc.h"

#define KBC_NUM_TRIES 5

int kbc_write_reg(int reg, uint8_t write) {
    uint8_t stat;
    for (int i = 0; i < KBC_NUM_TRIES; i++) {
        if(util_sys_inb(KBC_ST_REG, &stat) != OK) 
            return 1;

        /* loop while 8042 input buffer is not empty */
        if( (stat & KBC_ST_IBF) == 0 ) {
            if (sys_outb(reg, write) != OK)
                return 1;
            return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
    return 1;
}

int kbc_issue_command(uint8_t cmd) {
    return kbc_write_reg(KBC_CMD_REG, cmd);
}

int kbc_issue_argument(uint8_t arg) {
    return kbc_write_reg(KBC_ARG_REG, arg);
}

int kbc_read_data(uint8_t *data) {
    uint8_t stat;
    // for (int i = 0; i < KBC_NUM_TRIES; i++) {
        if (util_sys_inb(KBC_ST_REG, &stat) != OK)
            return 1;
        // TODO Is reading KBC_OBF and KBC_AUX necessary?
        //if ( (stat & KBC_OBF) && ((stat & KBC_AUX) == 0) ) {
            if (util_sys_inb(KBC_OUT_BUF, data) != OK)
                return 1;
                
            if ( (stat & (KBC_PAR_ERR | KBC_TO_ERR )) ) {
                return 1;
            } else {
                return 0;
            }
        //}
        // tickdelay(micros_to_ticks(DELAY_US));
    // }
    // return 1;
}

int kbc_read_command_byte(uint8_t *command_byte) {
    if (kbc_issue_command(CMD_READ_BYTE) != OK)
        return 1;
    if (util_sys_inb(KBC_OUT_BUF, command_byte) != OK)
        return 1;
    return 0;
}

int kbc_write_command_byte(uint8_t command_byte) {
    if (kbc_issue_command(CMD_WRITE_BYTE) != OK)
        return 1;
    if (sys_outb(KBC_ARG_REG, command_byte) != OK)
        return 1;
    
    return 0;
}

int kbc_flush() {
    uint8_t stat, discard;
    if (util_sys_inb(KBC_ST_REG, &stat) != OK)
        return 1;

    if (stat & KBC_OBF) {
        if (util_sys_inb(KBC_OUT_BUF, &discard) != OK)
            return 1;
    }
    
    return 0;
}
