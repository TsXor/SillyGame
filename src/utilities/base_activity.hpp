#pragma once
#ifndef __BASE_ACTIVITY__
#define __BASE_ACTIVITY__

#include "facilities/iface_activity.hpp"
#include "facilities/game_window.hpp"

/**
 * Activity基类。大多数情况下，我们需要保存所属窗口的引用。
 */
struct base_activity : public iface_activity {
    game_window& parent;
    base_activity(game_window& wnd) : parent(wnd) {}

    template <typename SceneT, typename... ArgTs>
    void call(ArgTs&&... args) {
        parent.actman.emplace<SceneT>(parent, std::forward<ArgTs>(args)...);
    }

    template <typename SceneT, typename... ArgTs>
    void next(ArgTs&&... args) {
        parent.actman.switch_to<SceneT>(parent, std::forward<ArgTs>(args)...);
    }

    void finish() { parent.actman.pop(); }
};

#endif // __BASE_ACTIVITY__
