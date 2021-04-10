/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <stdint.h>
#include <cstdio>
#include <cstdarg>

#ifdef ARDUINO
    #include <Arduino.h>
#elif ESP32
    #include <FreeRTOS.h>
    extern "C" {
    #include <lwip/sockets.h>
    #include <lwip/netdb.h>
    #include <errno.h>
    }
    #include <esp_log.h>
    static const char* module_tag = "log";
#else
    #error "Architecture/Framework is not supported. Supported: ESP32 (IDF and Arduino)"
#endif

#include "logger.h"


// ***************************************************************************

const char* SerialLogHandler::_color_strings[] = 
{
    /*0:UNDEFINED*/"\u001b[0m",  // reset
    /*1:DEBUG*/    "\u001b[36m", // cyan
    /*2:INFO*/     "\u001b[32m", // green
    /*3:WARNING*/  "\u001b[33m", // yellow
    /*4:ERROR*/    "\u001b[31m", // red
    /*5:CRITICAL*/ "\u001b[35m", // magenta
};

SerialLogHandler::SerialLogHandler(bool color, unsigned long baudRate):
    _color(color)
{
    if (baudRate > 0)
    {
        Serial.begin(baudRate);
    }
}

void SerialLogHandler::write(Logger::LogLevel level, const char *tag, const char *message, int messageLength)
{
    unsigned long ms = 0;
#ifdef ARDUINO
    ms = millis();
#elif ESP32
    ms = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
#endif

    // determine color
    int color_index = ((int) level) / 10;
    if (color_index >= sizeof(_color_strings))
    {
        color_index = sizeof(_color_strings) - 1;
    }

#ifdef ARDUINO
    Serial.printf("%s%lu.%03lu:%02d:%s:%s%s\n", 
        _color ? _color_strings[color_index] : "",
        ms / 1000, ms % 1000, 
        level,
        tag == nullptr ? "" : tag,
        message,
        _color ? _color_strings[0] : "");
#elif ESP32
    printf("%s%lu.%03lu:%02d:%s:%s%s\n", 
        _color ? _color_strings[color_index] : "",
        ms / 1000, ms % 1000, 
        level,
        tag == nullptr ? "" : tag,
        message,
        _color ? _color_strings[0] : "");
#endif
}

// ***************************************************************************

Logger::Logger(const char* tagPtr, LogHandler* logHandlerPtr):
    _level(NOTSET),
    _parentLogger(nullptr),
    _tag(tagPtr), 
    _logHandlerPtr(logHandlerPtr)
{
}

Logger::Logger(const char* tagPtr, const Logger& parentLogger):
    _level(NOTSET), 
    _parentLogger(&parentLogger),
    _tag(tagPtr), 
    _logHandlerPtr(parentLogger._logHandlerPtr)
{
}

Logger::LogLevel Logger::getLevel() const
{
    LogLevel level = _level;
    const Logger* parentLogger = _parentLogger;
    while (level == NOTSET && parentLogger != nullptr)
    {
        level = parentLogger->_level;
        parentLogger = parentLogger->_parentLogger;
    }
    return level;
}

void Logger::logv(LogLevel level, const char* format, va_list ap)
{
    constexpr int buflen = 1024;
    char buffer[buflen];

    if (_logHandlerPtr == nullptr)
    {
        return;
    }

    if (level >= getLevel())
    {
        int len = vsnprintf(buffer, buflen-1, format, ap);
        _logHandlerPtr->write(level, _tag, buffer, len);
    }
}

void Logger::logf(LogLevel level, const char* format...) {
    va_list args;
    va_start(args, format);
    logv(level, format, args);
    va_end(args);
}

void Logger::critical(const char* format...) {
    va_list args;
    va_start(args, format);
    logv(CRITICAL, format, args);
    va_end(args);
}

void Logger::error(const char* format...) {
    va_list args;
    va_start(args, format);
    logv(ERROR, format, args);
    va_end(args);
}

void Logger::warn(const char* format...) {
    va_list args;
    va_start(args, format);
    logv(WARNING, format, args);
    va_end(args);
}

void Logger::info(const char* format...) {
    va_list args;
    va_start(args, format);
    logv(INFO, format, args);
    va_end(args);
}

void Logger::debug(const char* format...) {
    va_list args;
    va_start(args, format);
    logv(DEBUG, format, args);
    va_end(args);
}

// ***************************************************************************

