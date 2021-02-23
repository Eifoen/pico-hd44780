if (_PICO_HD44780_INIT)
    return()
endif ()
set(_PICO_HD44780_INIT 1)

set(PICO_HD44780_PATH ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${PICO_HD44780_PATH})
