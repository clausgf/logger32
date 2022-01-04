/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include <logger.h>
#include <syslog_handler.h>
#include <multi_log_handler.h>

#include "secrets.h"


static const char* SYSLOG_HOSTNAME = "192.168.178.20";
static const int SYSLOG_PORT = 10000; // 8514;
static const char* NTP_SERVER = "pool.ntp.org";

auto serialHandler = SerialLogHandler( /*color*/true, /*baudRate*/115200 );
auto syslogHandler = SyslogHandler( /*color*/true, SYSLOG_HOSTNAME, SYSLOG_PORT );
auto multiLogHandler = MultiLogHandler();
Logger rootLogger = Logger( /*tag*/"main", &multiLogHandler );
String myHostname;


// ***************************************************************************
//             SETUP
// ***************************************************************************

void setup()
{
    delay(500);  // wait for serial interface to get up

    Serial.println("----------------------------------------------");
    Serial.println("Startup");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nConnected to %s\n", WIFI_SSID);
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());

    configTime(0, 0, NTP_SERVER);

    const int ID_MAXLEN = 20;
    char id_buf[ID_MAXLEN];
    snprintf(id_buf, ID_MAXLEN-1, "e32-%06llx", ESP.getEfuseMac());
    myHostname = String(id_buf);
    Serial.printf("Hostname: %s\n", myHostname.c_str());

    multiLogHandler.addLogHandler(&syslogHandler);
    multiLogHandler.addLogHandler(&serialHandler);
    
    Serial.println("----------------------------------------------");
    Serial.println("Finished startup");
    Serial.println("----------------------------------------------");
    delay(1000);

}


// ***************************************************************************
//              LOOP
// ***************************************************************************

static int counter = 0;

void loop()
{
    unsigned long startTime = micros();
    rootLogger.debug("This is debug message %d from the root logger", counter);
    rootLogger.info("This is info message %d", counter);
    rootLogger.warn("This is warn message %d", counter);
    rootLogger.error("This is error message %d", counter);
    rootLogger.critical("This is critical message %d", counter);
    unsigned long endTime = micros();
    rootLogger.debug("Duration per call for 5 calls: %0.3f ms", (endTime-startTime)/5.0/1000.0);

    counter++;
    rootLogger.info("Sleeping a while...");
    delay(10*1000);
}
