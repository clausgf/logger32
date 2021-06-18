/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <cstdint>

#ifdef ARDUINO
    #include <Arduino.h>
#elif ESP32
    #include <FreeRTOS.h>
#else
    #error "Architecture/Framework is not supported. Supported: ESP32 (IDF and Arduino)"
#endif

#include "logger.h"


// ***************************************************************************

const char* LogHandler::_EMPTY_STRING = "";

const char* LogHandler::_COLOR_STRINGS[] = 
{
    /*0:UNDEFINED*/"\u001b[0m",  // reset
    /*1:DEBUG*/    "\u001b[36m", // cyan
    /*2:INFO*/     "\u001b[32m", // green
    /*3:WARNING*/  "\u001b[33m", // yellow
    /*4:ERROR*/    "\u001b[31m", // red
    /*5:CRITICAL*/ "\u001b[35m", // magenta
};

const char* LogHandler::colorStartStr(Logger::LogLevel level) const
{
    if (!_color)
    {
        return _EMPTY_STRING;
    }

    // determine color
    int color_index = ((int) level) / 10;
    if (color_index >= sizeof(_COLOR_STRINGS))
    {
        color_index = sizeof(_COLOR_STRINGS) - 1;
    }
    return _COLOR_STRINGS[color_index];
}

const char* LogHandler::colorEndStr() const
{
    if (!_color)
    {
        return _EMPTY_STRING;
    }
    return _COLOR_STRINGS[0];
}

// ***************************************************************************

SerialLogHandler::SerialLogHandler(bool color, unsigned long baudRate):
    LogHandler(color)
{
    if (baudRate > 0)
    {
        #ifdef ARDUINO
        Serial.begin(baudRate);
        #endif
    }
}

void SerialLogHandler::write(Logger::LogLevel level, const char *deviceId, const char *tag, const char *message, int messageLength)
{
    unsigned long ms = 0;
#ifdef ARDUINO
    ms = millis();
#elif ESP32
    ms = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
#endif

#ifdef ARDUINO
    Serial.print(colorStartStr(level));
    Serial.printf("%lu.%03lu:%02d:%s:%s:%s", 
        ms / 1000, ms % 1000, 
        level,
        deviceId == nullptr ? "" : deviceId,
        tag == nullptr ? "" : tag,
        message);
    Serial.println(colorEndStr());
#elif ESP32
    printf("%s%lu.%03lu:%02d:%s:%s:%s%s\n", 
        colorStartStr(level),
        ms / 1000, ms % 1000, 
        level,
        deviceId == nullptr ? "" : deviceId,
        tag == nullptr ? "" : tag,
        message,
        colorEndStr());
#endif
}

// ***************************************************************************

Logger::Logger(const char* tag, LogHandler* logHandlerPtr):
    _level(NOTSET),
    _parentLogger(nullptr),
    _deviceId(nullptr),
    _tag(tag), 
    _logHandlerPtr(logHandlerPtr)
{
}

Logger::Logger(const char* tag, const Logger& parentLogger):
    _level(NOTSET), 
    _parentLogger(&parentLogger),
    _deviceId(nullptr),
    _tag(tag), 
    _logHandlerPtr(parentLogger._logHandlerPtr)
{
}

const char* Logger::getDeviceId() const
{
    const char* deviceId = _deviceId;
    const Logger* parentLogger = _parentLogger;
    while (deviceId == nullptr && parentLogger != nullptr)
    {
        deviceId = parentLogger->_deviceId;
        parentLogger = parentLogger->_parentLogger;
    }
    return deviceId;
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

void Logger::logv(LogLevel level, const char* format, va_list ap) const
{
    constexpr int BUFLEN = 256;
    char buffer[BUFLEN];

    if (_logHandlerPtr == nullptr)
    {
        return;
    }

    if (level >= getLevel())
    {
        int len = vsnprintf(buffer, BUFLEN-1, format, ap);
        _logHandlerPtr->write(level, getDeviceId(), _tag, buffer, len);
    }
}

void Logger::logf(LogLevel level, const char* format...) const
{
    va_list args;
    va_start(args, format);
    logv(level, format, args);
    va_end(args);
}

void Logger::critical(const char* format...) const
{
    va_list args;
    va_start(args, format);
    logv(CRITICAL, format, args);
    va_end(args);
}

void Logger::error(const char* format...) const
{
    va_list args;
    va_start(args, format);
    logv(ERROR, format, args);
    va_end(args);
}

void Logger::warn(const char* format...) const
{
    va_list args;
    va_start(args, format);
    logv(WARNING, format, args);
    va_end(args);
}

void Logger::info(const char* format...) const
{
    va_list args;
    va_start(args, format);
    logv(INFO, format, args);
    va_end(args);
}

void Logger::debug(const char* format...) const
{
    va_list args;
    va_start(args, format);
    logv(DEBUG, format, args);
    va_end(args);
}

// ***************************************************************************

