#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>

void *xzalloc(size_t len);
void *xrealloc(void *mem, size_t len);
char *xstrdup(const char *str);

