#include "./xutils.h"

#include <core/logger.h>

#include <QWidget>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace phrosty {

    namespace priv {

        static ::Display* get_display() { return QX11Info::display(); }

        std::string get_x_error_text(::Display* disp, int err_code) {
            constexpr const int length = 256;
            char err_text[length] = {0};
            XGetErrorText(disp, err_code, err_text, length);
            return std::string(err_text);
        }

        struct WindowProRes {
            ::Atom act_reqtype;
            unsigned char* value_ret = nullptr;
            int act_fmt = -1;
            unsigned long nitems = 0;
            unsigned long left = 0;

            ~WindowProRes() {
                if (this->value_ret) {
                    XFree(this->value_ret);
                }
            }
        };

        static bool get_window_property(WindowProRes* prop_res, XWindowID win, ::Atom atom,
                                        long length, ::Atom req_type) {
            auto disp = get_display();
            auto status =
                ::XGetWindowProperty(disp, win, atom, 0, length, false, req_type,
                                     &prop_res->act_reqtype, &prop_res->act_fmt, &prop_res->nitems,
                                     &prop_res->left, (unsigned char**)&prop_res->value_ret);

            if (status != Success) {
                PHLOG_ERROR("{}", priv::get_x_error_text(disp, status).c_str());
                return false;
            }

            if (prop_res->nitems == 0 || !prop_res->value_ret) {
                PHLOG_ERRORN("No items found");
                return false;
            }

            return true;
        }

    }

    XWindowID get_current_workspace_topmost_window() {
        auto disp = QX11Info::display();
        auto atom = ::XInternAtom(disp, "_NET_CLIENT_LIST_STACKING", false);
        priv::WindowProRes prop_res;
        if (!(priv::get_window_property(&prop_res, QX11Info::appRootWindow(), atom, (~0L),
                                        XA_WINDOW))) {
            return InvalidXWindowID;
        }

        auto cur_wsidx = get_current_workspace_index();
        if (cur_wsidx < 0) {
            PHLOG_ERRORN("Failed to get current workspace index");
            return InvalidXWindowID;
        }

        for (int i = prop_res.nitems - 1; i >= 0; i--) {
            auto wid = ((XID*)prop_res.value_ret)[i];
            auto wsidx = get_workspace_index(wid);
            if (cur_wsidx == wsidx) {
                return wid;
            }
        }

        return InvalidXWindowID;
    }

    XWindowID get_current_active_window() {
        auto disp = QX11Info::display();
        auto atom = ::XInternAtom(disp, "_NET_ACTIVE_WINDOW", false);
        priv::WindowProRes prop_res;
        if (!(priv::get_window_property(&prop_res, QX11Info::appRootWindow(), atom, 4,
                                        XA_WINDOW))) {
            return InvalidXWindowID;
        }
        if (prop_res.act_reqtype != XA_WINDOW || prop_res.nitems != 1) {
            PHLOG_ERRORN("Invalid result");
            return InvalidXWindowID;
        }
        XID tar_winid = *prop_res.value_ret;
        return tar_winid;
    }

    bool is_invalid_window(XWindowID win) { return win == InvalidXWindowID; }

    bool is_active_window(XWindowID win) { return get_current_active_window() == win; }

    void set_transient(XWindowID parent_win, XWindowID child_winid) {
        ::XSetTransientForHint(priv::get_display(), child_winid, parent_win);
    }

    void raise_window(XWindowID win) {
        auto disp = priv::get_display();
        XEvent x_evt;
        x_evt.xclient.type = ClientMessage;
        x_evt.xclient.window = win;
        x_evt.xclient.message_type = ::XInternAtom(disp, "_NET_ACTIVE_WINDOW", False);
        x_evt.xclient.format = 32;
        x_evt.xclient.data.l[0] = 1;
        // Specify timestamp
        x_evt.xclient.data.l[1] = CurrentTime;
        x_evt.xclient.data.l[2] = 0;
        x_evt.xclient.data.l[3] = 0;
        x_evt.xclient.data.l[4] = 0;

        ::XSendEvent(disp, DefaultRootWindow(disp), False, SubstructureRedirectMask, &x_evt);
        ::XMapRaised(disp, win);
    }

    void set_topmost_across_workspaces(XWindowID winid) {
        unsigned long data = 0xFFFFFFFF;
        XChangeProperty(QX11Info::display(), winid,
                        XInternAtom(QX11Info::display(), "_NET_WM_DESKTOP", False), XA_CARDINAL, 32,
                        PropModeReplace, reinterpret_cast<unsigned char*>(&data), 1);
    }

    int get_max_workspaces() {
        auto disp = QX11Info::display();
        auto atom = ::XInternAtom(disp, "_NET_NUMBER_OF_DESKTOPS", false);
        priv::WindowProRes prop_res;
        if (!(priv::get_window_property(&prop_res, QX11Info::appRootWindow(), atom, 1,
                                        XA_CARDINAL))) {
            return -1;
        }
        unsigned long number_of_desktops = *prop_res.value_ret;
        return number_of_desktops;
    }

    int get_current_workspace_index() {
        auto disp = QX11Info::display();
        auto atom = ::XInternAtom(disp, "_NET_CURRENT_DESKTOP", false);
        priv::WindowProRes prop_res;
        if (!(priv::get_window_property(&prop_res, QX11Info::appRootWindow(), atom, 1,
                                        XA_CARDINAL))) {
            return -1;
        }
        unsigned long current_index = *prop_res.value_ret;
        return current_index;
    }

    int get_workspace_index(XWindowID win) {
        auto disp = QX11Info::display();
        auto atom = ::XInternAtom(disp, "_NET_WM_DESKTOP", false);
        priv::WindowProRes prop_res;
        if (!(priv::get_window_property(&prop_res, win, atom, 4, XA_CARDINAL))) {
            return -1;
        }
        unsigned long current_index = *prop_res.value_ret;
        return current_index;
    }

    bool switch_to_workspace(int index) {
        auto disp = QX11Info::display();
        auto root_win = QX11Info::appRootWindow(QX11Info::appScreen());

        XEvent x_evt;
        x_evt.xclient.type = ClientMessage;
        x_evt.xclient.window = root_win;
        x_evt.xclient.message_type = ::XInternAtom(disp, "_NET_CURRENT_DESKTOP", False);
        x_evt.xclient.format = 32;
        x_evt.xclient.data.l[0] = index;
        // Specify timestamp
        x_evt.xclient.data.l[1] = CurrentTime;
        x_evt.xclient.data.l[2] = 0;
        x_evt.xclient.data.l[3] = 0;
        x_evt.xclient.data.l[4] = 0;

        ::XSendEvent(disp, root_win, False, SubstructureNotifyMask | SubstructureRedirectMask,
                     &x_evt);
        ::XFlush(disp);

        return true;
    }

}
