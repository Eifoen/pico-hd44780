#ifndef _SRC_PICO_HD44780_hd44780_h_ 
#define _SRC_PICO_HD44780_hd44780_h_ 

#include <stdio.h>
#include "pico/stdlib.h"

// RETURNS
#define HD44780_SUCCESS 0
#define HD44780_ERROR 1
#define HD44780_INVALID 10

// CONSTANTS
#define HD44780_DISABLED_PIN 255

typedef struct {
    uint data[8];
    uint offsets[4];
    uint rs;
    uint rw;
    uint e;
    uint rows;
    uint cols;
    uint8_t functions;
    uint8_t display;
} hd44780_t;

int hd44780_init(hd44780_t *lcd, uint pin_rw, uint pin_rs, uint pin_e, int data_pins, ...);
int hd44780_begin(hd44780_t *lcd, uint cols, uint rows, bool fancy_font);
void hd44780_clear(hd44780_t *lcd);
void hd44780_home(hd44780_t *lcd);
void hd44780_display_on(hd44780_t *lcd);
void hd44780_display_off(hd44780_t *lcd);
void hd44780_cursor_on(hd44780_t *lcd);
void hd44780_cursor_off(hd44780_t *lcd);
void hd44780_blink_on(hd44780_t *lcd);
void hd44780_blink_off(hd44780_t *lcd);
void hd44780_scroll_left(hd44780_t *lcd);
void hd44780_scroll_right(hd44780_t *lcd);
void hd44780_print(hd44780_t *lcd, char* text);
int hd44780_command(hd44780_t *lcd, uint8_t v);
int hd44780_write(hd44780_t *lcd, uint8_t v);

#endif