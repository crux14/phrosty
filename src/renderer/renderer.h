#pragma once

#include <cstdint>

namespace phrosty {

    struct GetProcAddress {
        void* (*func)(const char*) = nullptr;
    };

    struct GLXGetProcAddress {
        void* (*func)(const unsigned char*) = nullptr;
    };

    struct RenderParams {
        uint32_t default_fb;
        int screen_width;
        int screen_height;
    };

    struct RendererContext;

    class Renderer final {
      public:
        explicit Renderer();
        virtual ~Renderer();

        bool initialize(const RenderParams& params, const GetProcAddress& get_proc_address,
                        const GLXGetProcAddress& glx_get_proc_address);

        bool render(const RenderParams& params);

        bool resize(const RenderParams& params);

      private:
        RendererContext* m_ctx;
        bool m_initialized = false;
    };

}
