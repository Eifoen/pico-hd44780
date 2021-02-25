#ifndef _SRC_PICO_HD44780_hd44780_h_ 
#define _SRC_PICO_HD44780_hd44780_h_ 

#include <stdio.h>
#include "pico/stdlib.h"

// cmd types
#define HD44780_CLEARDISPLAY 0x01
#define HD44780_RETURNHOME 0x02
#define HD44780_ENTRYMODESET 0x04
#define HD44780_DISPLAYCONTROL 0x08
#define HD44780_CURSORSHIFT 0x10
#define HD44780_FUNCTIONSET 0x20
#define HD44780_SETCGRAMADDR 0x40
#define HD44780_SETDDRAMADDR 0x80

// display stuff
#define HD44780_DISPLAYON 0x04
#define HD44780_CURSORON 0x02
#define HD44780_BLINKON 0x01

// scrolling
#define HD44780_DISPLAYMOVE 0x08
#define HD44780_CURSORMOVE 0x00
#define HD44780_MOVERIGHT 0x04
#define HD44780_MOVELEFT 0x00
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYLEFT 0x02

// functions
#define HD44780_8BITMODE 0x10
#define HD44780_4BITMODE 0x00
#define HD44780_2LINE 0x08
#define HD44780_1LINE 0x00
#define HD44780_5x10DOTS 0x04
#define HD44780_5x8DOTS 0x00

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
    uint8_t display_mode;
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
void hd44780_autoscroll_on(hd44780_t *lcd);
void hd44780_autoscroll_off(hd44780_t *lcd);
void hd44780_flow_lr(hd44780_t *lcd);
void hd44780_flow_rl(hd44780_t *lcd);
void hd44780_scroll_left(hd44780_t *lcd);
void hd44780_scroll_right(hd44780_t *lcd);
void hd44780_cursor_set(hd44780_t *lcd, uint col, uint row);
void hd44780_set_char(hd44780_t *lcd, uint8_t location, uint8_t charmap[]);
void hd44780_print(hd44780_t *lcd, char* text, size_t size);
int hd44780_command(hd44780_t *lcd, uint8_t v);
int hd44780_write(hd44780_t *lcd, uint8_t v);

#endif