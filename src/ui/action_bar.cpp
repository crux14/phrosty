#include "./action_bar.h"
#include "./font.h"
#include "./slider.h"

#include <core/logger.h>
#include <core/state.h>

#include <QGridLayout>
#include <QApplication>
#include <QScreen>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>

namespace phrosty {

    ExitButton::ExitButton(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f) {
        this->setObjectName("exit_btn");

        QFont font;
        font.setPointSize(28);
        font.setFamily(phrosty::FontAwesomeFamily.c_str());
        this->setFont(font);

        this->setStyleSheet("color: white; font-size: 16px;");
        this->setText("\uf00d");
        this->setAlignment(Qt::AlignVCenter);
    }

    void ExitButton::mouseReleaseEvent(QMouseEvent* event) {
        QApplication::quit();
        event->accept();
    }

    ConfigValueLable::ConfigValueLable(const std::string& icon_str, const std::string& icon_value,
                                       QWidget* parent, Qt::WindowFlags f)
        : QLabel(parent, f), m_value(icon_value) {
        this->setObjectName("config_value_label");

        this->setMinimumWidth(20);

        QFont fa_font;
        fa_font.setPointSize(28);
        fa_font.setFamily(phrosty::FontAwesomeFamily.c_str());

        m_value_label = new QLabel(this);
        m_value_label->setText(m_value.c_str());
        m_value_label->setStyleSheet("color: white; font-size: 10px; text-align: center;");

        m_icon_label = new QLabel(this);
        m_icon_label->setText(icon_str.c_str());
        m_icon_label->setFont(fa_font);
        m_base_icon_css = "font-size: 16px;";
        m_icon_label->setStyleSheet((m_base_icon_css + "color: white;").c_str());

        m_main_layout = new QVBoxLayout(this);
        m_main_layout->setContentsMargins(0, 0, 0, 0);
        m_main_layout->setSpacing(0);

        m_main_layout->addWidget(m_value_label, 0, Qt::AlignCenter);
        m_main_layout->addWidget(m_icon_label, 1, Qt::AlignCenter);

        this->setLayout(m_main_layout);
    }

    void ConfigValueLable::set_value(const std::string& new_value) {
        m_value = new_value;
        m_value_label->setText(m_value.c_str());
    }

    bool ConfigValueLable::is_active() const { return m_active; }

    void ConfigValueLable::set_active() {
        m_icon_label->setStyleSheet((m_base_icon_css + "color: #00dbac;").c_str());
        m_active = true;
    }

    void ConfigValueLable::set_inactive() {
        m_icon_label->setStyleSheet((m_base_icon_css + "color: white;").c_str());
        m_active = false;
    }

    void ConfigValueLable::mouseReleaseEvent(QMouseEvent* event) {
        if (m_active) {
            this->set_inactive();
        } else {
            this->set_active();
        }
        Q_EMIT this->btn_clicked();
        event->accept();
    }

