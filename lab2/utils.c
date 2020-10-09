#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb != NULL) {
    *lsb = (uint8_t) val;
    return 0;
  }
  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t value_32b;
  
  int fail = sys_inb(port, &value_32b);
  if(fail) return fail;

  *value = (uint8_t) value_32b;
  return 0;
}
