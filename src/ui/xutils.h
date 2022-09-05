#pragma once

#include <cstdint>
#include <vector>

namespace phrosty {

    using XWindowID = uint32_t;
    inline constexpr uint32_t InvalidXWindowID = UINT32_MAX;

    XWindowID get_current_workspace_topmost_window();

    XWindowID get_current_active_window();

    bool is_invalid_window(XWindowID win);

    bool is_active_window(XWindowID win);

    void set_transient(XWindowID parent_winid, XWindowID child_winid);

    void raise_window(XWindowID winid);

    void set_topmost_across_workspaces(XWindowID winid);

    int get_max_workspaces();

    int get_current_workspace_index();

    int get_workspace_index(XWindowID win);

    bool switch_to_workspace(int index);

}
