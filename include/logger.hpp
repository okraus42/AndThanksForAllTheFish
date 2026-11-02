
#pragma once

#include <atomic>
#include <chrono>
#include <source_location>
#include <string_view>

class Logger {
public:
    enum class Level {
        Off,
        Error,
        Warn,
        Notice,
        Info,
        Debug,
        Verbose,
        Trace
    };

    enum class TimestampPrecision {
        None,
        Milliseconds,
        Microseconds
    };

    struct Config {
        Level level = Level::Debug;
        TimestampPrecision timestamp =
            TimestampPrecision::Milliseconds;
        bool show_source = true;
        bool colors = true;
    };

    // Access the single global logger instance.
    static Logger& instance();

    // Call once, early in main(), before other threads start logging.
    void init(const Config& config);

    // Change verbosity at runtime.
    void set_level(Level level) noexcept;

    // Check whether a message would be logged.
    // Useful for avoiding unnecessary formatting.
    [[nodiscard]] bool should_log(Level level) const noexcept;

    // Write an already formatted message.
    void write(
        Level level,
        std::string_view message,
        std::source_location source =
            std::source_location::current());

private:
    using Clock = std::chrono::steady_clock;

    Logger() = default;

    static const char* level_name(Level level) noexcept;
    static const char* level_color(Level level) noexcept;


    Config config_{};
    std::atomic<Level> level_{Level::Debug};
    Clock::time_point start_{};
};

// Formatting happens only when the selected level is enabled.
// C++20 __VA_OPT__ supports calls with or without formatting arguments.

#define LOG_ERROR(fmt, ...) \
    do { \
        auto& logger_ = Logger::instance(); \
        if (logger_.should_log(Logger::Level::Error)) \
            logger_.write(Logger::Level::Error, \
                std::format(fmt __VA_OPT__(,) __VA_ARGS__), \
                std::source_location::current()); \
    } while (false)

#define LOG_WARN(fmt, ...) \
    do { \
        auto& logger_ = Logger::instance(); \
        if (logger_.should_log(Logger::Level::Warn)) \
            logger_.write(Logger::Level::Warn, \
                std::format(fmt __VA_OPT__(,) __VA_ARGS__), \
                std::source_location::current()); \
    } while (false)

#define LOG_NOTICE(fmt, ...) \
    do { \
        auto& logger_ = Logger::instance(); \
        if (logger_.should_log(Logger::Level::Notice)) \
            logger_.write(Logger::Level::Notice, \
                std::format(fmt __VA_OPT__(,) __VA_ARGS__), \
                std::source_location::current()); \
    } while (false)

#define LOG_INFO(fmt, ...) \
    do { \
        auto& logger_ = Logger::instance(); \
        if (logger_.should_log(Logger::Level::Info)) \
            logger_.write(Logger::Level::Info, \
                std::format(fmt __VA_OPT__(,) __VA_ARGS__), \
                std::source_location::current()); \
    } while (false)

#define LOG_DEBUG(fmt, ...) \
    do { \
        auto& logger_ = Logger::instance(); \
        if (logger_.should_log(Logger::Level::Debug)) \
            logger_.write(Logger::Level::Debug, \
                std::format(fmt __VA_OPT__(,) __VA_ARGS__), \
                std::source_location::current()); \
    } while (false)

#define LOG_VERBOSE(fmt, ...) \
    do { \
        auto& logger_ = Logger::instance(); \
        if (logger_.should_log(Logger::Level::Verbose)) \
            logger_.write(Logger::Level::Verbose, \
                std::format(fmt __VA_OPT__(,) __VA_ARGS__), \
                std::source_location::current()); \
    } while (false)

#define LOG_TRACE(fmt, ...) \
    do { \
        auto& logger_ = Logger::instance(); \
        if (logger_.should_log(Logger::Level::Trace)) \
            logger_.write(Logger::Level::Trace, \
                std::format(fmt __VA_OPT__(,) __VA_ARGS__), \
                std::source_location::current()); \
    } while (false)

// Include <format> before using these logging macros.
