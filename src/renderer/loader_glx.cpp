#include "./glx_custom.h"

#include <core/logger.h>

#include "./renderer.h"

#ifdef PH_GLX_CUSTOM_LOADER

#define DEF_FN_CHECK(proc_addr, func_name)                                                         \
    do {                                                                                           \
        auto func = proc_addr.func((const unsigned char*)#func_name);                              \
        if (!func) {                                                                               \
            PHLOG_ERROR("Could not load GLX function: {}", func_name);                             \
            return false;                                                                          \
        }                                                                                          \
        phrosty::func_name = reinterpret_cast<decltype(phrosty::func_name)>(func);                 \
    } while (0)

PH_GLX_DECL_FUNCS(PH_GLX_VOID)

namespace phrosty {
    bool load_glx_functions(const GLXGetProcAddress& get_proc_address) {
        DEF_FN_CHECK(get_proc_address, glXGetFBConfigs);
        DEF_FN_CHECK(get_proc_address, glXGetVisualFromFBConfig);
        DEF_FN_CHECK(get_proc_address, glXGetFBConfigAttrib);
        DEF_FN_CHECK(get_proc_address, glXCreatePixmap);
        DEF_FN_CHECK(get_proc_address, glXDestroyGLXPixmap);

        // [TODO] we should explicitly check extensions
        DEF_FN_CHECK(get_proc_address, glXBindTexImageEXT);
        DEF_FN_CHECK(get_proc_address, glXReleaseTexImageEXT);
        return true;
    }

}

#endif
