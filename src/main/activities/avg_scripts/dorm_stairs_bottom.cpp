#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::dorm_stairs_bottom(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_stairs_bottom_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(640, 640);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_stairs_bottom(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 64, 0, 640}},
            {basics::aabb{256, 384, 0, 640}},
            {basics::aabb{64, 256, 0, 192}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            upstairs = {basics::aabb{64, 256, 576, 608}};
        utils::add_trigger_boxes(self, upstairs);

        utils::coll_event_table colls {
            { {person.ptr(), upstairs.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_stairs", points.dorm_stairs_down, "1"); } },
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({avg_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
