#pragma once

#include "./gl_common.h"

namespace phrosty {

    using XWinID = unsigned long;

    struct TextureData {
        int index = 0;
        GLuint buffer;
        void* image = nullptr;
        void* surface = nullptr;
        int width;  // includes stride
        int height; // includes stdide
        int effective_width;
        int effective_height;
        GLenum format = GL_RGBA;
        GLenum internal_format = GL_RGBA;
    };

}
