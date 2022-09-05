#pragma once

#include <QWidget>
#include <QLabel>

class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;
class QMouseEvent;

namespace phrosty {

    class SliderWidget;

    class ExitButton final : public QLabel {
        Q_OBJECT

      public:
        explicit ExitButton(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~ExitButton() = default;

      protected:
        virtual void mouseReleaseEvent(QMouseEvent* event) override;

      private:
        std::string m_base_css;
    };

    class ConfigValueLable final : public QLabel {
        Q_OBJECT

      public:
        explicit ConfigValueLable(const std::string& icon_str, const std::string& icon_value,
                                  QWidget* parent = Q_NULLPTR,
                                  Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~ConfigValueLable() = default;

        void set_value(const std::string& new_value);

        bool is_active() const;
        void set_active();
        void set_inactive();

      protected:
        virtual void mouseReleaseEvent(QMouseEvent* event) override;

      Q_SIGNALS:
        void btn_clicked();

      private:
        QVBoxLayout* m_main_layout;
        QLabel* m_icon_label;
        QLabel* m_value_label;
        std::string m_base_icon_css;
        std::string m_value;
        bool m_active = false;
    };

    class ActionBar final : public QWidget {
        Q_OBJECT
      public:
        explicit ActionBar(QWidget* parent = 0);

        void toggle_controls(bool show);

      protected:
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void enterEvent(QEvent* event) override;
        virtual void leaveEvent(QEvent* event) override;

      Q_SIGNALS:
        void fps_changed(unsigned int new_value);
        void opacity_changed(double new_value);

      public Q_SLOTS:
        void change_fps(unsigned int new_value);
        void change_opacity(double new_value);

      private:
        QGridLayout* m_main_layout;
        QHBoxLayout* m_btn_layout;
        SliderWidget* m_slider_widget;
        ExitButton* m_exit_btn;
        ConfigValueLable* m_fps_btn;
        ConfigValueLable* m_opacity_btn;

        QLabel* m_dummy;
        QLabel* m_cover;

        bool m_slider_active = false;
    };

}
