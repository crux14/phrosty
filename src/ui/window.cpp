#include "./window.h"

#include "./monitor.h"
#include "./action_bar.h"
#include "./slider.h"
#include "./store.h"

#include "./xutils.h"

#include <core/state.h>
#include <core/logger.h>

#include <QGridLayout>
#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScreen>

#include <QSizeGrip>

#include <QTimer>
#include <QApplication>

namespace phrosty {

    static QString DEFAULT_BORDER_CSS = u8R"(
            border-width: 2px;
            border-style: solid;
            border-color:
                qlineargradient(x1:0, y1:0, x2:1, y2:0, stop: 0 #00DBAC, stop: 1 #F850A3)
                #F850A3
                qlineargradient(x1:0, y1:0, x2:1, y2:0, stop: 0 #00DBAC, stop: 1 #F850A3)
                #00DBAC;
        )";

    Window::Window(QWidget* parent) : QFrame(parent) {
        this->setObjectName("window");

        auto ui_state = State::get().ui_state_synced();
        auto render_state = State::get().render_state_synced();

        QString border_css =
            (ui_state.use_default_border    ? DEFAULT_BORDER_CSS
             : ui_state.border.length() > 0 ? QString::fromStdString(ui_state.border)
                                            : "border: none");

        this->setStyleSheet("#window {" + border_css + "}");
        this->setFrameShadow(QFrame::Raised);

        this->setWindowOpacity(ui_state.opacity);

        m_monitor = new MonitorWidget(this);

        m_size_grip = new QSizeGrip{this};
        m_size_grip->setObjectName("size_grip");
        m_size_grip->setStyleSheet("QSizeGrip { background: transparent; }");

        m_action_bar = new ActionBar{this};
        m_action_bar->change_fps(render_state.fps);
        m_action_bar->change_opacity(ui_state.opacity);

        m_main_layout = new QGridLayout;
        m_main_layout->setContentsMargins(0, 0, 0, 0);
        m_main_layout->setSpacing(0);

        m_main_layout->addWidget(m_action_bar, 0, 0, -1, -1, Qt::AlignTop);

        // [XXX] keep it align unset, or the expansion does not work properly
        m_main_layout->addWidget(m_monitor, 0, 0, -1, -1);

        m_main_layout->addWidget(m_size_grip, 0, 0, -1, -1, Qt::AlignBottom | Qt::AlignRight);

        this->setLayout(m_main_layout);

        m_store = new Store;

        QObject::connect(m_store, &Store::fps_changed, [this](unsigned int new_value) {
            m_monitor->change_fps(new_value);
            m_action_bar->change_fps(new_value);
        });

        QObject::connect(m_store, &Store::opacity_changed, [this](double new_value) {
            this->setWindowOpacity(new_value);
            m_action_bar->change_opacity(new_value);
        });

        QObject::connect(m_action_bar, &ActionBar::fps_changed,
                         [this](unsigned int new_value) { m_store->change_fps(new_value); });

        QObject::connect(m_action_bar, &ActionBar::opacity_changed,
                         [this](double new_value) { m_store->change_opacity(new_value); });
    }

    Window::~Window(){};

    void Window::showEvent(QShowEvent*) {}

    void Window::changeEvent(QEvent* event) {
        if (event->type() == QEvent::ActivationChange) {
            if (this->isActiveWindow()) {
                // [TODO] A workaround for disabling auto focus issues that occur
                // when switching between workspaces
                if (State::get().ui_state_synced().topmost) {
                    auto cur_wsidx = get_current_workspace_index();
                    if (m_last_wsidx_on_inactive >= 0 and m_last_wsidx_on_inactive != cur_wsidx) {
                        raise_window(get_current_workspace_topmost_window());
                        return;
                    }
                }

                m_action_bar->toggle_controls(true);
                Q_EMIT this->window_activated();

            } else {
                m_last_wsidx_on_inactive = get_current_workspace_index();
                m_action_bar->toggle_controls(false);
            }
        }
    }

    void Window::mousePressEvent(QMouseEvent* event) { m_last_mouse = event->globalPos(); }

    void Window::mouseReleaseEvent(QMouseEvent* /*event*/) {
        // ensure the parent window to be raised
        Q_EMIT this->window_activated();
    }

    void Window::mouseMoveEvent(QMouseEvent* event) {
        this->move(this->pos() + (event->globalPos() - m_last_mouse));
        m_last_mouse = event->globalPos();
    }

}
