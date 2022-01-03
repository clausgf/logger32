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

// syslog from https://www.rfc-editor.org/info/rfc5424
// <PRI>1 TIMESTAMP HOSTNAME APPNAME PROCID MSGID MSG
void MultiLogHandler::write(Logger::LogLevel level, const char *tag, const char* format, va_list ap)
{
    for (auto &&it : _logHandlerPtrs)
    {
        LogHandler *handlerPtr = *it;
        if (handlerPtr != nullptr)
        {
            handlerPtr->write(level, tag, format, ap);
        }
    }
    
}

// ***************************************************************************

