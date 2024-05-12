#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::dorm_balcony(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        int room_number = std::stoi(arg);
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_balcony_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(512, 352);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_balcony(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 128, 0, 352}},
            {basics::aabb{384, 512, 0, 352}},
            {basics::aabb{128, 384, 0, 128}},
            {basics::aabb{128, 224, 320, 352}},
            {basics::aabb{288, 384, 320, 352}}
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t door = {basics::aabb{224, 288, 320, 352}};
        utils::add_trigger_boxes(self, door);

        utils::coll_event_table colls {
            { {person.ptr(), door.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_room", points.dorm_room_back_door, std::to_string(room_number)); } }
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({simu_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
