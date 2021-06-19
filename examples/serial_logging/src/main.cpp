/**
 * Loggerdfdfdfddsfadsfadsf for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <logger.h>


auto logHandler = SerialLogHandler( 115200 );
Logger rootLogger = Logger( "main", &logHandler );
Logger childLogger = Logger( "child", rootLogger );


// ***************************************************************************
//             SETUP
// ***************************************************************************

void setup()
{
    rootLogger.setDeviceId("MyDeviceId");
    Serial.println("----------------------------------------------");
    Serial.println("Finished startup");
    Serial.println("----------------------------------------------");
}


// ***************************************************************************
//              LOOP
// ***************************************************************************

static int counter = 0;

void loop()
{
    Logger::LogLevel level = Logger::LogLevel(10*(counter%6));
    Serial.printf("Changing childLogger log level to %d\n", level);
    childLogger.setLevel(level);

    rootLogger.debug("This is debug message #d from the root logger", counter);
    rootLogger.info("This is info message #d", counter);
    rootLogger.warn("This is warn message #d", counter);
    rootLogger.error("This is error message #d", counter);
    rootLogger.critical("This is critical message #d", counter);

    childLogger.debug("This is debug message #d from the root logger", counter);
    childLogger.info("This is info message #d", counter);
    childLogger.warn("This is warn message #d", counter);
    childLogger.error("This is error message #d", counter);
    childLogger.critical("This is critical message #d", counter);

    delay(1000);
}
