#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

bool parse_number(char *src, long *out, long min, long max) {
    errno = 0;
    char *endptr = nullptr;
    long n = strtol(src, &endptr, 0);

    if (errno || !endptr || *endptr) {
        fprintf(stderr, "invalid number: %s\n", src);
        return false;
    } else if (n < min || n > max) {
        fprintf(stderr, "number out of range [%ld, %ld]: %ld\n", min, max, n);
        return false;
    }

    *out = n;
    return true;
}

