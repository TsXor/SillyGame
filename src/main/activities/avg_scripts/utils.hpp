#pragma once
#ifndef __AVG_SCIPT_UTILS__
#define __AVG_SCIPT_UTILS__

#include "../avg_scene.hpp"
#include <silly_framework/rewheel/map_utils.hpp>
#include <silly_framework/rewheel/hash_utils.hpp>

namespace acts::avg_scripts::utils {

namespace sf = silly_framework;
namespace eng = naive_engine;

using coll_event_table = sf::utils::unordered_upair_map<eng::simulator::entity_t*, std::function<void()>>;

static inline void add_main_char(avg_scene& self, eng::simulator::entity_node_t& person) {
    self.simu->add_entity(person);
    self.simu->add_colldet_source(person.ptr());
    self.keyboard_controlled = {person.ptr(), 1.0};
    self.camera_attached = person.ptr();
}

static inline auto add_map_obstacles(avg_scene& self, const std::span<eng::simulator::entity_node_t>& obstacles) {
    for (auto&& obstacle : obstacles) {
        obstacle.data.flags = { .repulsive = true, .fixed = true  };
        self.simu->add_entity(obstacle);
    }
}

static inline auto add_trigger_boxes(avg_scene& self, const std::span<eng::simulator::entity_node_t>& triggers) {
    for (auto&& trigger : triggers) {
        trigger.data.flags = { .repulsive = false, .fixed = true  };
        self.simu->add_entity(trigger);
    }
}

static inline bool check_coll_events(avg_scene& self, const coll_event_table& coll_table, const std::any& evt_data) {
    auto&& [coll_a, coll_b, mtv] = avg_coro_host::extract_data<avg_coro_host::evt_collision>(evt_data);
    auto coll_it = coll_table.find(std::make_pair(coll_a, coll_b));
    if (coll_it != coll_table.end()) { coll_it->second(); return true; }
    return false;
}

} // namespace acts::avg_scripts::utils

#endif // __AVG_SCIPT_UTILS__
