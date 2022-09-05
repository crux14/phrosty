#include "./renderer.h"

#include "./gl_common.h"
#include "./glx_common.h"
#include "./x11trace_pass.h"
#include "./x11trace_texture.h"

#include <core/logger.h>
#include <core/state.h>

namespace phrosty {

    struct RendererContext {
        X11TracePass* trace_pass = nullptr;
        X11TraceTexture* trace_tex = nullptr;
    };

    Renderer::Renderer() { m_ctx = new RendererContext; }

    Renderer::~Renderer() {
        if (m_ctx) {
            delete m_ctx->trace_pass;
            delete m_ctx->trace_tex;
        }
        delete m_ctx;
    }

    bool Renderer::initialize(const RenderParams& /* params */,
                              const GetProcAddress& get_proc_address,
                              const GLXGetProcAddress& glx_get_proc_address) {
        if (m_initialized) {
            PHLOG_ERRORN("Already initialized");
            return false;
        }

        if (!gladLoadGLLoader((GLADloadproc)get_proc_address.func)) {
            PHLOG_ERRORN("Failed to initialize OpenGL context");
            return false;
        }

        if (!load_glx_functions(glx_get_proc_address)) {
            PHLOG_ERRORN("Failed to initialize GLX functions");
            return false;
        }

        m_ctx->trace_tex = new X11TraceTexture;
        m_ctx->trace_pass = new X11TracePass;

        m_initialized = true;

        return true;
    }

    bool Renderer::render(const RenderParams& params) {
        if (!m_initialized) {
            PHLOG_ERRORN("Not yet initialized");
            return false;
        }

        m_ctx->trace_tex->poll_events();

        // if (glfwGetTime() - lasttime < (1.0 / phrosty_conf.fps)) {
        //     std::this_thread::sleep_for(std::chrono::milliseconds(2));
        //     return false;
        // }
        {
            m_ctx->trace_pass->begin();

            // glBindFramebuffer(GL_FRAMEBUFFER, params.default_fb);
            glViewport(0, 0, params.screen_width, params.screen_height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            m_ctx->trace_tex->map(m_ctx->trace_pass->prop().tex_loc);
            m_ctx->trace_pass->render(m_ctx->trace_tex->tex_data());
            m_ctx->trace_tex->unmap();

            m_ctx->trace_pass->end();
        }

        return true;
    }

    bool Renderer::resize(const RenderParams& params) { return true; }
}
