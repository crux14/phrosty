#pragma once

#include "./gl_common.h"

namespace phrosty {

    struct TextureData;

    struct X11TracePassProp {
        GLuint prog;

        GLuint vao;

        GLuint ibo;

        int ibo_length;

        GLuint tex_loc;

        GLuint mvp_loc;

        GLuint flipY_loc;

        GLuint time_loc;

        GLuint resolution_loc;
    };

    class X11TracePass final {
      public:
        explicit X11TracePass(void);

        virtual ~X11TracePass(void);

        bool render(const TextureData& tex_data);

        void begin();

        void end();

        const X11TracePassProp& prop() const { return m_prop; }

      private:
        bool load_shader();

        bool load_vao();

        bool load_ibo();

      private:
        X11TracePassProp m_prop;
    };

}
