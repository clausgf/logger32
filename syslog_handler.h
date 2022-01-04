/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#pragma once

#include <WiFiUdp.h>

#include "logger.h"


// ***************************************************************************

/**
 * Concrete LogHandler for a syslog server via UDP
 */
class SyslogHandler: public LogHandler
{
public:
    /**
     * Construct a SyslogLogHandler 
     * @param color  If `true`, use ANSI colors in the log output.
     * @param hostname  Name or ip address of the syslog server
     * @param port  Port of the syslog server
     */
    SyslogHandler(bool color, String hostname, int port);

    virtual void write(Logger::LogLevel level, const char *tag, const char* format, va_list ap);

private:
    String _hostname;
    int _port;
    WiFiUDP _wifiUdp;
    static const int _LEVEL_MAPPING[];
    static constexpr int _FACILITY = 1;
};

// ***************************************************************************
