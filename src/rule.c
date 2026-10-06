#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "rule.h"
#include "utils.h"
#include "xmalloc.h"

static bool parse_matcher(char *src, struct rule_matcher *out) {
    // either a number of a range
    char *dash = strchr(src, '-');
    if (!dash) {
        long n;
        if (!parse_number(src, &n, 0, INT32_MAX)) {
            return false;
        }

        out->type = RULE_MATCHER_VALUE;
        out->value = n;
        return true;
    } else if (dash == src || !*(dash + 1)) {
        fprintf(stderr, "expected number\n");
        return false;
    } else {
        *dash = '\0';
        long start, end;
        const bool res = parse_number(src, &start, 0, INT32_MAX)
                      && parse_number(dash + 1, &end, 0, INT32_MAX);
        if (!res) {
            return false;
        }

        out->type = RULE_MATCHER_RANGE;
        out->range.start = start;
        out->range.end = end;
        return true;
    }
}

static bool parse_matcher_list(char *src, struct rule_matcher_list **out) {
    // 1,2,3-6,7-9,10
    // * means "match everything"
    if (!strcmp(src, "*")) {
        *out = nullptr;
        return true;
    }

    *out = xzalloc(sizeof(**out));

    char *start = src, *pos;
    while (true) {
        pos = strchr(start, ',');
        if (pos == start) {
            fprintf(stderr, "number expected, got ,\n");
            return false;
        } else if (pos) {
            *pos = '\0';
        }

        (*out)->matchers = xrealloc((*out)->matchers, ++(*out)->len * sizeof((*out)->matchers[0]));
        if (!parse_matcher(start, &(*out)->matchers[(*out)->len - 1])) {
            return false;
        }

        start = pos + 1;
        if (!pos) {
            break;
        }
    }

    return true;
}

bool parse_rule(const char *_src, struct rule *out) {
    // type:code:val:command
    char *src = xstrdup(_src);
    char *tokens[4] = { src };

    for (int i = 0; i < 3; i++) {
        char *pos = strchr(tokens[i], ':');
        if (!pos) {
            fprintf(stderr, ": expected\n");
            return false;
        } else if (pos == tokens[i]) {
            fprintf(stderr, "empty rule segment\n");
            return false;
        }

        *pos = '\0';
        tokens[i + 1] = pos + 1;
    }
    if (!*tokens[3]) {
        fprintf(stderr, "command expected\n");
        return false;
    }

    for (int i = 0; i < 3; i++) {
        if (!parse_matcher_list(tokens[i], &out->matchers[i])) {
            return false;
        }
    }

    out->command = xstrdup(tokens[3]);
    free(src);
    return true;
}

