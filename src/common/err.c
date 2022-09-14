#include "../../include/common/err.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error_exit(int err_num) {
    fprintf(stderr, "Error in file %s in function %s at line %d:\n%s\n",
            __FILENAME__, __PRETTY_FUNCTION__, __LINE__, strerror(err_num));
    exit(EXIT_FAILURE);
};
