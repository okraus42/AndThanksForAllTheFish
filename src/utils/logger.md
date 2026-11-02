# Logger Usage Guide

A small C++20 logger with configurable verbosity, elapsed-time timestamps, optional source locations, and optional ANSI console colors.

## Requirements

- C++20
- A compiler and standard library with `std::format` support
- `logger.hpp` and `logger.cpp` included in your project

## Project layout

```text
my_project/
├── CMakeLists.txt
├── include/
│   └── logger.hpp
└── src/
    ├── logger.cpp
    ├── main.cpp
    └── player.cpp
```

## Initialize early

Initialize the logger once near the beginning of `main()`, before starting worker threads or calling code that logs.

```cpp
#include "logger.hpp"

int main() {
    Logger::Config config;
    config.level = Logger::Level::Debug;
    config.timestamp = Logger::TimestampPrecision::Milliseconds;
    config.show_source = true;
    config.colors = true;

    Logger::instance().init(config);

    LOG_INFO("Application started");
    LOG_DEBUG("Loaded {} resources", 42);

    // Start the rest of your application here.
}
```

The elapsed-time clock starts when `init()` is called.

## Log from any source file

Include the header in any `.cpp` file that needs logging. Compile and link `logger.cpp` exactly once as part of your application.

```cpp
#include "logger.hpp"

void update_player(float x, float y) {
    LOG_DEBUG("Player position: x={}, y={}", x, y);
    LOG_TRACE("Entering update_player()");
}
```

Use `{}` placeholders to format values:

```cpp
LOG_INFO("Loaded {} textures in {:.2f} seconds", count, elapsed);
LOG_WARN("Missing optional file '{}'", filename);
LOG_ERROR("Initialization failed with code {}", error_code);
```

Formatting follows `std::format` rules. Format strings are checked at compile time where supported by the standard library.

## Log levels

The levels, from least verbose to most verbose, are:

| Configured level | Messages shown |
| --- | --- |
| `Off` | No messages |
| `Error` | Errors and warnings |
| `Warn` | Errors and warnings |
| `Notice` | Errors, warnings, and notices |
| `Info` | Errors, warnings, notices, and info |
| `Debug` | Above plus debug messages |
| `Verbose` | Above plus verbose messages |
| `Trace` | All messages |

`ERROR` and `WARN` are always enabled for every setting except `Off`.

Examples:

```cpp
Logger::instance().set_level(Logger::Level::Info);
```

This shows `ERROR`, `WARN`, `NOTICE`, and `INFO`, while suppressing `DEBUG`, `VERBOSE`, and `TRACE`.

To enable all diagnostics:

```cpp
Logger::instance().set_level(Logger::Level::Trace);
```

To silence all logging:

```cpp
Logger::instance().set_level(Logger::Level::Off);
```

## Configuration options

### Timestamp precision

```cpp
config.timestamp = Logger::TimestampPrecision::None;
config.timestamp = Logger::TimestampPrecision::Milliseconds;
config.timestamp = Logger::TimestampPrecision::Microseconds;
```

Timestamps show elapsed time since logger initialization, not the current date or time. For example:

```text
[1.234s] [INFO] src/main.cpp:14 Application started
```

Microsecond formatting prints six fractional digits. Display precision does not guarantee the clock itself has matching accuracy.

### Source locations

```cpp
config.show_source = true;  // Include source filename and line number
config.show_source = false; // Omit source location
```

Source locations are captured automatically at each logging call.

### Console colors

```cpp
config.colors = true;  // ANSI colors by log level
config.colors = false; // Plain output
```

Some terminals or redirected output may not support ANSI colors, so disable them when needed.

## Output streams

- `ERROR` and `WARN` go to `std::cerr`.
- `NOTICE`, `INFO`, `DEBUG`, `VERBOSE`, and `TRACE` go to `std::cout`.

This lets errors and warnings be redirected separately from normal diagnostic output.

## CMake setup

Example `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyApp LANGUAGES CXX)

add_executable(my_app
    src/main.cpp
    src/logger.cpp
    src/player.cpp
)

target_include_directories(my_app PRIVATE include)
target_compile_features(my_app PRIVATE cxx_std_20)
```

Build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Make sure `src/logger.cpp` is compiled and linked exactly once.

## Important notes

- Initialize the logger before other threads start logging.
- No mutex is used. Concurrent writes can interleave, and changing configuration while other threads are logging is not thread-safe.
- The log level is atomic, so changing the level itself is safe; the other configuration fields are not atomic.
- Filtering happens before formatting, but ordinary C++ function arguments are evaluated before the logging macro runs. Avoid expensive expressions in log arguments if you need to skip their evaluation when a level is disabled.
- `Off` suppresses errors and warnings too.
- Elapsed timestamps use `std::chrono::steady_clock`, so they are not affected by system wall-clock adjustments.
