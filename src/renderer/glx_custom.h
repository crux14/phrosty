#pragma once

#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>

#define PH_GLX_CUSTOM_LOADER 1;

#define PH_GLX_VOID ;

#define PH_GLX_DECL_FUNCS(decl_kind)                                                               \
    namespace phrosty {                                                                            \
        decl_kind PFNGLXGETFBCONFIGSPROC glXGetFBConfigs;                                          \
        decl_kind PFNGLXGETVISUALFROMFBCONFIGPROC glXGetVisualFromFBConfig;                        \
        decl_kind PFNGLXGETFBCONFIGATTRIBPROC glXGetFBConfigAttrib;                                \
        decl_kind PFNGLXCREATEPIXMAPPROC glXCreatePixmap;                                          \
        decl_kind PFNGLXDESTROYPIXMAPPROC glXDestroyGLXPixmap;                                     \
                                                                                                   \
        decl_kind PFNGLXBINDTEXIMAGEEXTPROC glXBindTexImageEXT;                                    \
        decl_kind PFNGLXRELEASETEXIMAGEEXTPROC glXReleaseTexImageEXT;                              \
    }

PH_GLX_DECL_FUNCS(extern)

namespace phrosty {

    struct GLXGetProcAddress;

    bool load_glx_functions(const GLXGetProcAddress& get_proc_address);

}
