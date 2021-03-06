#include "include/pico_hd44780/hd44780.h"
#include <stdarg.h>
#include "pico/stdlib.h"
#include "pico/time.h"


// internal functions
int _hd44780_pin_setup(hd44780_t *lcd);
int _hd44780_display_setup(hd44780_t *lcd);
int _hd44780_dimensions(hd44780_t *lcd, uint cols, uint rows);
int _hd44780_set_offsets(hd44780_t *lcd, uint row0, uint row1, uint row2, uint row3);
int _hd44780_send(hd44780_t *lcd, uint8_t v, bool rs);
void _hd44780_write_nibble(hd44780_t *lcd, uint8_t v);
void _hd44780_write_byte(hd44780_t *lcd, uint8_t v);
void _hd44780_flush(hd44780_t *lcd);

/**
 * Initialization function - needs to be called first for new hd44780_t
 * 
 * This funtion initializes all the basics of the LCD datastructure.
 * 
 * @param lcd contains a reference to an hd44780_t in which the configuration can be stored
 * @param pin_rw pin number of the R/W pin
 * @param pin_rs pin number of the register select pin
 * @param pin_e pin number of the enable pin
 * @param data_pins number of data pin (either 8 or 4)
 * @param ... gpio numbers of data_pins (LSB to MSB)
 * 
 * @return HD44780_SUCCESS on success, error code on error
 * 
 */

int hd44780_init(hd44780_t *lcd, uint pin_rw, uint pin_rs, uint pin_e, int data_pins, ...) {
    va_list sp;
    va_start(sp, data_pins);

    lcd->functions = 0;
    lcd->display = 0;
    lcd->display_mode = 0;

    lcd->e = pin_e;
    lcd->rs = pin_rs;
    lcd->rw = pin_rw;

    if(data_pins != 4 && data_pins != 8) {
        return HD44780_INVALID;
    }

    for(int i = 0; i < 8; i++) {
        lcd->data[i] = HD44780_DISABLED_PIN;
    }

    for(int i = 0; i < data_pins; i++) {
        uint pin;
        pin = va_arg(sp, uint);
        lcd->data[i] = pin;
    }

    if(data_pins == 4) {
        lcd->functions |= HD44780_4BITMODE; 
    } else {
        lcd->functions |= HD44780_8BITMODE;
    }

    return HD44780_SUCCESS;
}

/**
 * Begin - here the setup of gpio pins and initialization of the display happens
 * 
 * 
 * @param lcd contains a reference to an hd44780_t in which the configuration can be stored
 * @param cols number of display columns
 * @param rows number of display rows
 * @param fancy_font boolean if display should be initialized with 5x10 dot font
 * 
 * @return HD44780_SUCCESS on success, error code on error
 */

int hd44780_begin(hd44780_t *lcd, uint cols, uint rows, bool fancy_font) {
    int err;
    if(fancy_font) {
        lcd->functions |= HD44780_5x10DOTS;
    } else {
        lcd->functions |= HD44780_5x8DOTS;
    }

    err = _hd44780_dimensions(lcd, cols, rows);
    if( err != HD44780_SUCCESS) {
        return err;
    }

    err = _hd44780_pin_setup(lcd);
    if( err != HD44780_SUCCESS) {
        return err;
    }

    err = _hd44780_display_setup(lcd);
    if( err != HD44780_SUCCESS) {
        return err;
    }

    err = _hd44780_set_offsets(lcd, 0x00, 0x40, 0x00 + cols, 0x40 + cols);
    if( err != HD44780_SUCCESS) {
        return err;
    }

    lcd->display = HD44780_DISPLAYON;

    hd44780_display_on(lcd);
    hd44780_autoscroll_off(lcd);
    hd44780_flow_lr(lcd);
    return HD44780_SUCCESS;
}

/**
 * Clear the Display
 * 
 * @param lcd Display to clear
 */
void hd44780_clear(hd44780_t *lcd) {
    hd44780_command(lcd, HD44780_CLEARDISPLAY);
    sleep_us(2000);
}

/**
 * Set Cursor to 0, 0
 * 
 * @param lcd Display on which the cursor will be set
 */
void hd44780_home(hd44780_t *lcd) {
    hd44780_command(lcd, HD44780_RETURNHOME);
    sleep_us(2000);
}

/**
 * Turns LCD display on
 * 
 * @param lcd Display that will be turned on
 */
