cmake_minimum_required(VERSION 3.5)

project("TCP Keyboard")

set(CLIENT_SOURCES
	src/client/helper.c
)

set (SERVER_SOURCES
	src/server/main.c
	src/server/helper.c
)

add_executable(client ${CLIENT_SOURCES})
add_executable(server ${SERVER_SOURCES})

target_include_directories(server PRIVATE /usr/include/libevdev-1.0/)
target_include_directories(client PRIVATE /usr/include/libevdev-1.0/)

target_link_libraries(server PRIVATE evdev)
target_link_libraries(client PRIVATE evdev)
