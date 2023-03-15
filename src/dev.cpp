#include "../include/dev.hpp"
#include "../include/err.h"

#include <cstring>
#include <err.h>
#include <fcntl.h>
#include <iostream>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace Kagami {

int Device::init(const std::string path, int id) {
    int fd, err;
    struct libevdev *dev;

    fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
        return -1;

    err = libevdev_new_from_fd(fd, &dev);
    if (err < 0)
        return err;

    _id = id;
    _fd = fd;
    _dev = dev;

    _name = libevdev_get_name(dev);
    _path = path;

    strncpy(_info.name, _name.c_str(),
            (sizeof(_info.name) / sizeof(_info.name[0])));

    err = ioctl(fd, EVIOCGBIT(0, EV_MAX), &(_info.event_info));
    if (err < 0)
        return err;

    int event, code;
    for (event = 0; event < EV_MAX; event++) {
        if (test_bit(event, _info.event_info))
            ioctl(fd, EVIOCGBIT(event, KEY_MAX), _info.code_info[event]);
    }

    return 0;
}

int Device::init_uinput(struct dev_info *dev_info) {
    _info = *dev_info;
    _id = dev_info->id;

    _name = dev_info->name;

    std::string n_name = "kagami " + _name;

    _dev = libevdev_new();
    libevdev_set_name(_dev, n_name.c_str());

    /* enable events */
    int event, code;
    for (event = 0; event < EV_MAX; event++) {
        if (test_bit(event, _info.event_info)) {
            /* if (event == EV_LED)
                continue; */

            std::cout << "enabling event type " << event << std::endl;
            if (libevdev_enable_event_type(_dev, event) != 0)
                error_exit(errno);
            if (event == EV_SYN)
                continue; /* idk, evtest did this */
            for (code = 0; code < KEY_MAX; code++) {
                if (test_bit(code, _info.code_info[event])) {
                    std::cout << "enabling event code " << code << std::endl;
                    if (libevdev_enable_event_code(_dev, event, code, NULL) ==
                        -1)
                        error_exit(errno);
                }
            }
        }
    }

    if (libevdev_uinput_create_from_device(_dev, LIBEVDEV_UINPUT_OPEN_MANAGED,
                                           &_uinput) != 0)
        error_exit(errno);

    return 0;
}

int Device::uinput_create() {
    if (_uinput == NULL)
        return -1;
    return libevdev_uinput_create_from_device(
        _dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &_uinput);
}

int Device::fd() { return _fd; }

int Device::id() { return _id; }

int Device::dev_info(struct dev_info *info) {
    memcpy(info, &_info, sizeof(*info));
    return 0;
}

std::string Device::name() { return _name; }

std::string Device::path() { return _path; }

int Device::event_run(const struct input_event *event) {
    std::cout << "writing code " << event->code << " value " << event->value
              << std::endl;
    if (libevdev_uinput_write_event(_uinput, event->type, event->code,
                                    event->value) < 0)
        error_exit(errno);
    return 0;
}

int Device::event_read(struct r_input_event *event) {
    event->id = _id;
    return libevdev_next_event(_dev, LIBEVDEV_READ_FLAG_NORMAL,
                               &(event->event));
}

} // namespace Kagami