void hd44780_display_on(hd44780_t *lcd) {
    lcd->display |= HD44780_DISPLAYON;
    hd44780_command(lcd, HD44780_DISPLAYCONTROL | lcd->display);
}

/**
 * Turns LCD display off
 * 
 * @param lcd Display that will be turned off
 */
void hd44780_display_off(hd44780_t *lcd) {
    lcd->display &= ~HD44780_DISPLAYON;
    hd44780_command(lcd, HD44780_DISPLAYCONTROL | lcd->display);
}

/**
 * Turns LCD cursor on
 * 
 * @param lcd Display on which the cursor will be turned on
 */
void hd44780_cursor_on(hd44780_t *lcd) {
    lcd->display |= HD44780_CURSORON;
    hd44780_command(lcd, HD44780_DISPLAYCONTROL | lcd->display);
}

/**
 * Turns LCD cursor off
 * 
 * @param lcd Display on which the cursor will be turned off
 */
void hd44780_cursor_off(hd44780_t *lcd) {
    lcd->display &= ~HD44780_CURSORON;
    hd44780_command(lcd, HD44780_DISPLAYCONTROL | lcd->display);
}

/**
 * Turns LCD cursor blinking on
 * 
 * @param lcd Display on which cursor blinking will be turned on
 */
void hd44780_blink_on(hd44780_t *lcd) {
    lcd->display |= HD44780_BLINKON;
    hd44780_command(lcd, HD44780_DISPLAYCONTROL | lcd->display);
}

/**
 * Turns LCD cursor blinking off
 * 
 * @param lcd Display on which cursor blinking will be turned off
 */
void hd44780_blink_off(hd44780_t *lcd) {
    lcd->display &= ~HD44780_BLINKON;
    hd44780_command(lcd, HD44780_DISPLAYCONTROL | lcd->display);
}

void hd44780_autoscroll_on(hd44780_t *lcd) {
    lcd->display_mode |= HD44780_ENTRYSHIFTINCREMENT;
    hd44780_command(lcd, HD44780_ENTRYMODESET | lcd->display_mode);
}

void hd44780_autoscroll_off(hd44780_t *lcd) {
    lcd->display_mode &= ~HD44780_ENTRYSHIFTINCREMENT;
    hd44780_command(lcd, HD44780_ENTRYMODESET | lcd->display_mode);
}

/**
 * Set character flow to 'left to right'
 * 
 * @param lcd Display on which flow will be set
 */
void hd44780_flow_lr(hd44780_t *lcd) {
    lcd->display_mode |= HD44780_ENTRYLEFT;
    hd44780_command(lcd, HD44780_ENTRYMODESET | lcd->display_mode);
}


/**
 * Set character flow to 'right to left'
 * 
 * @param lcd Display on which flow will be set
 */
void hd44780_flow_rl(hd44780_t *lcd) {
    lcd->display_mode &= ~HD44780_ENTRYLEFT;
    hd44780_command(lcd, HD44780_ENTRYMODESET | lcd->display_mode);
}

/**
 * Scrolls display to the left
 * 
 * @param lcd Display that will be scrolled
 */
