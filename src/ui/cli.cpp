#include "./cli.h"

#include <core/logger.h>
#include <core/state.h>
#include <version.h>

#include <CLI/CLI.hpp>
#include <string>
#include <vector>

namespace phrosty {
    bool cli_parse(int* status, int argc, char** argv) {
        CLI::App app{"Phrosty: A global Picture-in-Picture (PiP) tool for Linux\n", "phrosty"};

        app.add_flag_callback(
            "--version", []() { throw CLI::CallForVersion(); }, "Show version");

        app.add_option_function<std::vector<unsigned int>>(
               "--init-win-resolution",
               [](const std::vector<unsigned int>& args) {
                   assert(args.size() == 2); // usually this passes
                   State::get().set_ui_state_synced([args](UIState& us) {
                       us.init_win_resolution.first = args[0];
                       us.init_win_resolution.second = args[1];
                   });
               },
               "Set initial window resolution")
            ->delimiter(',')
            ->check(CLI::PositiveNumber)
            ->expected(2);

        app.add_option_function<double>(
               "--opacity",
               [](const auto& arg) {
                   State::get().set_ui_state_synced([arg](UIState& us) { us.opacity = arg; });
               },
               "Set initial window opacity")
            ->check(CLI::Range(0.1, 1.0));

        // app.add_option_function<unsigned int>(
        //        "--topmost",
        //        [](const auto& arg) {
        //            State::get().set_ui_state_synced(
        //                [arg](UIState& us) { us.topmost = arg == 1 ? true : false; });
        //        },
        //        "Enable topmost window mode if 1 provided")
        //     ->check(CLI::Range(0, 1));

        app.add_option_function<unsigned int>(
               "--show-notification",
               [](const auto& arg) {
                   State::get().set_ui_state_synced(
                       [arg](UIState& us) { us.show_notification = arg == 1 ? true : false; });
               },
               "Enable notification if 1 provided")
            ->check(CLI::Range(0, 1));

        app.add_option_function<unsigned int>(
               "--use-default-border",
               [](const auto& arg) {
                   State::get().set_ui_state_synced(
                       [arg](UIState& us) { us.use_default_border = arg == 1 ? true : false; });
               },
               "Enable default window border")
            ->check(CLI::Range(0, 1));

        app.add_option_function<std::string>(
            "--border",
            [](const std::string& arg) {
                State::get().set_ui_state_synced([arg](UIState& us) { us.border = arg; });
            },
            "Set css for the window border");

        app.add_option_function<unsigned int>(
               "--fps",
               [](const auto& arg) {
                   State::get().set_render_state_synced([arg](RenderState& rs) { rs.fps = arg; });
               },
               "Set initial FPS")
            ->check(CLI::Range(1, 60));

        try {
            app.parse(argc, argv);
        } catch (const CLI::CallForVersion& v) {
            fprintf(stderr, "Phrosty %d.%d.%d %s\n", PHROSTY_VERSION_MAJOR, PHROSTY_VERSION_MINOR,
                    PHROSTY_VERSION_PATCH, PHROSTY_GIT_REV);
            *status = 0;
            return false;
        } catch (const CLI::ParseError& e) {
            *status = app.exit(e);
            return false;
        }

        *status = 0;
        return true;
    }
}
