/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <logger.h>

#include "another_module.h"


auto logHandler = SerialLogHandler( /*color*/true, /*baudRate*/115200 );
Logger rootLogger = Logger( /*tag*/"main", &logHandler );

auto anotherModule = AnotherModule();


// ***************************************************************************
//             SETUP
// ***************************************************************************

void setup()
{
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
    unsigned long startTime = micros();
    rootLogger.debug("This is debug message %d from the root logger", counter);
    rootLogger.info("This is info message %d", counter);
    rootLogger.warn("This is warn message %d", counter);
    rootLogger.error("This is error message %d", counter);
    rootLogger.critical("This is critical message %d", counter);
    unsigned long endTime = micros();
    rootLogger.debug("Duration per call for 5 calls: %0.3f ms", (endTime-startTime)/5.0/1000.0);

    anotherModule.doSomething(counter);

    counter++;
    rootLogger.info("Sleeping a while...");
    delay(5000);
}
