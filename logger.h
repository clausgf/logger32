/**
 * Logger for 32 Bit Microcontrollers
 * Copyright (c) 2021 clausgf@github. See LICENSE.md for legal information.
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
     * @param tag  Pointer to C string containing a tag. The tag can be used later 
     *             to identify this logger. In a LogHandler, it can also be added 
     *             to the log output (like in the default SerialLogHandler does).
     * 
     *             *Warning:* The tag is not copied, so make sure that the memory
     *             managed by this pointer is always available while using 
     *             this Logger (i.e. do not put it on the stack).
     * @param logHandlerPtr  Pointer to the LogHandler which is used for output.
     */
    Logger(const char* tag, LogHandler* logHandlerPtr);

    /**
     * Construct a new Logger with a parent Logger
     * 
     * Use this constructor to derive a new Logger form a parent logger
     * like `rootLogger` or its descendants. The deviceId is always inherited
     * from the parent.
     * @param tag  Pointer to C string containing a tag. The tag can be used later 
     *             to identify this logger. In a LogHandler, it can also be added 
     *             to the log output (like in the default SerialLogHandler does).
     * 
     *             *Warning:* The tag is not copied, so make sure that the memory
     *             managed by this pointer is always available while using 
     *             this Logger (i.e. do not put it on the stack).
     * @param parentLogger  Reference to the parent logger. The LogHandler
     *                      is copied from the parent, and the log level
     *                      is derived from it if the own log level is UNSET.
     */
    Logger(const char* tag, const Logger& parentLogger);

    /**
     * Set the device Id of this logger
     * @param deviceId  Pointer to C string containing a device id.
     *                     In a LogHandler, the device id could or could not be
     *                     added to the log output (e.g. in the default 
     *                     `SyslogLogHandler`). Usually, deviceId is defined 
     *                     only for the `rootLogger`.
     * 
     *                *Warning:* The deviceId is not copied, so make sure that 
     *                the memory managed by this pointer is always available 
     *                while using this Logger (i.e. do not put it on the stack).
     */
    void setDeviceId(const char* deviceId) { _deviceId = deviceId; }

    /**
     * Get the device id for this logger.
     * 
     * If a Logger's device id is not set (nullptr), it inherits
     * it's parent logger's device is. Usually, only the root logger
     * has the devices id.
     * 
     * The device id determination is dynamic to make a change in the 
     * root logger device id visible to all children inheriting from there.
     */
    const char* getDeviceId() const;

    /**
     * Get the tag for this logger
     */
    const char* getTag() const { return _tag; }

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
     * 
     * The log level determination is dynamic to make a change in the 
     * root logger's log level visible to all children inheriting from there.
     */
    LogLevel getLevel() const;

    /**
     * Log output with given level, format and arguments referenced by ap.
     */
    void logv(LogLevel level, const char* format, va_list ap) const;

    /**
     * Log output with given level, format and printf()-style arguments.
     */
    void logf(LogLevel level, const char* format...) const;

    // --- logging helpers ---
    void critical(const char* format...) const;
    void error(const char* format...) const;
    void warn(const char* format...) const;
    void info(const char* format...) const;
    void debug(const char* format...) const;

private:
    LogLevel _level = NOTSET;
    const Logger* _parentLogger;
    const char* _deviceId;
    const char* _tag;
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
    /**
     * Construct a LogHandler 
     * @param color  If `true`, use ANSI colors in the log output.
     */
    LogHandler(bool color = true);
    virtual ~LogHandler() {}
    virtual void write(Logger::LogLevel level, const char *deviceId, const char *tag, const char *message, int messageLength) = 0;

protected:
    const char* colorStartStr(Logger::LogLevel level) const;
    const char* colorEndStr() const;

private:
    bool _color;
    static const char* _EMPTY_STRING;
    static const char* _COLOR_STRINGS[];
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

    virtual void write(Logger::LogLevel level, const char *deviceId, const char *tag, const char *message, int messageLength);

};

// ***************************************************************************

/**
 * Default logger
 */
extern Logger rootLogger;

// ***************************************************************************
