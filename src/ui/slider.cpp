#include "./slider.h"

#include <core/logger.h>

#include <QStyleOptionSlider>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QLabel>

namespace phrosty {

    void ConfigSlider::mouseMoveEvent(QMouseEvent* event) {
        QSlider::mouseMoveEvent(event);
        event->accept();
    }

    void ConfigSlider::mousePressEvent(QMouseEvent* event) {
        // By default, QSlider will move its step when it is clicked
        // This behavior is unwanted, so workaround below is needed
        auto before_page_step = this->pageStep();
        auto before_single_step = this->singleStep();

        this->setPageStep(0);
        this->setSingleStep(0);
        QSlider::mousePressEvent(event);
        this->setPageStep(before_page_step);
        this->setSingleStep(before_single_step);

        // event->accept();
    }

    void ConfigSlider::wheelEvent(QWheelEvent* event) {
        // disable wheel event
        event->accept();
    }

    SliderWidget::SliderWidget(QWidget* parent) : QWidget(parent), m_unit(1) {
        this->setObjectName("slider_widget");

        m_slider = new ConfigSlider;
        m_slider->setObjectName("config_slider");
        m_slider->setOrientation(Qt::Horizontal);
        m_slider->setRange(0, 100);
        m_slider->setValue(0);
        m_slider->setSingleStep(1);

        m_slider->setStyleSheet(u8R"(
            QSlider::sub-page:horizontal {
                border: 1px solid #00dbac;
                background: #00dbac;
                height: 2px;
                border-radius: 2px;
            }
            QSlider::add-page:horizontal {
                background: #eeeeee;
                border: 0px solid #eeeeee;
                height: 2px;
                border-radius: 2px;
            }
            QSlider::groove:horizontal {
                border: 1px solid #eeeeee;
                background: #eeeeee;
                height: 2px;
                border-radius: 1px;
                padding: 0px -1px;
            } 
            QSlider::handle:horizontal {
                background: #00dbac;
                width: 12px;
                margin-top: -6px;
                margin-bottom: -6px; 
                border-radius: 6px;
            }
        )");

        m_legend = new QLabel(this);
        m_legend->setStyleSheet("color: white; font-size: 14px;");

        m_main_layout = new QHBoxLayout;
        m_main_layout->setContentsMargins(0, 0, 0, 0);
        m_main_layout->setSpacing(6);
        m_main_layout->addWidget(m_legend, 0);
        m_main_layout->addWidget(m_slider, 0);

        this->setLayout(m_main_layout);

        QObject::connect(m_slider, &ConfigSlider::sliderMoved, [this](int pos) {
            Q_EMIT this->slider_moved(pos, m_unit, m_legend_value);
        });

        // QObject::connect(m_slider, &ConfigSlider::sliderPressed, [this]() {
        //     Q_EMIT this->slider_moved(m_slider->value(), m_unit, m_legend_value);
        // });

        // QObject::connect(m_slider, &ConfigSlider::sliderReleased, [=]() {
        //     if (m_state->m_atomic_state.last_play_state == state::PlayState::PLAYING) {
        //         Q_EMIT m_slider_released(akashi::state::PlayState::PLAYING);
        //     }
        // });
    }

    void SliderWidget::mouseReleaseEvent(QMouseEvent* event) { event->accept(); }

    void SliderWidget::set_legend_value(const std::string& value) {
        m_legend_value = value;
        m_legend->setText(value.c_str());
    }

    const std::string& SliderWidget::legend_value() const { return m_legend_value; }

    void SliderWidget::init_slider(int value, const std::array<int, 2>& vranges, double unit) {
        blockSignals(true);
        m_unit = unit;
        m_slider->setRange(vranges[0], vranges[1]);
        m_slider->setValue(value);
        blockSignals(false);
    }

}
