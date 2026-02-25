#include "logger.h"
#include <cstdarg>

void Logger::init() {
    Serial.begin(115200);
    delay(100);
    LOG_I("LOGGER", "Logger initialized");
}

void Logger::log(Level level, const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(level, tag, format, args);
    va_end(args);
}

void Logger::logError(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(ERROR, tag, format, args);
    va_end(args);
}

void Logger::logWarn(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(WARN, tag, format, args);
    va_end(args);
}

void Logger::logInfo(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(INFO, tag, format, args);
    va_end(args);
}

void Logger::logDebug(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(DEBUG, tag, format, args);
    va_end(args);
}

void Logger::vlog(Level level, const char* tag, const char* format, va_list args) {
#if LOG_ENABLE
    if (level > LOG_LEVEL) return;

    char timestamp[32];
    uint32_t ms = millis();
    snprintf(timestamp, sizeof(timestamp), "[%06lu]", ms);

    Serial.print(timestamp);
    Serial.print(" [");
    Serial.print(levelToString(level));
    Serial.print("] ");
    Serial.print(tag);
    Serial.print(": ");

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
#endif
}

const char* Logger::levelToString(Level level) {
    switch (level) {
        case ERROR: return "ERR";
        case WARN:  return "WRN";
        case INFO:  return "INF";
        case DEBUG: return "DBG";
        default:   return "UNK";
    }
}
