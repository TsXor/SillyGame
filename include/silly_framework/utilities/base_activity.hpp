#pragma once
#ifndef __SFUT_BASE_ACTIVITY__
#define __SFUT_BASE_ACTIVITY__

#include "silly_framework/facilities/iface_activity.hpp"
#include "silly_framework/facilities/game_window.hpp"

namespace silly_framework {

/**
 * Activity基类。大多数情况下，我们需要保存所属窗口的引用。
 */
struct base_activity : public iface_activity {
    game_window& parent;
    base_activity(game_window& wnd) : parent(wnd) {}

    template <typename SceneT, typename... ArgTs>
    void call(ArgTs&&... args) {
        parent.inpman.clear_state();
        on_key_signal(); on_key_change();
        parent.actman.post_emplace<SceneT>(parent, std::forward<ArgTs>(args)...);
    }

    template <typename SceneT, typename... ArgTs>
    void next(ArgTs&&... args) {
        parent.actman.post_switch<SceneT>(parent, std::forward<ArgTs>(args)...);
    }

    void finish() { parent.actman.post_pop(); }
};

} // namespace silly_framework

#endif // __SFUT_BASE_ACTIVITY__
