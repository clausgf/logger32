/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <cstdint>

//#ifdef ARDUINO
#include <Arduino.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
//#else
//    #error "Architecture/Framework is not supported. Supported: ESP32 (IDF and Arduino)"
//#endif

#include "syslog_handler.h"


// ***************************************************************************

const int SyslogHandler::_LEVEL_MAPPING[] = 
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
void SyslogHandler::write(Logger::LogLevel level, const char *tag, const char* format, va_list ap)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }
    unsigned long ms = millis();

    // pri = facility + level
    int level_index = ((int) level) / 10;
    if (level_index >= sizeof(_LEVEL_MAPPING))
    {
        level_index = sizeof(_LEVEL_MAPPING) - 1;
    }
    int pri = _FACILITY*8 + _LEVEL_MAPPING[level_index];

    // determine the time in (Zulu/UTC)
    time_t now;
    time(&now);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);

    // determine the task name
    const char* task = pcTaskGetTaskName(NULL);

    // create the log string
    constexpr int BUFLEN = 256;
    char msg[BUFLEN];
    int msgLen = snprintf(msg, BUFLEN-1, 
        "<%d>1 %s %s %s %s %lu.%03lu %s",
        pri, 
        time_str, 
        _deviceId == nullptr ? "-" : _deviceId,
        tag == nullptr ? "-" : tag,
        task == NULL ? "-" : task,
        ms / 1000, ms % 1000, 
        colorStartStr(level));
    msgLen += vsnprintf(&msg[msgLen], BUFLEN-1-msgLen, format, ap);
    msgLen += snprintf(&msg[msgLen], BUFLEN-1-msgLen, colorEndStr());

    //printf("%s\n", msg);
    _wifiUdp.beginPacket(_hostname.c_str(), _port);
    int bytesWritten = _wifiUdp.write((const uint8_t*) msg, msgLen);
    bool ok = _wifiUdp.endPacket();

    // Timing measurements 22-01-04 11:30:
    // - printf(), but no UDP output: 6.8 ms/call
    // - added UDP output: 6.954,6.920,7.007 ms/call
    // - UDP output only: 1.442/1.454/1.533 ms/call
}

// ***************************************************************************

