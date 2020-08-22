#ifndef DEBUG_H
#define DEBUG_H

/*
 * Debugging macros, definitions and helpers
 */

#include <assert.h>
#include <stdio.h>

// asserts that the condition `x` is true
#define ASSERT(x)                                                          \
    do {                                                                   \
        if (!(x)) {                                                        \
            printf("ASSERTION TRIGGERED IN %s:%d", __FILE__, __LINE__); \
            assert(x);                                                     \
        }                                                                  \
    } while (false)

// aborts when called, to protect unreachable code from ever being called
#define ASSERT_NOT_REACHABLE()                                          \
    printf("UNREACHABLE CODE REACHED IN %s:%d", __FILE__, __LINE__); \
    assert(false)

// asserts that `x` is not NULL
#define ASSERT_NOT_NULL(x) ASSERT((x) != nullptr)

// logs message to debug output
#define debug(msg)                                                         \
    {                                                                      \
        time_t dbg_time = time(NULL);                                      \
        int dbg_secs = dbg_time % 60;                                      \
        int dbg_mins = dbg_time / 60 % 60;                                 \
        printf("[%d:%d debug] %s: ", dbg_mins, dbg_secs, __FUNCTION__); \
    }                                                                      \
    std::cout << msg << std::endl

// x as a string
#define nameof(x) #x

#endif // DEBUG_H
