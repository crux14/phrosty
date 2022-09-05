#include "./store.h"

#include <core/state.h>

namespace phrosty {

    Store::Store(QObject* parent) : QObject(parent) {}

    void Store::change_fps(unsigned int new_value) {
        State::get().set_render_state_synced([new_value](RenderState& rs) { rs.fps = new_value; });
        Q_EMIT this->fps_changed(new_value);
    }

    void Store::change_opacity(double new_value) {
        State::get().set_ui_state_synced([new_value](UIState& us) { us.opacity = new_value; });
        Q_EMIT this->opacity_changed(new_value);
    }

}
