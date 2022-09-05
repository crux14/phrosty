#pragma once

#include "./item.h"

typedef struct _XDisplay Display;

namespace phrosty {

    struct X11TraceTexturePriv;

    class X11TraceTexture final {
      private:
        struct TextureHint {
            int width;
            int height;
        };

      public:
        explicit X11TraceTexture();
        virtual ~X11TraceTexture();

        bool map(GLint tex_loc);

        bool unmap();

        void poll_events();

        const TextureData& tex_data() const { return m_tex_data; }

      private:
        bool create_texture(const TextureHint& tex_hint);

        bool load_texture();

        GLenum get_texture_unit(int texture_index);

      private:
        TextureData m_tex_data;
        X11TraceTexturePriv* m_priv = nullptr;
    };

}
