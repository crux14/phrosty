#pragma once

#include <QOpenGLWidget>
#include <core/timer.h>

class QTimer;

namespace phrosty {

    class Renderer;
    struct RenderParams;

    class MonitorWidget final : public QOpenGLWidget {
        Q_OBJECT

      public:
        explicit MonitorWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~MonitorWidget(void);

      protected:
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int w, int h) override;

      public Q_SLOTS:
        void change_fps(unsigned int new_value);

      private:
        void read_render_params(RenderParams* params);

      private:
        Renderer* m_renderer = nullptr;
        QTimer* m_update_timer = nullptr;
        ClockTime m_last_update_time;
    };

}
