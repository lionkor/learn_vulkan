#ifndef DEBUG_H
#define DEBUG_H

/*
 * Debugging macros, definitions and helpers
 */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

FILE* g_debug_file = NULL;
bool g_debug_system_started = false;

// prints to stdout and the debug file
#define dbgprintf(...)                      \
    assert(g_debug_system_started == true); \
    fprintf(stdout, __VA_ARGS__);           \
    fprintf(g_debug_file, __VA_ARGS__)


static void get_time_string(char* buffer) {
    time_t now = time(NULL);
    struct tm* info = localtime(&now);
    char* temp = asctime(info);
    size_t len = strlen(temp);
    strcpy(buffer, asctime(info));
    buffer[len - 1] = '\0'; // remove trailing newline
}

static void start_debug_system() {
    assert(g_debug_system_started == false);
    g_debug_file = fopen("debug.log", "w+");
    if (g_debug_file == nullptr) {
        printf("ERROR: Could not open debug file. Will write to stderr instead.\n");
        g_debug_file = stderr;
    }
    g_debug_system_started = true;
    char time[64];
    get_time_string(time);
    dbgprintf("[%s] debug system started\n", time);
}

static void stop_debug_system() {
    assert(g_debug_system_started == true);
    char time[64];
    get_time_string(time);
    dbgprintf("[%s] debug system stopped\n", time);
    if (g_debug_file != stderr) {
        fclose(g_debug_file);
    }
    g_debug_system_started = false;
}

// asserts that the condition `x` is true
#define ASSERT(x)                                                          \
    do {                                                                   \
        if (!(x)) {                                                        \
            dbgprintf("ASSERTION TRIGGERED IN %s:%d", __FILE__, __LINE__); \
            assert(x);                                                     \
        }                                                                  \
    } while (false)

// aborts when called, to protect unreachable code from ever being called
#define ASSERT_NOT_REACHABLE()                                          \
    dbgprintf("UNREACHABLE CODE REACHED IN %s:%d", __FILE__, __LINE__); \
    assert(false)

// asserts that `x` is not NULL
#define ASSERT_NOT_NULL(x) ASSERT((x) != nullptr)

// logs message to debug output
#define debug(...)                                                         \
    {                                                                      \
        time_t dbg_time = time(NULL);                                      \
        int dbg_secs = dbg_time % 60;                                      \
        int dbg_mins = dbg_time / 60 % 60;                                 \
        dbgprintf("[%d:%d debug] %s: ", dbg_mins, dbg_secs, __FUNCTION__); \
    }                                                                      \
    dbgprintf(__VA_ARGS__)

// x as a string
#define nameof(x) #x

#endif // DEBUG_H
