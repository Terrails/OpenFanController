#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"

// Log levels
typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} LogLevel;

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#ifndef LOG_NAME
#define LOG_NAME "UNKNOWN"
#endif

#define LOG(level, fmt, ...) \
    do { \
        if (level <= LOG_LEVEL) { \
            logger_send(level, LOG_NAME, fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define LOG_ERROR(fmt, ...) LOG(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

void logger_send(LogLevel level, const char* name, const char* fmt, ...);

#endif