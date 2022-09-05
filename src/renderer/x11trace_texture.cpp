#include "./x11trace_texture.h"

#include <core/state.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xatom.h>
#include <cassert>

#include "./glx_common.h"

#include <string>

namespace phrosty {

    static bool create_pixmap(Pixmap& ret_pix_map, GLXPixmap& ret_glx_pixmap,
                              XWindowAttributes& ret_attrib, Display* disp, XID xid) {
        Status s = XGetWindowAttributes(disp, xid, &ret_attrib);
        if (s == 0) {
            printf("Fail to get window attributes!\n");
            return false;
        }

        // XRenderPictFormat* format = XRenderFindVisualFormat(display, attrib.visual);
        // int width = attrib.width;
        // int height = attrib.height;
        // int depth = attrib.depth;

        // printf("%d, %d, %d\n", width, height, depth);

        auto visualid = XVisualIDFromVisual(ret_attrib.visual);

        int screen = DefaultScreen(disp);

        int nfbconfigs = 0;
        int config = 0;
        auto fbconfigs = glXGetFBConfigs(disp, screen, &nfbconfigs);
        for (config = 0; config < nfbconfigs; config++) {
            auto visinfo = glXGetVisualFromFBConfig(disp, fbconfigs[config]);
            if (!visinfo || visinfo->visualid != visualid) {
                continue;
            }

            int value = -1;
            glXGetFBConfigAttrib(disp, fbconfigs[config], GLX_DRAWABLE_TYPE, &value);
            if (!(value & GLX_PIXMAP_BIT)) {
                continue;
            }

            glXGetFBConfigAttrib(disp, fbconfigs[config], GLX_BIND_TO_TEXTURE_TARGETS_EXT, &value);
            if (!(value & GLX_TEXTURE_2D_BIT_EXT)) {
                continue;
            }

            glXGetFBConfigAttrib(disp, fbconfigs[config], GLX_BIND_TO_TEXTURE_RGBA_EXT, &value);
            if (value == GL_FALSE) {
                glXGetFBConfigAttrib(disp, fbconfigs[config], GLX_BIND_TO_TEXTURE_RGB_EXT, &value);
                if (value == GL_FALSE) {
                    continue;
                }
            }

            // glXGetFBConfigAttrib(display, fbconfigs[i], GLX_Y_INVERTED_EXT, &value);
            // if (value == GL_TRUE) {
            //     top = 0.0f;
            //     bottom = 1.0f;
            // } else {
            //     top = 1.0f;
            //     bottom = 0.0f;
            // }

            break;
        }

        if (config == nfbconfigs) {
            printf("Could not find suitable fbconfig");
            return false;
        }

        Pixmap pixmap = XCompositeNameWindowPixmap(disp, xid);
        // clang-format off
        const int pixmap_attribs[] = {
            GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT, 
            GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGBA_EXT, 
            None
        };
        // clang-format on
        auto glx_pixmap = glXCreatePixmap(disp, fbconfigs[config], pixmap, pixmap_attribs);

        ret_pix_map = pixmap;
        ret_glx_pixmap = glx_pixmap;

        return true;
    }

    struct X11TraceTexturePriv {
        Display* display = nullptr;
        XID xid = 0;
        Pixmap x_pixmap = 0;
        GLXPixmap glx_pixmap = 0;
    };

    X11TraceTexture::X11TraceTexture() {
        Display* display = XOpenDisplay(NULL);

        XID xid = State::get().trace_state_synced().window_id;

        // Check if Composite extension is enabled
        int event_base_return;
        int error_base_return;
        if (XCompositeQueryExtension(display, &event_base_return, &error_base_return)) {
            // printf("COMPOSITE IS ENABLED!\n");
        }

        XCompositeRedirectWindow(display, xid, CompositeRedirectAutomatic);

        Pixmap pixmap;
        GLXPixmap glx_pixmap;
        XWindowAttributes attrib;
        if (!create_pixmap(pixmap, glx_pixmap, attrib, display, xid)) {
            // [TODO] throw exception?
            return;
        }

        XSelectInput(display, xid, StructureNotifyMask);

        m_priv = new X11TraceTexturePriv;
        m_priv->xid = xid;
        m_priv->display = display;
        m_priv->x_pixmap = pixmap;
        m_priv->glx_pixmap = glx_pixmap;

        this->create_texture({attrib.width, attrib.height});
    }

    X11TraceTexture::~X11TraceTexture() {
        glDeleteTextures(1, &m_tex_data.buffer);

        if (m_priv) {
            glXDestroyGLXPixmap(m_priv->display, m_priv->glx_pixmap);
            XFreePixmap(m_priv->display, m_priv->x_pixmap);
            XCompositeUnredirectWindow(m_priv->display, m_priv->xid, CompositeRedirectAutomatic);
        }

        delete m_priv;
    }

    bool X11TraceTexture::map(GLint tex_loc) {
        glActiveTexture(this->get_texture_unit(m_tex_data.index));
        glBindTexture(GL_TEXTURE_2D, m_tex_data.buffer);
        glXBindTexImageEXT(m_priv->display, m_priv->glx_pixmap, GLX_FRONT_LEFT_EXT, nullptr);
        glUniform1i(tex_loc, m_tex_data.index);
        return true;
    }

    bool X11TraceTexture::unmap() {
        glXReleaseTexImageEXT(m_priv->display, m_priv->glx_pixmap, GLX_FRONT_LEFT_EXT);
        return true;
    }

    void X11TraceTexture::poll_events() {
        while (QLength(m_priv->display) > 0) {
            XEvent ev;
            XNextEvent(m_priv->display, &ev);
            if (ev.type == ConfigureNotify) {
                // printf("ConfigureNotify: width: %d, height: %d\n", ev.xconfigure.width,
                //        ev.xconfigure.height);

                if (m_priv) {
                    glXDestroyGLXPixmap(m_priv->display, m_priv->glx_pixmap);
                    XFreePixmap(m_priv->display, m_priv->x_pixmap);
                }

                Pixmap pixmap;
                GLXPixmap glx_pixmap;
                XWindowAttributes attrib;
                if (!create_pixmap(pixmap, glx_pixmap, attrib, m_priv->display, m_priv->xid)) {
                    break;
                }

                m_priv->x_pixmap = pixmap;
                m_priv->glx_pixmap = glx_pixmap;

                glDeleteTextures(1, &m_tex_data.buffer);
                this->create_texture({attrib.width, attrib.height});
            }
        }

        XFlush(m_priv->display);
    }

    bool X11TraceTexture::create_texture(const TextureHint& tex_hint) {
        glGenTextures(1, &m_tex_data.buffer);

        m_tex_data.width = tex_hint.width;
        m_tex_data.height = tex_hint.height;
        m_tex_data.effective_width = tex_hint.width;
        m_tex_data.effective_height = tex_hint.height;

        glBindTexture(GL_TEXTURE_2D, m_tex_data.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    bool X11TraceTexture::load_texture() { return true; }

    GLenum X11TraceTexture::get_texture_unit(int texture_index) {
        // [TODO] there might be portablity issues
        return GL_TEXTURE0 + texture_index;
    };

}
