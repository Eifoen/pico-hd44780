# HD44780 library for Raspberry Pi PICO

>DISCLAIMER: This project is not finished yet and will be worked on within the next few days/weeks.


This library aims to simplify the usage of an HD44780 driven LCD with the Raspberry Pi PICO. It is inspired by the [LiquidCrystal library](https://github.com/arduino-libraries/LiquidCrystal) for Arduino but entirely written in plain C so that it may be used with C or CXX just like the PICO SDK. 

## Including the lib
The include pattern of this library is designed around the pico-sdk. It should be added to your project as a git submodule and the command

```cmake
include(pico-hd44780/pico_hd44780.cmake)
```

replacing `pico-hd44780/` with the path you cloned the submodule respectively. This command should be added **above** the project() command within your root `CMakeLists.txt`

## Usage
When the library is included into your project you can now link your target against `pico_hd44780` and include the headers with

```C
#include "pico_hd44780/hd44780.h"
```

## Example

This example is intended for use with a 4x20 display.

```C
#include "pico/stdlib.h"
#include "pico_hd44780/hd44780.h"

const uint LCD_RS = 16;
const uint LCD_E = 17;
const uint LCD_4 = 18;
const uint LCD_5 = 19;
const uint LCD_6 = 20;
const uint LCD_7 = 21;

int main() {
    hd44780_t lcd;
    hd44780_init(&lcd, HD44780_DISABLED_PIN, LCD_RS, LCD_E, 4, LCD_4, LCD_5, LCD_6, LCD_7);
    hd44780_begin(&lcd, 20, 4, false);
    hd44780_print(&lcd, "Hello from PICO!");
    return 0;
}
```

