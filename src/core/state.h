#pragma once

#include <mutex>
#include <functional>

#define PHROSTY_DEF_STATE(tp_name, method_name)                                                    \
  private:                                                                                         \
    tp_name m_##method_name;                                                                       \
                                                                                                   \
  public:                                                                                          \
    void set_##method_name##_synced(const std::function<void(tp_name&)>& setter) {                 \
        std::lock_guard<std::mutex> lock(m_mtx);                                                   \
        setter(m_##method_name);                                                                   \
    }                                                                                              \
    const tp_name& method_name##_synced() {                                                        \
        std::lock_guard<std::mutex> lock(m_mtx);                                                   \
        return m_##method_name;                                                                    \
    }

namespace phrosty {

    struct UIState {
        std::pair<int, int> init_win_resolution = {640, 360};
        double opacity = 1.0;
        bool topmost = false;
        bool show_notification = true;
        std::string border = "";
    };

    struct RenderState {
        double fps = 24;
    };

    struct TraceState {
        unsigned long window_id = 0;
    };

    class State final {
      public:
        static inline State& get() {
            static State s_instance;
            return s_instance;
        }

        PHROSTY_DEF_STATE(UIState, ui_state);
        PHROSTY_DEF_STATE(RenderState, render_state);
        PHROSTY_DEF_STATE(TraceState, trace_state);

      private:
        explicit State() = default;
        virtual ~State() = default;

      private:
        std::mutex m_mtx;
    };

}
