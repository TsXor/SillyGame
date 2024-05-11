#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::dorm_corridor_middle(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        int floor_number = std::stoi(arg);
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_corridor_middle_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(1024, 512);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_corridor_middle(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 1024, 0, 240}},
            {basics::aabb{0, 256, 240, 256}},
            {basics::aabb{352, 672, 240, 256}},
            {basics::aabb{768, 1024, 240, 256}},
            {basics::aabb{0, 640, 448, 512}},
            {basics::aabb{768, 1024, 448, 512}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            door_08 = {basics::aabb{256, 352, 240, 256}},
            door_07 = {basics::aabb{672, 768, 240, 256}},
            door_stairs = {basics::aabb{640, 768, 448, 512}},
            joint_left = {basics::aabb{0, 16, 256, 448}},
            joint_right = {basics::aabb{1008, 1024, 256, 448}};
        utils::add_trigger_boxes(self, joint_left, joint_right, door_stairs, door_07, door_08);

        auto jroom = [&](int room_number) { self.next<acts::avg_scene>("dorm_room", points.dorm_room_front_door, std::to_string(floor_number * 100 + room_number)); };
        utils::coll_event_table colls {
            { {person.ptr(), joint_left.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_corridor_left", points.dorm_corridor_left_joint, std::to_string(floor_number)); } },
            { {person.ptr(), joint_right.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_corridor_right", points.dorm_corridor_right_joint, std::to_string(floor_number)); } },
            { {person.ptr(), door_07.ptr()}, [&](){ jroom( 7); } },
            { {person.ptr(), door_08.ptr()}, [&](){ jroom( 8); } }
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({avg_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
