#pragma once

#include <stddef.h>

enum rule_matcher_type {
    RULE_MATCHER_VALUE,
    RULE_MATCHER_RANGE,
};

struct rule_matcher {
    enum rule_matcher_type type;
    union {
        unsigned value;
        struct {
            unsigned start, end;
        } range;
    };
};

struct rule_matcher_list {
    size_t len;
    struct rule_matcher *matchers;
};

enum rule_matcher_list_type {
    RULE_MATCHER_LIST_TYPE,
    RULE_MATCHER_LIST_CODE,
    RULE_MATCHER_LIST_VALUE,
};

struct rule {
    // NULL -> accept everything
    struct rule_matcher_list *matchers[3];
    const char *command;
};

bool parse_rule(const char *src, struct rule *out);

