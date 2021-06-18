/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <cstdint>

#ifdef ARDUINO
    #include <WiFiUdp.h>
#else
    #error "Architecture/Framework is not supported. Supported: ESP32 (IDF and Arduino)"
#endif

#include "syslog_handler.h"


// ***************************************************************************

const char* LogHandler::_LEVEL_MAPPING[] = 
{
    /*0:UNDEFINED*/7, // reset
    /*1:DEBUG*/    7, // 7=debug
    /*2:INFO*/     6, // 6=info, 5=notice
    /*3:WARNING*/  4, // 4=waring
    /*4:ERROR*/    3, // 3=error
    /*5:CRITICAL*/ 2, // 2=critical, 1=alert, 0=emergency
};

SyslogHandler::SyslogHandler(bool color, String hostname, int port):
    LogHandler(color),
    _hostname(hostname),
    _port(port),
    _wifiUdp()
{
}

// syslog from https://www.rfc-editor.org/info/rfc5424
// <PRI>1 TIMESTAMP HOSTNAME APPNAME PROCID MSGID MSG
void SyslogHandler::write(Logger::LogLevel level, const char *deviceId, const char *tag, const char *message, int messageLength)
{
    unsigned long ms = millis();

    int level_index = ((int) level) / 10;
    if (level_index >= sizeof(_LEVEL_MAPPING))
    {
        level_index = sizeof(_LEVEL_MAPPING) - 1;
    }
    int pri = _FACILITY*8 + _LEVEL_MAPPING[level_index];

    const char* task = xTaskGetName(NULL);

    constexpr int BUFLEN = 256;
    char msg[BUFLEN];
    int msgLen = snprintf(msg, BUFLEN-1, 
        "<%d>1 - %s %s %s %s %lu.%03lu %s%s%s",
        pri, 
        deviceId == nullptr ? "-" : deviceId,
        tag == nullptr ? "-" : tag,
        task == NULL ? "-" : task,
        ms / 1000, ms % 1000, 
        colorStartStr(level),
        message,
        colorEndStr());

    _wifiUdp.beginPacket(_hostname.c_str(), _port);
    _wifiUdp.write((const uint8_t*) msg, msgLen);
    _wifiUdp.endPacket();
}

// ***************************************************************************

