/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#pragma once

#include <vector>>

#include "logger.h"


// ***************************************************************************

/**
 * Concrete LogHandler for output to multiple other log handlers
 */
class MultiLogHandler: public LogHandler
{
public:
    MultiLogHandler();
    void addLogHandler(LogHandler *logHandlerPtr) { _logHandlerPtrs.add(logHandlerPtr); }

    virtual void write(Logger::LogLevel level, const char *tag, const char* format, va_list ap);

private:
    std::vector<LogHandler *> _logHandlerPtrs;
};

// ***************************************************************************
