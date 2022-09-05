#pragma once

#include <QSlider>
#include <array>

class QHBoxLayout;
class QKeyEvent;
class QLabel;

namespace phrosty {

    class ConfigSlider final : public QSlider {
        Q_OBJECT

      public:
        explicit ConfigSlider(Qt::Orientation, QWidget* parent = nullptr) { Q_UNUSED(parent); };
        explicit ConfigSlider(QWidget* parent = nullptr) { Q_UNUSED(parent); };
        virtual ~ConfigSlider() = default;

      protected:
        void mouseMoveEvent(QMouseEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent*) override;
    };

    class SliderWidget final : public QWidget {
        Q_OBJECT
      public:
        explicit SliderWidget(QWidget* parent = 0);
        virtual ~SliderWidget() = default;
        void set_legend_value(const std::string& value);
        const std::string& legend_value() const;
        void init_slider(int value, const std::array<int, 2>& vranges, double unit);

      protected:
        void mouseReleaseEvent(QMouseEvent* event) override;

      private:
        ConfigSlider* m_slider;
        QLabel* m_legend;
        QHBoxLayout* m_main_layout;

        double m_unit;
        std::string m_legend_value;
      Q_SIGNALS:
        void slider_moved(int value, double m_unit, const std::string& legend_value);
        // void slider_pressed(double);
        // void slider_released(double);
    };

}
