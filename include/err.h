#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>

/* https://stackoverflow.com/a/8488201/13261485 */
#define __FILENAME__                                                           \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define error_exit(err_num)                                                    \
    do {                                                                       \
        fprintf(stderr, "Error in file %s in function %s at line %d:\n%s\n",   \
                __FILENAME__, __PRETTY_FUNCTION__, __LINE__,                   \
                strerror(err_num));                                            \
        std::exit(EXIT_FAILURE);                                               \
    } while (0)

/* void error_exit(int err_num); */
