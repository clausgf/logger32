/**
 * Logger32 for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 */

#include <logger.h>


class AnotherModule
{
public:
    AnotherModule(const Logger& parentLogger = rootLogger):
        _logger("anotherModule", parentLogger)
    {}

    void doSomething(int count)
    {
        // change the log level of AnotherModule
        Logger::LogLevel level = Logger::LogLevel( 10 * (count % 6) );
        _logger.critical("Changing AnotherModule log level to %d", level);
        _logger.setLevel(level);

        _logger.debug("Debug message from AnotherModule (#%d)", count);
        _logger.info("Info message from AnotherModule (#%d)", count);
        _logger.warn("Warning message from AnotherModule (#%d)", count);
        _logger.error("Error message from AnotherModule (#%d)", count);
        _logger.critical("Critical message from AnotherModule (#%d)", count);
    }

private:
    Logger _logger;
};
