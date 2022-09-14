cmake_minimum_required(VERSION 3.5)

project("Kagami")

file(GLOB CLIENT_SOURCES
	src/common/*
	src/client/main.c
	src/client/helper.c
)

file (GLOB SERVER_SOURCES
	src/common/*
	src/server/main.c
	src/server/helper.c
)

add_executable(client ${CLIENT_SOURCES})
add_executable(server ${SERVER_SOURCES})

target_include_directories(server PRIVATE /usr/include/libevdev-1.0/)
target_include_directories(client PRIVATE /usr/include/libevdev-1.0/)

target_link_libraries(server PRIVATE evdev)
target_link_libraries(client PRIVATE evdev)
