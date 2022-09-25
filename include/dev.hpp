#pragma once

#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <string>

/* TODO undef these */
#ifndef BITS_PER_LONG
#define BITS_PER_LONG (sizeof(long) * 8)
#endif

#ifndef NBITS
#define NBITS(x) ((((x)-1) / BITS_PER_LONG) + 1)
#endif

#ifndef OFF
#define OFF(x) ((x) % BITS_PER_LONG)
#endif

#ifndef BIT
#define BIT(x) (1UL << OFF(x))
#endif

#ifndef LONG
#define LONG(x) ((x) / BITS_PER_LONG)
#endif

#ifndef test_bit
#define test_bit(bit, array) ((array[LONG(bit)] >> OFF(bit)) & 1)
#endif

namespace Kagami {

/* wrapper for with id to identify which uinput device to run event on  */
struct r_input_event {
    int id;
    struct input_event event;
};

class Device {
  public:
    struct dev_info {
        char name[256];
        int id;

        unsigned long event_info[NBITS(EV_MAX)]; /* each bit holds info */
        unsigned long code_info[EV_MAX][NBITS(KEY_MAX)];
    };

  private:
    int _fd = -1;
    int _id = -1;
    struct dev_info _info;

    std::string _name;
    std::string _path;

    struct libevdev *_dev;
    struct libevdev_uinput *_uinput; /* only used if this device is a uinput
                                       device, not a real one */

  public:
    ~Device();

    int init(const std::string path, int id);
    int init_uinput(struct dev_info *dev_info);

    int fd();
    std::string name();
    std::string path();

    int set_id();
    int id();

    int event_read(struct r_input_event *event);
    int event_run(const struct input_event *event); /* only for uinput */

    int uinput_create();
};

} // namespace Kagami
