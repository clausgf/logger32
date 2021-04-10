C++ Logger for small (but not tiny) 32 Bit Microcontrollers
===========================================================

Simple logging layer for printf() style logging. Suitable for smaller (but not tiny) embedded systems, e.g. 32 bit MCUs in the ARM Cortex M3 or ESP32 class.

Features
--------
- Creation of multiple loggers, e.g. one for each module.
- Logger hierarchy with inheritance.
- Flexible and extensible output. Currently, Serial and UDP logging are supported out of the box.
- Optional colorization of the output using ANSI terminal colors.
- Optimized for application level logging with efficiency in mind, but usability was kept in mind as an important factor too. Your 32-Bit MCU is more powerful than early PCs!
- Inspired by the Python and log4j/log4cxx logging frameworks.
- At least some documentation to get you started.

Usage
-----
Every logging configuration consists a Logger configured with 
- a LogHandler for formatting and actually generating the log output and
- a LogLevel for filtering messages with lower levels than configured in 
  the Logger.

Each module in your software should create it's own Logger instance. These Logger instances form a hierarchy used to compute a Logger's effective log level: If a Logger's own log level is `UNSET`, the parent Logger's log level is used.

A default logger named `rootLogger` is declared but not defined. You have to create and initialize an instance externally (e.g. in your main.cpp):

```cpp
#include "logger.h"
auto logHandler = SerialLogHandler(/*color*/true, /*initBaudRate*/115200);
Logger rootLogger = Logger(/*tag*/"main", &logHandler);
```

To output log messages, use `printf()` style log statements like:

```cpp
rootLogger.debug("Debug message with %d integer", intVal); 
rootLogger.error("Error message with %s c string", cString); 
```

The `rootLogger` is available in every module which includes the `logger.h` header file. It forms the root of the hierarchy of loggers. Usually, each module has its own `Logger` derrived from the `rootLogger`. In this way, its log level can be individually set for the module or derived from the root Logger.

In a module of your software, use a constuct like this to create a derived logger:
```cpp
#include "logger.h"
static auto moduleLogger("module_name", rootLogger)
```

A logger can also be attached to a class instead:
```cpp
#include "logger.h"

class MyClass {
public:
  MyClass(Logger parentLogger = rootLogger): 
    _logger("tag_name", parentLogger)
  {}
private:
  Logger _logger;
};
```

Child Loggers initially copy the LogHandler from their parents. The LogHandler of any Logger can be changed later, but these changes are not propagated along the hierarchy.
