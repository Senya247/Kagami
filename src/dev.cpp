#include "../include/dev.hpp"

#include <fcntl.h>
#include <iostream>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <string>

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

    _fd = fd;
    _dev = dev;

    _name = libevdev_get_name(dev);
    _path = path;

    return 0;
}

int Device::init_uinput(struct dev_info *dev_info) {
    _info = *dev_info;
    _id = dev_info->id;

    std::string n_name = "kagami_";
    n_name.append(dev_info->name);

    _dev = libevdev_new();
    libevdev_set_name(_dev, n_name.c_str());

    /* enable events */
    int event, code;
    for (event = 0; event < EV_MAX; event++) {
        if (test_bit(event, _info.event_info)) {
            libevdev_enable_event_type(_dev, event);
            for (code = 0; code < KEY_MAX; code++) {
                if (test_bit(code, _info.code_info[event])) {
                    libevdev_enable_event_code(_dev, event, code, NULL);
                }
            }
        }
    }

    int fd;
    fd = open("/dev/uinput", O_RDONLY);
    if (fd == -1)
        return fd;

    return libevdev_uinput_create_from_device(_dev, fd, &_uinput);
}

Device::~Device() {
    /* libevdev_uinput_destroy(_uinput);
    libevdev_free(_dev); */
    std::cout << "deconstructed Device" << std::endl;
}

int Device::uinput_create() {
    if (_uinput == NULL)
        return -1;
    return libevdev_uinput_create_from_device(
        _dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &_uinput);
}

int Device::fd() { return _fd; }

int Device::id() { return _id; }

std::string Device::name() { return _name; }

std::string Device::path() { return _path; }

int Device::event_run(const struct input_event *event) {
    return libevdev_uinput_write_event(_uinput, event->type, event->code,
                                       event->value);
}

int Device::event_read(struct r_input_event *event) {
    event->id = _id;
    return libevdev_next_event(_dev, LIBEVDEV_READ_FLAG_NORMAL,
                               &(event->event));
}

} // namespace Kagami