void hd44780_scroll_left(hd44780_t *lcd) {
    hd44780_command(lcd, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}


/**
 * Scrolls display to the right
 * 
 * @param lcd Display that will be scrolled
 */
void hd44780_scroll_right(hd44780_t *lcd) {
    hd44780_command(lcd, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

/**
 * Set cursor to specified position
 * 
 * @param lcd Display on which the cursor will be set
 * @param col Column where the cursor will be set
 * @param row Row where the cursor will be set
 */
void hd44780_cursor_set(hd44780_t *lcd, uint col, uint row) {
    if(row >= lcd->rows) {
        row = lcd->rows;
    }

    if(col >= lcd->cols) {
        col = lcd->cols;
    }

    hd44780_command(lcd, HD44780_SETDDRAMADDR | col + lcd->offsets[row]);
}

int _hd44780_pin_setup(hd44780_t *lcd) {
    gpio_init(lcd->e);
    gpio_set_dir(lcd->e, GPIO_OUT);
    gpio_init(lcd->rs);
    gpio_set_dir(lcd->rs, GPIO_OUT);

    if(lcd->rw != HD44780_DISABLED_PIN) {
        gpio_init(lcd->rw);
        gpio_set_dir(lcd->rw, GPIO_OUT);
    }

    for(int i = 0; i <= 7; i++) {
        if(lcd->data[i] == HD44780_DISABLED_PIN) {
            printf("skipping pin setup of pin %d\n", i);
            break;
        }
        gpio_init(lcd->data[i]);
        gpio_set_dir(lcd->data[i], GPIO_OUT);
        printf("pin %d (%d) setup as GPIO_OUT\n", i, lcd->data[i]);
    }

    return HD44780_SUCCESS;
}

int _hd44780_display_setup(hd44780_t *lcd) {
    // wait 40 msec according to HD44780 spec
    sleep_ms(50);
    gpio_put(lcd->rs, 0);
    gpio_put(lcd->e, 0);
    if(lcd->rw != HD44780_DISABLED_PIN) {
        gpio_put(lcd->rw, 0);
    }

    if(lcd->functions & HD44780_8BITMODE) {
        // TODO: init to 8-Bit
    } else {
        // init to 4-Bit
        _hd44780_write_nibble(lcd, 0x03);
        sleep_ms(5); 


        _hd44780_write_nibble(lcd, 0x03);
        sleep_ms(5); 
        

        _hd44780_write_nibble(lcd, 0x03); 
        sleep_us(150);

        _hd44780_write_nibble(lcd, 0x02);
    }

    hd44780_command(lcd, HD44780_FUNCTIONSET | lcd->functions);
}

int _hd44780_set_offsets(hd44780_t *lcd, uint row0, uint row1, uint row2, uint row3) {
    lcd->offsets[0] = row0;
    lcd->offsets[1] = row1;
    lcd->offsets[2] = row2;
    lcd->offsets[3] = row3;
    return HD44780_SUCCESS;
}

int _hd44780_dimensions(hd44780_t *lcd, uint cols, uint rows) {
    if(rows == 1) {
        lcd->functions |= HD44780_1LINE;
    } else {
        lcd->functions |= HD44780_2LINE;
    }
    return HD44780_SUCCESS;
}

void hd44780_set_char(hd44780_t *lcd, uint8_t location, uint8_t charmap[]) {
    location &= 0x7;
    hd44780_command(lcd, HD44780_SETCGRAMADDR | (location << 3));
    for(int i = 0; i < 8; i++) {
        hd44780_write(lcd, charmap[i]);
    }
}

/**
 * Prints text to display
 */
void hd44780_print(hd44780_t *lcd, char* text, size_t size) {
    int i = 0;
    for(int i = 0; i < size; i++){
        hd44780_write(lcd, text[i]);
    }
}

/**
 * Send command to display
 */
int hd44780_command(hd44780_t *lcd, uint8_t v) {
    return _hd44780_send(lcd, v, false);
}

/**
 * Write single character to display
 */
int hd44780_write(hd44780_t *lcd, uint8_t v) {
    return _hd44780_send(lcd, v, true);
}

int _hd44780_send(hd44780_t *lcd, uint8_t v, bool rs) {
    gpio_put(lcd->rs, rs);

    if(lcd->rw != HD44780_DISABLED_PIN) {
        gpio_put(lcd->rw, false);
    }

    if(lcd->functions & HD44780_8BITMODE) {
        _hd44780_write_byte(lcd, v);
    } else {
        _hd44780_write_nibble(lcd, v >> 4);
        _hd44780_write_nibble(lcd, v);
    }

    return HD44780_SUCCESS;
}

void _hd44780_write_nibble(hd44780_t *lcd, uint8_t v) {
    printf("writing nibble with v=%d\n", v);
    for(int i = 0; i <= 3; i++) {
        printf("writing %d to D%d in pin %d\n", ((v >> i) & 0x01), 4+i, lcd->data[i]);
        gpio_put(lcd->data[i], ((v >> i) & 0x01));
    }
    _hd44780_flush(lcd);
}

void _hd44780_write_byte(hd44780_t *lcd, uint8_t v) {
    for(int i = 0; i <= 8; i++) {
        gpio_put(lcd->data[i], ((v >> i) & 0x01));
    }
    _hd44780_flush(lcd);
}

void _hd44780_flush(hd44780_t *lcd) {
    gpio_put(lcd->e, false);
    sleep_us(1);
    gpio_put(lcd->e, true);
    sleep_us(1);
    gpio_put(lcd->e, false);
    sleep_us(100);
}
