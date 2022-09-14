#pragma once

#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>

/* create device instance */
int device_new(const char *path, struct libevdev **dev);

/* poll next event */
int device_read_event(struct libevdev *dev, struct input_event *input_event);
