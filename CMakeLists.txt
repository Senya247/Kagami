cmake_minimum_required(VERSION 3.5)

project("TCP Keyboard")

set(SOURCES
	src/main.c
)

add_executable(prog ${SOURCES})

target_include_directories(prog PRIVATE /usr/include/libevdev-1.0/)

find_library(LIBEVDEV_LIB libevdev HINTS /usr/lib/x86_64-linux-gnu/)

target_link_libraries(prog PRIVATE LIBEVDEV_LIB)
