/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <cstdint>

#include "multi_log_handler.h"


// ***************************************************************************

MultiLogHandler::MultiLogHandler():
    LogHandler(false)
{
}

void MultiLogHandler::write(Logger::LogLevel level, const char *tag, const char* format, va_list ap)
{
    for (auto &&it : _logHandlerPtrs)
    {
        if (it != nullptr)
        {
            it->write(level, tag, format, ap);
        }
    }
    
}

// ***************************************************************************

