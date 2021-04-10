/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
 * 
 * Simple logging layer for printf() style logging.
 * Suitable for smaller (but not tiny) embedded systems, e.g. 32 bit MCUs
 * in the ARM Cortex M3 or ESP32 class.
 * 
 * Features
 * --------
 * - Creation of multiple loggers, e.g. one for each module.
 * - Logger hierarchie with inheritance.
 * - Flexible and extensible output. Currently, Serial and UDP logging are
 *   supported out of the box.
 * - Optional colorization of the output using ANSI terminal colors.
 * - Optimized for application level logging with efficiency in mind,
 *   but usability weighed in as an important factor too. 
 *   Your 32-Bit MCU is more powerful than early PCs.
 * - Inspired by the Python and log4j/log4cxx logging frameworks.
 * - At least some documentation to get you started.
 * 
 * Usage
 * -----
 * Every logging configuration consists a Logger configured with 
 * - a LogHandler for formatting and actually generating the log output and
 * - a LogLevel for filtering messages with lower levels than configured in 
 *   the Logger.
 * 
 * Each module in the user's software should create it's own Logger instance.
 * These Logger instances form a hierarchy used to compute a Logger's 
 * effective log level: If a Logger's own log level is UNSET, the parent
 * Logger's log level is used.
 * 
 * A default logger named rootLogger is declared but not defined. It must be 
 * intantiated initialized externally (e.g. in your main.cpp):
 * 
 * ```
 * #include "logger.h"
 * auto logHandler = SerialLogHandler(true, 115200);
 * Logger rootLogger = Logger("main", &logHandler);
 * ```
 * 
 * To output log messages, use printf style log statements like:
 * 
 * ```
 * rootLogger.debug("Debug message with %d integer", intVal); 
 * rootLogger.error("Error message with %s c string", cString); 
 * ```
 * 
 * The `rootLogger` is available in every module which includes this 
 * header file. It forms the root of the hierarchy of loggers. 
 * Usually, each module has its own `Logger` derrived from the 
 * `rootLogger`. In this way, its log level can be individually
 * set for the module or derived from the root Logger.
 * 
 * In a module of your software, use a constuct like this to
 * create a derived logger:
 * ```
 * #include "logger.h"
 * static auto moduleLogger("module_name", rootLogger)
 * ```
 * 
 * A logger can also be attached to a class instead:
 * ```
 * #include "logger.h"
 * 
 * class MyClass {
 * public:
 *   MyClass(Logger parentLogger = rootLogger): 
 *     _logger("tag_name", parentLogger)
 *   {}
 * private:
 *   Logger _logger;
 * };
 * ```
 * 
 * A LogHandler is configured when creating the root Logger. 
 * Child Loggers initially copy their parent's LogHandler. 
 * The LogHandler of any Logger can be changed later, but these changes 
 * are not propagated along the hierarchy.
 */

#pragma once

#include <cstdio>
#include <cstdarg>


// ***************************************************************************

class LogHandler;

/**
 * Logger class providing log levels and user friendly log functions.
 */
class Logger
{
public:
    enum LogLevel { CRITICAL = 50, ERROR = 40, WARNING = 30, INFO = 20, DEBUG = 10, NOTSET = 0 };

    /**
     * Construct a new Logger with no parent Logger
     * 
     * Use this constructor for your root Logger.
     * @param tagPtr  Pointer to C string containing a tag. The tag can be used later 
     *                to identify this logger. In a LogHandler, it can also be added 
     *                to the log output (like in the default SerialLogHandler does).
     *                Warning: The tag is not copied, so make sure that the memory
     *                managed by this pointer is always available while using 
     *                this Logger (i.e. do not put it on the stack).
     * @param logHandlerPtr  Pointer to the LogHandler which is used for output.
     */
    Logger(const char* tagPtr, LogHandler* logHandlerPtr);

