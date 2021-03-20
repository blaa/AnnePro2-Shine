#ifndef LIGHT_UTILS_INCLUDED
#define LIGHT_UTILS_INCLUDED

#include "board.h"
#include "hal.h"

/*
    Structs
*/
// Struct defining an LED and its RGB color components
/*typedef struct {
  uint8_t red, green, blue;
  } led_t; */

typedef union {
    struct {
        uint8_t blue, green, red;
        uint8_t align;
    } p;
    uint8_t pv[4]; /* 0 - blue, 1 - green, 2 - red :/ */
    /* Little endian: 0xrgb in mem is b g r 0 */
    uint32_t rgb;
} led_t;

/*
    Function Signatures
*/
void setAllKeysColor(led_t *ledColors, uint32_t color);
void setModKeysColor(led_t *ledColors, uint32_t color);
void setKeyColor(led_t *key, uint32_t color);

#endif
