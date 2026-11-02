
#include "logger.hpp"

#include <format>
#include <iostream>

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::init(const Config& config) {
    config_ = config;
    start_ = Clock::now();
    level_.store(config.level, std::memory_order_relaxed);
}

void Logger::set_level(Level level) noexcept {
    level_.store(level, std::memory_order_relaxed);
}

bool Logger::should_log(Level level) const noexcept {
    if (level == Level::Off)
        return false;

    const auto threshold =
        level_.load(std::memory_order_relaxed);

    if (threshold == Level::Off)
        return false;
    return level <= threshold;
}

const char* Logger::level_name(Level level) noexcept {
    switch (level) {
        case Level::Off:     return "OFF";
        case Level::Error:   return "ERROR";
        case Level::Warn:    return "WARN";
        case Level::Notice:  return "NOTICE";
        case Level::Info:    return "INFO";
        case Level::Debug:   return "DEBUG";
        case Level::Verbose: return "VERBOSE";
        case Level::Trace:   return "TRACE";
    }

    return "?";
}

const char* Logger::level_color(Level level) noexcept {
    switch (level) {
        case Level::Error:   return "\033[31m"; // Red
        case Level::Warn:    return "\033[33m"; // Yellow
        case Level::Notice:  return "\033[36m"; // Cyan
        case Level::Info:    return "\033[32m"; // Green
        case Level::Debug:   return "\033[37m"; // White
        case Level::Verbose: return "\033[90m"; // Gray
        case Level::Trace:   return "\033[90m"; // Gray
        case Level::Off:     return "";
    }

    return "";
}

void Logger::write(
    Level level,
    std::string_view message,
    std::source_location source) {
	std::cout << "logging" << std::endl;

    auto& out = (level == Level::Error ||
                 level == Level::Warn)
                    ? std::cerr
                    : std::cout;

    const auto elapsed = Clock::now() - start_;

    if (config_.timestamp != TimestampPrecision::None) {
        if (config_.timestamp == TimestampPrecision::Milliseconds) {
            const auto ms =
                std::chrono::duration_cast<
                    std::chrono::milliseconds>(elapsed).count();

            out << '[' << ms / 1000 << '.'
                << std::format("{:03}", ms % 1000) << "s] ";
        } else {
            const auto us =
                std::chrono::duration_cast<
                    std::chrono::microseconds>(elapsed).count();

            out << '[' << us / 1'000'000 << '.'
                << std::format("{:06}", us % 1'000'000) << "s] ";
        }
    }

    if (config_.colors)
        out << level_color(level);

    out << '[' << level_name(level) << ']';

    if (config_.colors)
        out << "\033[0m";

    if (config_.show_source) {
        out << ' ' << source.file_name()
            << ':' << source.line();
    }

    out << ' ' << message << std::endl;
}
