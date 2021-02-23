cmake_minimum_required(VERSION 3.13)

project(pico_hd44780 C)

set(PICO_LIB_NAME "pico_hd44780")

add_library(${PICO_LIB_NAME} INTERFACE)
target_include_directories(${PICO_LIB_NAME} INTERFACE  ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(${PICO_LIB_NAME} INTERFACE
    "hd44780.c"
)
target_link_libraries(${PICO_LIB_NAME} INTERFACE
    pico_stdlib
    pico_time
)