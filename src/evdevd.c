#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <linux/input.h>

#include "rule.h"
#include "xmalloc.h"

static bool running = true;

static void on_sigchld(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

static void on_sigterm(int) {
    running = false;
}

static void run(const char *cmd, const struct input_event *ev) {
    char ts[64], type[32], code[32], value[32];

    snprintf(ts, sizeof(ts), "%ld.%lu", ev->input_event_sec, ev->input_event_usec);
    snprintf(type, sizeof(type), "%hu", ev->type);
    snprintf(code, sizeof(code), "%hu", ev->code);
    snprintf(value, sizeof(value), "%u", ev->value);

    switch (fork()) {
    case -1: // error
        fprintf(stderr, "fork() failed: %m\n");
        return;
    case 0: // child
        execlp("sh", "sh", "-c", cmd, "sh", ts, type, code, value, nullptr);
        fprintf(stderr, "exec sh failed: %m\n");
        exit(1);
    default:
    }
}

static bool match_one(unsigned n, const struct rule_matcher_list *matchers) {
    if (!matchers) {
        return true;
    }

    for (size_t i = 0; i < matchers->len; i++) {
        const struct rule_matcher *matcher = &matchers->matchers[i];
        switch (matcher->type) {
        case RULE_MATCHER_VALUE:
            if (n == matcher->value) {
                return true;
            }
            break;
        case RULE_MATCHER_RANGE:
            if (n >= matcher->range.start && n <= matcher->range.end) {
                return true;
            }
            break;
        }
    }

    return false;
}

static bool match(const struct input_event *event, const struct rule *rule) {
    return match_one(event->type, rule->matchers[RULE_MATCHER_LIST_TYPE])
        && match_one(event->code, rule->matchers[RULE_MATCHER_LIST_CODE])
        && match_one(event->value, rule->matchers[RULE_MATCHER_LIST_VALUE]);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: evdevd DEVICE RULE...\n");
        return 1;
    }

    const char *dev = argv[1];

    const unsigned n_rules = argc - 2;
    struct rule *rules = xzalloc(sizeof(rules[0]) * n_rules);

    for (unsigned i = 0; i < n_rules; i++) {
        char *rule = argv[2 + i];
        if (!parse_rule(rule, &rules[i])) {
            fprintf(stderr, "Invalid rule: %s\n", rule);
            return 1;
        }
    }

    int fd = open(dev, O_RDONLY | O_CLOEXEC | O_NOCTTY);
    if (fd < 0) {
        fprintf(stderr, "Could not open %s: %m\n", dev);
        return 1;
    }

    struct stat statbuf;
    if (!fstat(fd, &statbuf) && !S_ISCHR(statbuf.st_mode)) {
        fprintf(stderr, "Warning: %s does not appear to be a character device\n", dev);
    }

    sigaction(SIGCHLD, &(struct sigaction){
        .sa_handler = on_sigchld,
        .sa_flags = SA_NOCLDSTOP,
    }, nullptr);

    sigaction(SIGTERM, &(struct sigaction){
        .sa_handler = on_sigterm,
        .sa_flags = SA_RESETHAND,
    }, nullptr);
    sigaction(SIGINT, &(struct sigaction){
        .sa_handler = on_sigterm,
        .sa_flags = SA_RESETHAND,
    }, nullptr);

    struct input_event events[32];
    while (running) {
        ssize_t ret = read(fd, events, sizeof(events));
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            fprintf(stderr, "Read failed: %m\n");
            return 1;
        }

        for (unsigned i = 0; i < ret / sizeof(events[0]); i++) {
            const struct input_event *ev = &events[i];
            for (unsigned j = 0; j < n_rules; j++) {
                const struct rule *rule = &rules[j];

                if (match(ev, rule)) {
                    run(rule->command, ev);
                }
            }
        }
    }

    return 0;
}

