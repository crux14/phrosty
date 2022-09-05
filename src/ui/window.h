#pragma once

#include <QFrame>

class QGridLayout;
class QMouseEvent;
class QResizeEvent;
class QSizeGrip;

namespace phrosty {

    class MonitorWidget;
    class ActionBar;
    class Store;

    class Window final : public QFrame {
        Q_OBJECT
      public:
        explicit Window(QWidget* parent = 0);
        virtual ~Window();

      protected:
        virtual void showEvent(QShowEvent* event) override;
        virtual void changeEvent(QEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;

      Q_SIGNALS:
        void window_activated(void);

      private:
        Store* m_store;
        MonitorWidget* m_monitor;
        QGridLayout* m_main_layout;
        ActionBar* m_action_bar;
        QSizeGrip* m_size_grip;
        QPoint m_last_mouse;

        int m_last_wsidx_on_inactive = -1;
    };

}
