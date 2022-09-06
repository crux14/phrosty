#include "./x11trace_pass.h"

#include <core/logger.h>

#include "./item.h"

#include "./gl_common.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <cstdlib>
#include <cstdio>

namespace phrosty {

    static void create_buffer(GLuint& buffer, GLenum target, void* data, size_t data_size) {
        // Generally, error check is not necessary in this case
        // ref: https://stackoverflow.com/questions/20897179/behavior-of-glgenbuffers
        glGenBuffers(1, &buffer);

        glBindBuffer(target, buffer);
        glBufferData(target, data_size, data, GL_STATIC_DRAW);

        glBindBuffer(target, 0);
    }

    static bool compile_attach_shader(const GLuint prog, const GLenum type, const char* source) {
        bool res = true;
        GLuint shader = 0;
        GLint compile_status = 0;
        GLint log_length = 0;
        GLchar* log_str = nullptr;

        shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length > 1) {
            log_str = static_cast<GLchar*>(calloc(log_length + 1, sizeof(GLchar)));
            glGetShaderInfoLog(shader, log_length, nullptr, log_str);
            PHLOG_ERROR("Shader compile log: {}", log_str);
        }

        if (!compile_status) {
            PHLOG_ERROR("compile_attach_shader()<type={}> failed: Failed to compile shader: {}",
                        type, compile_status);
            res = false;
            goto exit;
        }

        glAttachShader(prog, shader);

