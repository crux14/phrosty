#include "./logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>
#include <memory>

namespace phrosty {

    constexpr const char PH_LOGGER_NAME[] = "phrosty_logger";

    void create_logger(const LogCapabilities& cap) {
        std::vector<spdlog::sink_ptr> sinks;

        auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
        console_sink->set_level(static_cast<spdlog::level::level_enum>(cap.console_log_level));
        console_sink->set_color(spdlog::level::debug, console_sink->white);
        console_sink->set_color(spdlog::level::info, console_sink->green);
        console_sink->set_color(spdlog::level::warn, console_sink->yellow);
        console_sink->set_color(spdlog::level::err, console_sink->red);
        console_sink->set_pattern("%^[%t%v%$");
        sinks.push_back(console_sink);

        auto logger = std::make_shared<spdlog::logger>(PH_LOGGER_NAME, sinks.begin(), sinks.end());

        // set the minimum log level that will trigger automatic flush
        logger->flush_on(spdlog::level::debug);
        logger->set_level(static_cast<spdlog::level::level_enum>(cap.console_log_level));

        // https://github.com/gabime/spdlog/wiki/2.-Creating-loggers#accessing-loggers-using-spdlogget
        spdlog::register_logger(logger);
        spdlog::set_default_logger(spdlog::get(PH_LOGGER_NAME));
    }

    void destroy_logger() {
        // [FIXME] not sure how to release loggers safely
        // spdlog::drop_all();
        // spdlog::drop(PH_LOGGER_NAME);
        // spdlog::shutdown();
    }

}
