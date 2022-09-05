#pragma once

#include <spdlog/spdlog.h>
#include <string>

#define PH_FORMAT_MSG(msg) ("]({}:{}) " msg)

#define PHLOG_LOG(lvl, msg, ...)                                                                   \
    do {                                                                                           \
        static_assert(lvl != phrosty::LogLevel::OFF, "LogLevel::OFF is not supported");            \
        SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(),                                           \
                           static_cast<spdlog::level::level_enum>(lvl), PH_FORMAT_MSG(msg),        \
                           phrosty::detail::to_module_path(__FILE__), __LINE__, __VA_ARGS__);      \
    } while (0)

#define PHLOG_LOGN(lvl, msg)                                                                       \
    do {                                                                                           \
        static_assert(lvl != phrosty::LogLevel::OFF, "LogLevel::OFF is not supported");            \
        SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(),                                           \
                           static_cast<spdlog::level::level_enum>(lvl), PH_FORMAT_MSG(msg),        \
                           phrosty::detail::to_module_path(__FILE__), __LINE__);                   \
    } while (0)

#define PHLOG_DEBUG(msg, ...) PHLOG_LOG(phrosty::LogLevel::DEBUG, msg, __VA_ARGS__)
#define PHLOG_DEBUGN(msg) PHLOG_LOGN(phrosty::LogLevel::DEBUG, msg)
#define PHLOG_INFO(msg, ...) PHLOG_LOG(phrosty::LogLevel::INFO, msg, __VA_ARGS__)
#define PHLOG_INFON(msg) PHLOG_LOGN(phrosty::LogLevel::INFO, msg)
#define PHLOG_WARN(msg, ...) PHLOG_LOG(phrosty::LogLevel::WARN, msg, __VA_ARGS__)
#define PHLOG_WARNN(msg) PHLOG_LOGN(phrosty::LogLevel::WARN, msg)
#define PHLOG_ERROR(msg, ...) PHLOG_LOG(phrosty::LogLevel::ERROR, msg, __VA_ARGS__)
#define PHLOG_ERRORN(msg) PHLOG_LOGN(phrosty::LogLevel::ERROR, msg)

namespace phrosty {

    enum class LogLevel { DEBUG = 1, INFO, WARN, ERROR, OFF };

    struct LogCapabilities {
        LogLevel console_log_level = LogLevel::WARN;
    };

    void create_logger(const LogCapabilities& cap);

    void destroy_logger();

    namespace detail {

        constexpr inline int64_t contains_char(const char* str, const char* tar, int64_t pos = 0,
                                               bool seq = false) {
            if (*tar == '\0') {
                return pos;
            }
            while (*str) {
                if (*str == *tar) {
                    int64_t res = contains_char(str + 1, tar + 1, pos, true);
                    if (res > 0) {
                        return res;
                    }
                }
                if (seq) {
                    return -1;
                }
                str++;
                pos++;
            }
            return -1;
        }

        constexpr inline const char* substr_char(const char* str, const size_t pos) {
            size_t cnt = 0;
            while (*str && cnt < pos) {
                str++;
                cnt++;
            }
            return str;
        }

        constexpr inline const char* to_module_path(const char* str) {
            int64_t n = contains_char(str, "src/");
            if (n > 0) {
                return substr_char(str, n);
            } else {
                return str;
            }
        }

    }

}