    /**
     * Construct a new Logger with a parent Logger
     * 
     * Use this constructor to derive a new Logger form a parent logger
     * like `rootLogger` or its descendants.
     * @param tagPtr  Pointer to C string containing a tag. The tag can be used later 
     *                to identify this logger. In a LogHandler, it can also be added 
     *                to the log output (like in the default SerialLogHandler does).
     *                Warning: The tag is not copied, so make sure that the memory
     *                managed by this pointer is always available while using 
     *                this Logger (i.e. do not put it on the stack).
     * @param parentLogger  Reference to the parent logger. The LogHandler
     *                      is copied from the parent, and the log level
     *                      is derived from it if the own log level is UNSET.
     */
    Logger(const char *tagPtr, const Logger& parentLogger);

    /**
     * Set the LogHandler for this Logger.
     */
    void setLogHandlerPtr(LogHandler* logHandlerPtr)
    {
        _logHandlerPtr = logHandlerPtr;
    }

    /**
     * Get the LogHandler for this Logger.
     */
    LogHandler* getLogHandlerPtr()
    {
        return _logHandlerPtr;
    }

    /**
     * Set the tag of this logger
     * @param tagPtr  Pointer to C string containing a tag. The tag can be used later 
     *                to identify this logger. In a LogHandler, it can also be added 
     *                to the log output (like, e.g.,in the default SerialLogHandler).
     * 
     *                Warning: The tag is not copied, so make sure that the memory
     *                managed by this pointer is always available while using 
     *                this Logger (i.e. do not put it on the stack).
     */
    void setTag(const char *tag) { _tag = tag; }

    /**
     * Get the tag for this logger
     */
    const char *getTag() { return _tag; }

    /**
     * Set log level, all output with a lower level is discarded.
     */
    void setLevel(LogLevel level) { _level = level; }

    /**
     * Get the effective log level.
     * 
     * Return the effective log level of this Logger. The effective 
     * log level of a logger determines the minimum log level requred in
     * a logging statement to actually output a message. All messages with
     * a lower log level are discarded.
     * 
     * If a Logger's log level is not set
     * (which is the default, setLevel(Logger::NOTSET)),
     * the logger inherits it's parent logger's level. The ancestors
     * of the Loggers are investigated until either a log level >
     * Logger::NOTSET is found the or root logger is hit.
     */
    LogLevel getLevel() const;

    /**
     * Log output with given level, format and arguments referenced by ap.
     */
    void logv(LogLevel level, const char* format, va_list ap);

    /**
     * Log output with given level, format and printf()-style arguments.
     */
    void logf(LogLevel level, const char* format...);

    // --- logging helpers ---
    void critical(const char* format...);
    void error(const char* format...);
    void warn(const char* format...);
    void info(const char* format...);
    void debug(const char* format...);

private:
    LogLevel _level = NOTSET;
    const Logger* _parentLogger;
    const char *_tag;
    LogHandler* _logHandlerPtr;
};

// ***************************************************************************

/**
 * Abstract base class LogHandler for formatting and writing logs to some output
 * 
 * A LogHandler writes a preformatted log message to the output. It could
 * add additional information like a timestamp, the log level or a simple
 * carriage return to the message.
 * 
 * An examples for concrete LogHandler is SerialLogHandler.
 */
class LogHandler
{
public:
    virtual ~LogHandler() {}
    virtual void write(Logger::LogLevel level, const char *tag, const char *message, int messageLength) = 0;
};

// ***************************************************************************


/**
 * Concrete LogHandler for the serial interface
 */
class SerialLogHandler: public LogHandler
{
public:
    /**
     * Construct a SerialLogHandler 
     * @param color  If `true`, use ANSI colors in the log output.
     * @param baudRate  If baudRate != 0, initialize the serial interface.
     */
    SerialLogHandler(bool color = true, unsigned long baudRate = 0);

    virtual void write(Logger::LogLevel level, const char *tag, const char *message, int messageLength);

private:
    bool _color;
    static const char* _color_strings[];
};

// ***************************************************************************

/**
 * Default logger
 */
extern Logger rootLogger;

// ***************************************************************************