    ActionBar::ActionBar(QWidget* parent) : QWidget(parent) {
        this->setObjectName("action_bar");

        m_cover = new QLabel(this);
        m_cover->setStyleSheet("background: rgba(0,0,0,20%);");

        m_dummy = new QLabel(this);
        m_dummy->setText("?");
        m_dummy->setStyleSheet("color: transparent; font-size: 32px;");

        m_slider_widget = new SliderWidget(this);
        m_slider_widget->hide();

        m_fps_btn = new ConfigValueLable("\uf863", "-1", this);
        m_opacity_btn = new ConfigValueLable("\uf43c", "-1", this);

        m_exit_btn = new ExitButton(this);

        m_main_layout = new QGridLayout;
        m_main_layout->setContentsMargins(0, 0, 0, 0);
        m_main_layout->setSpacing(0);

        m_btn_layout = new QHBoxLayout;
        m_btn_layout->setContentsMargins(15, 0, 15, 0);
        m_btn_layout->setSpacing(10);
        m_btn_layout->addWidget(m_exit_btn, 0);
        m_btn_layout->addWidget(m_slider_widget, 0, Qt::AlignCenter);
        m_btn_layout->addStretch(5);
        m_btn_layout->addWidget(m_fps_btn, 0);
        m_btn_layout->addWidget(m_opacity_btn, 0);

        m_main_layout->addWidget(m_cover, 0, 0, -1, -1);
        m_main_layout->addWidget(m_dummy, 0, 0, -1, -1);
        m_main_layout->addLayout(m_btn_layout, 0, 0, -1, -1, Qt::AlignCenter);

        this->setLayout(m_main_layout);

        this->toggle_controls(false);

        QObject::connect(m_slider_widget, &SliderWidget::slider_moved,
                         [this](int value, double m_unit, const std::string& legend_value) {
                             if (legend_value == "FPS") {
                                 Q_EMIT this->fps_changed(value);
                             } else if (legend_value == "OPACITY") {
                                 Q_EMIT this->opacity_changed(value * m_unit);
                             }
                         });

        QObject::connect(m_fps_btn, &ConfigValueLable::btn_clicked, [this]() {
            auto render_state = State::get().render_state_synced();
            auto cur_fps = render_state.fps;
            int max_fps = QApplication::primaryScreen()->refreshRate();

            if (m_opacity_btn->is_active()) {
                m_opacity_btn->set_inactive();
                m_slider_widget->set_legend_value("FPS");
                m_slider_widget->init_slider(cur_fps, {1, max_fps}, 1);
            } else if (!m_slider_active) {
                m_slider_widget->set_legend_value("FPS");
                m_slider_widget->init_slider(cur_fps, {1, max_fps}, 1);
                m_slider_widget->show();
                m_slider_active = true;
            } else {
                m_slider_widget->hide();
                m_slider_active = false;
            }
        });

        QObject::connect(m_opacity_btn, &ConfigValueLable::btn_clicked, [this]() {
            auto ui_state = State::get().ui_state_synced();
            auto cur_opacity = int(ui_state.opacity * 100);

            if (m_fps_btn->is_active()) {
                m_fps_btn->set_inactive();
                m_slider_widget->set_legend_value("OPACITY");
                m_slider_widget->init_slider(cur_opacity, {10, 100}, 0.01);
            } else if (!m_slider_active) {
                m_slider_widget->set_legend_value("OPACITY");
                m_slider_widget->init_slider(cur_opacity, {10, 100}, 0.01);
                m_slider_widget->show();
                m_slider_active = true;
            } else {
                m_slider_widget->hide();
                m_slider_active = false;
            }
        });
    }

    void ActionBar::toggle_controls(bool show) {
        if (show) {
            m_exit_btn->show();
            m_fps_btn->show();
            m_opacity_btn->show();
            if (m_slider_active) {
                m_slider_widget->show();
            }
            m_cover->setStyleSheet("background: rgba(0,0,0,20%);");
        } else {
            m_exit_btn->hide();
            m_fps_btn->hide();
            m_opacity_btn->hide();
            m_slider_widget->hide();
            m_cover->setStyleSheet("background: transparent;");
        }
    }

    void ActionBar::mouseReleaseEvent(QMouseEvent* event) {
        m_fps_btn->set_inactive();
        m_opacity_btn->set_inactive();
        m_slider_widget->hide();
        m_slider_active = false;
        // event->accept();
    }

    void ActionBar::enterEvent(QEvent*) {}

    void ActionBar::leaveEvent(QEvent*) {}

    void ActionBar::change_fps(unsigned int new_value) {
        m_fps_btn->set_value(std::to_string(new_value));
    }

    void ActionBar::change_opacity(double new_value) {
        auto iv = int(new_value);
        auto i_str = std::to_string(iv);
        auto f_str = std::to_string(int(new_value * 100) - (iv * 100));
        m_opacity_btn->set_value(i_str + "." + f_str);
    }

}
