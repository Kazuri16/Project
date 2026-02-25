#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "config.h"

class Logger {
public:
    enum Level { ERROR = 0, WARN = 1, INFO = 2, DEBUG = 3 };

    static void init();
    static void log(Level level, const char* tag, const char* format, ...);
    static void logError(const char* tag, const char* format, ...);
    static void logWarn(const char* tag, const char* format, ...);
    static void logInfo(const char* tag, const char* format, ...);
    static void logDebug(const char* tag, const char* format, ...);

private:
    static void vlog(Level level, const char* tag, const char* format, va_list args);
    static const char* levelToString(Level level);
};

#define LOG_E(tag, fmt, ...) Logger::logError(tag, fmt, ##__VA_ARGS__)
#define LOG_W(tag, fmt, ...) Logger::logWarn(tag, fmt, ##__VA_ARGS__)
#define LOG_I(tag, fmt, ...) Logger::logInfo(tag, fmt, ##__VA_ARGS__)
#define LOG_D(tag, fmt, ...) Logger::logDebug(tag, fmt, ##__VA_ARGS__)

#endif // LOGGER_H
