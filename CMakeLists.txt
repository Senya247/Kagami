cmake_minimum_required(VERSION 3.5)

project("Kagami")

file (GLOB SOURCES
	main.cpp
	src/*.cpp
	src/server/*.cpp
	src/client/*.cpp
)


add_executable(kagami ${SOURCES})
set(CMAKE_BUILD_TYPE Debug)

target_include_directories(kagami PRIVATE /usr/include/libevdev-1.0/)

target_link_libraries(kagami PRIVATE evdev)

target_link_libraries(kagami PRIVATE pthread)
