#include "./monitor.h"

#include <core/state.h>
#include <core/logger.h>
#include <core/timer.h>
#include <renderer/renderer.h>

#include <QOpenGLContext>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QTimer>

#include <GL/glx.h>

#define PH_PERF_ON 0

namespace phrosty {

    static void* get_proc_address(const char* name) {
        QOpenGLContext* glctx = QOpenGLContext::currentContext();
        if (!glctx) {
            return nullptr;
        }
        return reinterpret_cast<void*>(glctx->getProcAddress(QByteArray(name)));
    }
    static void* glx_get_proc_address(const unsigned char* name) {
        // [TODO] add checks for validity of glx?
        return reinterpret_cast<void*>(glXGetProcAddress(name));
    }

    MonitorWidget::MonitorWidget(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent, f) {
        this->setObjectName("monitor_widget");
        m_renderer = new Renderer;

        auto render_state = State::get().render_state_synced();

        m_update_timer = new QTimer;
        m_update_timer->setInterval(1000.0 / render_state.fps);
        QObject::connect(m_update_timer, &QTimer::timeout, [this]() {
#ifdef PH_PERF_ON
            auto cur_time = phrosty::current_time();
            auto diff_time = phrosty::timediff_msec(cur_time, m_last_update_time);
            m_last_update_time = cur_time;
            PHLOG_INFO("diff: {}", diff_time);
#endif

            this->update();
        });
    }

    MonitorWidget::~MonitorWidget(void) {
        delete m_renderer;
        // PHLOG_WARNN("deleted");
    }

    void MonitorWidget::initializeGL() {
        RenderParams params;
        read_render_params(&params);
        m_renderer->initialize(params, {get_proc_address}, {glx_get_proc_address});
    }

    void MonitorWidget::paintGL() {
        RenderParams params;
        read_render_params(&params);
        m_renderer->render(params);

        if (!m_update_timer->isActive()) {
#ifdef PH_PERF_ON
            m_last_update_time = phrosty::current_time();
#endif
            m_update_timer->start();
        }
    }

    void MonitorWidget::resizeGL(int w, int h) {}

    void MonitorWidget::change_fps(unsigned int new_value) {
        m_update_timer->setInterval(1000.0 / new_value);
    }

    void MonitorWidget::read_render_params(RenderParams* params) {
        params->screen_width = this->width();
        params->screen_height = this->height();
        params->default_fb = this->defaultFramebufferObject();
    }

}
