#include "logger.h"
#include "usb_cdc.h"

// Log level strings
static const char* log_level_str[] = {
    [LOG_LEVEL_ERROR] = "ERROR",
    [LOG_LEVEL_INFO]  = "INFO",
    [LOG_LEVEL_DEBUG] = "DEBUG"
};

// Send a log message over Serial
void logger_send(LogLevel level, const char* name, const char* fmt, ...) {
    char buffer[256]; // Adjust size as needed
    va_list args;
    va_start(args, fmt);

    // Format: "[LEVEL] [NAME] message"
    int len = snprintf(buffer, sizeof(buffer), "[%s] [%s] ", log_level_str[level], name);
    vsnprintf(buffer + len, sizeof(buffer) - len, fmt, args);
    va_end(args);

    // Ensure newline at the end
    strcat(buffer, "\r\n");

    // Send over CDC
    usb_cdc_send_str(buffer);
}
