#include "./include/vendor/xwininfo/xwininfo.h"

#include "./dsimple.h"

namespace xwininfo {

    xwininfo::X_WIN_ID select_window() {
        xcb_connection_t* xcb_dpy = nullptr;
        xcb_screen_t* xcb_screen = nullptr;
        ::Setup_Display_And_Screen("", &xcb_dpy, &xcb_screen);

        ::Intern_Atom(xcb_dpy, "_NET_VIRTUAL_ROOTS");
        ::Intern_Atom(xcb_dpy, "WM_STATE");

        return ::Select_Window(xcb_dpy, xcb_screen, !0);
    }

}
