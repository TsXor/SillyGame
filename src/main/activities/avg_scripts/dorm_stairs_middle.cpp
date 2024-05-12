#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::dorm_stairs_middle(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        int floor_number = std::stoi(arg);
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_stairs_middle_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(640, 640);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_stairs_middle(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 640, 0, 472}},
            {basics::aabb{0, 128, 472, 640}},
            {basics::aabb{512, 640, 472, 640}},
            {basics::aabb{128, 512, 632, 640}},
            {basics::aabb{304, 336, 584, 632}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            downstairs = {basics::aabb{128, 304, 600, 632}},
            upstairs = {basics::aabb{336, 512, 600, 632}};
        utils::add_trigger_boxes(self, upstairs, downstairs);

        utils::coll_event_table colls {
            { {person.ptr(), downstairs.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_stairs", points.dorm_stairs_up, std::to_string(floor_number - 1)); } },
            { {person.ptr(), upstairs.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_stairs", points.dorm_stairs_down, std::to_string(floor_number)); } },
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({avg_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
