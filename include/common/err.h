#pragma once

/* https://stackoverflow.com/a/8488201/13261485 */
#define __FILENAME__                                                           \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

void error_exit(int err_num);
