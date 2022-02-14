#ifndef TIME_UTILS_H_
#define TIME_UTILS_H_
#include <time.h>

static inline uint64_t get_current_us() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t ts = (now.tv_sec * 1000000LL) + now.tv_nsec / 1000LL;
    return ts;
}

static inline uint64_t get_current_ms() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t ts = (now.tv_sec * 1000LL) + now.tv_nsec / 1000000LL;
    return ts;
}

#endif
