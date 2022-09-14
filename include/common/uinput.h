#pragma once

#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>

/* create uinput device from given device */
int uinput_create_from_device(const char *path, struct libevdev_uinput **uidev);

int write_uintput_event(struct libevdev_uinput *uidev, struct input_event *ev);
