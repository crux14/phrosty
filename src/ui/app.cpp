#include "./window.h"
#include "./xutils.h"
#include "./font.h"
#include "./cli.h"

#include <core/state.h>
#include <core/logger.h>
#include <core/notify.h>
#include <vendor/xwininfo/xwininfo.h>

#include <QApplication>
#include <QTextCodec>
#include <thread>
#include <QSurfaceFormat>
#include <QScreen>
#include <QFontDatabase>

#include <QTimer>

#include <csignal>
#include <unistd.h>

int main(int argc, char** argv) {
    if (auto status = 0; !(phrosty::cli_parse(&status, argc, argv))) {
        return status;
    }

    phrosty::LogCapabilities cap;
    if (std::getenv("PH_LOGLEVEL")) {
        cap.console_log_level =
            static_cast<phrosty::LogLevel>(std::stoi(std::getenv("PH_LOGLEVEL"), nullptr));
    }
    phrosty::create_logger(cap);

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QApplication::setApplicationName("Phrosty");
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        // send SIGTERM to the main thread
        // kill(getpid(), SIGTERM);
    });

    phrosty::load_font_FontAwesome();

    QSurfaceFormat format;
    format.setVersion(4, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(1);
    QSurfaceFormat::setDefaultFormat(format);

    phrosty::Window window;

    // [XXX] Call this before xwininfo::select_window()
    auto parent_win = phrosty::get_current_active_window();
    auto cur_workspace_index = phrosty::get_current_workspace_index();

    auto ui_state = phrosty::State::get().ui_state_synced();
    xwininfo::X_WIN_ID trace_winid = 0;
    {
        phrosty::Notifier notifier("Phrosty", "Click on the window you want to display in PIP");
        if (ui_state.show_notification) {
            notifier.show();
        }
        trace_winid = xwininfo::select_window();
        if (ui_state.show_notification) {
            notifier.close();
        }
    }

    if (trace_winid == 0) {
        return 1;
    } else {
        phrosty::State::get().set_trace_state_synced(
            [trace_winid](phrosty::TraceState& ts) { ts.window_id = trace_winid; });
    }

    phrosty::switch_to_workspace(cur_workspace_index);

    // disable auto focus on startup
    // window.setAttribute(Qt::WA_ShowWithoutActivating);

    if (!ui_state.topmost) {
        window.setWindowFlags(window.windowFlags() | Qt::FramelessWindowHint);
    } else {
        window.setWindowFlags(window.windowFlags() | Qt::FramelessWindowHint |
                              Qt::WindowStaysOnTopHint);
        phrosty::set_topmost_across_workspaces(window.winId());
    }

    window.resize(ui_state.init_win_resolution.first, ui_state.init_win_resolution.second);
    auto screen_geom = QApplication::primaryScreen()->geometry();
    auto padding = screen_geom.height() * 0.02;
    window.move((screen_geom.width() - ui_state.init_win_resolution.first) - padding, padding);
    window.show();

    // [XXX] window.winId() must be called here.
    // If not (ex. called inside MonitorWidget), alpha blending will be broken.
    if (!phrosty::is_invalid_window(parent_win) and !ui_state.topmost) {
        // call it after window.show()
        phrosty::set_transient(parent_win, window.winId());

        // Focus passing
        QObject::connect(&window, &phrosty::Window::window_activated, [parent_win]() {
            QSharedPointer<QTimer> timer = QSharedPointer<QTimer>::create();
            QSharedPointer<int> rest_count = QSharedPointer<int>::create();
            *rest_count = 10;
            timer->setInterval(100);
            QObject::connect(timer.data(), &QTimer::timeout, [rest_count, timer, parent_win]() {
                if (phrosty::is_active_window(parent_win)) {
                    PHLOG_INFON("Parent window successfully raised!");
                    timer->stop();
                }
                if (*rest_count < 1) {
                    PHLOG_ERRORN("TIMEOUT");
                    // [TODO] need clear()?
                    timer->stop();
                } else {
                    phrosty::raise_window(parent_win);
                    (*rest_count)--;
                    PHLOG_INFO("...raising parent window: {}", *rest_count);
                }
            });
            phrosty::raise_window(parent_win);
            timer->start();
        });
    }

    return app.exec();
}
