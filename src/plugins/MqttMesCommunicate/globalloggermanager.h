#ifndef GLOBAL_LOGGER_H
#define GLOBAL_LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <vector>

class GlobalLoggerManager
{
private:
    inline static std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> loggers_;
    inline static std::mutex mutex_;
    inline static std::string base_path_ = "./logs";
    inline static bool initialized_ = false;
    inline static bool flush_thread_started_ = false;

    static void portable_localtime(std::time_t t, std::tm &out_tm)
    {
#if defined(_MSC_VER)
        localtime_s(&out_tm, &t);
#else
        localtime_r(&t, &out_tm);
#endif
    }

    GlobalLoggerManager() = default;

    static std::string get_current_hour_folder()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        portable_localtime(t, tm);

        char date_buf[9];
        char hour_buf[11];
        std::strftime(date_buf, sizeof(date_buf), "%Y%m%d", &tm);

        return std::string(date_buf);
    }

    static std::shared_ptr<spdlog::sinks::sink> create_file_sink(const std::string &logger_name)
    {
        try
        {
            std::string hour_folder = get_current_hour_folder();
            std::string full_dir = base_path_ + "/" + hour_folder;

            std::filesystem::create_directories(full_dir);

            auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
            portable_localtime(t, tm);

            std::string filename = logger_name + ".log";
            std::string filepath = full_dir + "/" + filename;

            return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                filepath, 1024 * 1024 * 10, 100);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("create sink error: " + std::string(e.what()));
        }
    }

public:
    static void initialize(const std::string &base_path = "./logs")
    {
        if (initialized_)
        {
            return;
        }

        base_path_ = base_path;
        std::filesystem::create_directories(base_path_);

        if (!flush_thread_started_)
        {
            spdlog::flush_every(std::chrono::seconds(1));
            flush_thread_started_ = true;
        }
        initialized_ = true;
    }

    static std::shared_ptr<spdlog::logger> create_logger(const std::string &logger_name,
                                                         spdlog::level::level_enum level = spdlog::level::info)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = loggers_.find(logger_name);
        if (it != loggers_.end())
        {
            return it->second;
        }

        try
        {
            auto file_sink = create_file_sink(logger_name);

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            auto logger = std::make_shared<spdlog::logger>(logger_name,
                                                           spdlog::sinks_init_list{file_sink, console_sink});
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
            logger->set_level(level);
            logger->flush_on(spdlog::level::warn);
            spdlog::register_logger(logger);
            loggers_[logger_name] = logger;
            return logger;
        }
        catch (const std::exception &e)
        {
            spdlog::error("create logger '{}' error: {}", logger_name, e.what());
            throw;
        }
    }

    static std::shared_ptr<spdlog::logger> get_logger(const std::string &logger_name)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = loggers_.find(logger_name);
        if (it == loggers_.end())
        {
            throw std::runtime_error("Logger '" + logger_name + "' not fond");
        }

        return it->second;
    }

    static bool logger_exists(const std::string &logger_name)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return loggers_.find(logger_name) != loggers_.end();
    }

    static void set_all_levels(spdlog::level::level_enum level)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto &pair : loggers_)
        {
            pair.second->set_level(level);
        }
    }

    static void shutdown()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto &pair : loggers_)
        {
            pair.second->flush();
        }

        loggers_.clear();
        spdlog::shutdown();
        initialized_ = false;
    }

    static std::vector<std::string> get_logger_names()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        std::vector<std::string> names;
        for (const auto &pair : loggers_)
        {
            names.push_back(pair.first);
        }
        return names;
    }
};

#define LOG_NAMED(name, level, ...) GlobalLoggerManager::get_logger(name)->level(__VA_ARGS__)

#define MQTT_LOG(...) LOG_NAMED("mqtt", info, __VA_ARGS__)
#define STATUS_LOG(...) LOG_NAMED("status", info, __VA_ARGS__)

#endif // GLOBAL_LOGGER_H