    exit:
        if (log_str != nullptr) {
            free(log_str);
            log_str = nullptr;
        }
        glDeleteShader(shader);
        return res;
    }

    static bool link_shader(const GLuint prog) {
        GLint link_status = 0;
        GLint log_length = 0;

        glLinkProgram(prog);

        glGetProgramiv(prog, GL_LINK_STATUS, &link_status);
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_length);

        if (!link_status) {
            GLchar* log_str = static_cast<GLchar*>(calloc(log_length + 1, sizeof(GLchar)));
            glGetProgramInfoLog(prog, log_length, nullptr, log_str);
            PHLOG_ERROR("Failed to link shader: {}, {}", link_status, log_str);
            free(log_str);
            return false;
        } else {
            return true;
        }
    }

    static bool update_scale(glm::mat4& new_mvp, const TextureData& tex, const double scale_ratio) {
        // adjustment of aspect ratio
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        int screen_width = viewport[2];
        int screen_height = viewport[3];
        double aspect = (tex.effective_width * 1.0) / tex.effective_height;
        double scale_w = 1.0;
        double scale_h = 1.0;

        if (tex.effective_width < screen_width && tex.height < screen_height) {
            scale_w = (double)tex.effective_width / screen_width;
            scale_h = (double)tex.effective_height / screen_height;
        } else if (screen_width > screen_height) {
            // fixed height
            scale_w = (screen_height * aspect) / screen_width;
            scale_h = 1.0;
            if (scale_w > 1.0) {
                scale_h /= scale_w;
                scale_w = 1.0;
            }
        } else {
            // fixed width
            scale_w = 1.0;
            scale_h = (screen_width / aspect) / screen_height;
            if (scale_h > 1.0) {
                scale_w /= scale_h;
                scale_h = 1.0;
            }
        }

        new_mvp = glm::scale(new_mvp, glm::vec3(scale_w, scale_h, 1.0) * (float)scale_ratio);

        return true;
    }

    static constexpr const char* vshader_src = u8R"(
    #version 420 core
    uniform mat4 mvpMatrix;
    uniform float flipY;
    in vec3 vertices;
    in vec2 uvs;

    out VS_OUT {
        vec2 vUvs;
    } vs_out;
    
    void main(void){
        vs_out.vUvs = uvs;
        gl_Position = mvpMatrix * vec4(vertices * vec3(1, flipY, 1), 1.0);
    }
)";

    static constexpr const char* fshader_src = u8R"(
    #version 420 core
    uniform sampler2D texture0;

    in VS_OUT {
        vec2 vUvs;
    } fs_in;

    out vec4 fragColor;

    void main(void){
        vec4 smpColor = texture(texture0, fs_in.vUvs);
        fragColor = smpColor; 
    }
)";

    X11TracePass::X11TracePass() {
        m_prop.prog = glCreateProgram();

        // loading shader
        this->load_shader();

        // uniform location
        m_prop.mvp_loc = glGetUniformLocation(m_prop.prog, "mvpMatrix");
        m_prop.flipY_loc = glGetUniformLocation(m_prop.prog, "flipY");
        m_prop.tex_loc = glGetUniformLocation(m_prop.prog, "texture0");
        m_prop.time_loc = glGetUniformLocation(m_prop.prog, "time");
        m_prop.resolution_loc = glGetUniformLocation(m_prop.prog, "resolution");

        this->load_vao();
        this->load_ibo();
        m_prop.ibo_length = 6;
    }

    X11TracePass::~X11TracePass() {
        glDeleteBuffers(1, &m_prop.ibo);
        glDeleteVertexArrays(1, &m_prop.vao);
        glDeleteProgram(m_prop.prog);
    }

    void X11TracePass::begin() { glUseProgram(m_prop.prog); }

    void X11TracePass::end() {
        // glUseProgram(0);
    }

    bool X11TracePass::render(const TextureData& tex_data) {
        glUseProgram(m_prop.prog);

        glm::mat4 new_mvp = glm::mat4(1.0f);
        update_scale(new_mvp, tex_data, 1.0);

        glUniformMatrix4fv(m_prop.mvp_loc, 1, GL_FALSE, &new_mvp[0][0]);

        glUniform1f(m_prop.flipY_loc, 1);
        glUniform1f(m_prop.time_loc, 0.0);
        // glUniform2f(m_prop.resolution_loc, resolution[0], resolution[1]);
        glBindVertexArray(m_prop.vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_prop.ibo);

        glDrawElements(GL_TRIANGLES, m_prop.ibo_length, GL_UNSIGNED_SHORT, 0);

        glBindVertexArray(0);

        return true;
    }

    bool X11TracePass::load_shader() {
        compile_attach_shader(m_prop.prog, GL_VERTEX_SHADER, vshader_src);
        compile_attach_shader(m_prop.prog, GL_FRAGMENT_SHADER, fshader_src);
        link_shader(m_prop.prog);
        return true;
    }

    bool X11TracePass::load_vao() {
        GLfloat quadWidth = 1.0f;
        GLfloat quadHeight = 1.0f;

        GLfloat vertices[] = {
            -quadWidth, quadHeight,  0.0, // left-top
            quadWidth,  quadHeight,  0.0, // right-top
            -quadWidth, -quadHeight, 0.0, // left-bottom
            quadWidth,  -quadHeight, 0.0  // right-bottom
        };
        GLuint vertices_vbo;
        create_buffer(vertices_vbo, GL_ARRAY_BUFFER, vertices, sizeof(vertices));

        GLfloat uvs[] = {
            0.0, 0.0, // left-top
            1.0, 0.0, // right-top
            0.0, 1.0, // left-bottom
            1.0, 1.0, // right-bottom
        };
        GLuint uvs_vbo;
        create_buffer(uvs_vbo, GL_ARRAY_BUFFER, uvs, sizeof(uvs));

        glGenVertexArrays(1, &m_prop.vao);
        glBindVertexArray(m_prop.vao);

        // vertices attribute
        glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
        // [TODO] error check?
        GLint verticesLoc = glGetAttribLocation(m_prop.prog, "vertices");
        glEnableVertexAttribArray(verticesLoc);
        glVertexAttribPointer(verticesLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        // uvs attribute
        glBindBuffer(GL_ARRAY_BUFFER, uvs_vbo);
        // [TODO] error check?
        GLint texCoordLoc = glGetAttribLocation(m_prop.prog, "uvs");
        glEnableVertexAttribArray(texCoordLoc);
        glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

    bool X11TracePass::load_ibo() {
        // [XXX] make sure to use the same type of the values to be passed to glDrawElements
        unsigned short indices[] = {
            0, 1, 2, // left
            1, 2, 3  // right
        };
        create_buffer(m_prop.ibo, GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices));
        return true;
    }

}
