#include <string.h>
#include <assert.h>

#include "xmalloc.h"

static void *xmalloc_check_oom(void *mem) {
    assert(mem && "Out of memory");
    return mem;
}

void *xzalloc(size_t len) {
    return memset(xmalloc_check_oom(malloc(len)), '\0', len);
}

void *xrealloc(void *mem, size_t len) {
    return xmalloc_check_oom(realloc(mem, len));
}

char *xstrdup(const char *str) {
    return (char *)xmalloc_check_oom(strdup(str));
}

