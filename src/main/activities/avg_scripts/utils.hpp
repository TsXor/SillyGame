#pragma once
#ifndef __AVG_SCIPT_UTILS__
#define __AVG_SCIPT_UTILS__

#include "../avg_scene.hpp"
#include <silly_framework/rewheel/map_utils.hpp>

namespace acts::avg_scripts::utils {

namespace sf = silly_framework;
namespace eng = naive_engine;

using scene_fn = void(*)(acts::avg_scene&);

static inline auto add_map_obstacle(avg_scene& self, const eng::basics::aabb& box) {
    auto hd = self.simu->add_entity(box, {0, 0});
    hd->flags = { .repulsive = true, .fixed = true  };
    return hd;
}

static inline auto add_trigger_box(avg_scene& self, const eng::basics::aabb& box) {
    auto hd = self.simu->add_entity(box, {0, 0});
    hd->flags = { .repulsive = false, .fixed = true  };
    return hd;
}

static inline auto listen_exits(avg_scene& self, avg_scene::entity_t person, sf::utils::listit_umap<avg_scene::entity_t, scene_fn>&& exit_map) -> coutils::async_fn<void> {
    auto moved_exit_map = std::move(exit_map);
    while (true) {
        auto [_, coll_data] = co_await self.cohost.wait_event({avg_coro_host::EVT_COLLISION});
        auto&& [coll_a, coll_b, mtv] = avg_coro_host::extract_data<avg_coro_host::evt_collision>(coll_data);
        if (coll_a == person) {
            if (auto it = moved_exit_map.find(coll_b); it != moved_exit_map.end()) {
                it->second(self); co_return;
            }
        }
    }
}

} // namespace acts::avg_scripts::utils

#endif // __AVG_SCIPT_UTILS__
