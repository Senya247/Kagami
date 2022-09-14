#include "../../include/common/err.h"
#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <string.h>

/* create device instance */
int device_new(const char *path, struct libevdev **dev) {
    int fd, err;

    fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd == -1)
        return fd;

    err = libevdev_new_from_fd(fd, dev);
    if (err < 0)
        return err;
    return 0;
}

/* poll next event */
int device_read_event(struct libevdev *dev, struct input_event *input_event) {
    int ret = 1;
    struct input_event ev;

    while (ret == 0 || ret == 1 || ret == -EAGAIN) {
        ret = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (ret == 0) {
            memcpy(input_event, &ev, sizeof(*input_event));
            return 0;
        }
    }
    return ret;
}
