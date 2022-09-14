#include "../../include/common/err.h"
#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>

/* create uinput device from given device */
int uinput_create_from_device(const char *path,
                              struct libevdev_uinput **uidev) {
    int fd, uifd, err;
    struct libevdev *dev;

    fd = open(path, O_RDONLY);
    if (fd < 0)
        return fd;

    err = libevdev_new_from_fd(fd, &dev);
    if (err != 0)
        return err;

    uifd = open("/dev/uinput", O_RDWR);
    if (uifd < 0)
        return errno;

    err = libevdev_uinput_create_from_device(dev, uifd, uidev);
    /* advisable to sleep for 1 second after creation, so userspace has time to
     * detect */

    return 0;
}

int write_uintput_event(struct libevdev_uinput *uidev, struct input_event *ev) {
    return libevdev_uinput_write_event(uidev, ev->type, ev->code, ev->value);
}
