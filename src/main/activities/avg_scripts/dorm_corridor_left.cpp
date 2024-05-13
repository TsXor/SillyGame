#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "activities/render_utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::dorm_corridor_left(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        int floor_number = std::stoi(arg);
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_corridor_left_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(512, 1024);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_corridor_left(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        self.cust_render = [&, floor_number]() {
            render_number(self.parent, floor_number, {24, 48, 24, 72});
        };

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{128, 384, 0, 128}},
            {basics::aabb{0, 112, 0, 1024}},
            {basics::aabb{112, 128, 0, 288}},
            {basics::aabb{112, 128, 384, 768}},
            {basics::aabb{112, 128, 960, 1024}},
            {basics::aabb{400, 512, 0, 1024}},
            {basics::aabb{384, 400, 0, 288}},
            {basics::aabb{384, 400, 384, 576}},
            {basics::aabb{384, 400, 672, 864}},
            {basics::aabb{384, 400, 960, 1024}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            door_32 = {basics::aabb{112, 128, 288, 384}},
            door_restroom = {basics::aabb{112, 128, 768, 960}},
            door_11 = {basics::aabb{384, 400, 288, 384}},
            door_10 = {basics::aabb{384, 400, 576, 672}},
            door_09 = {basics::aabb{384, 400, 864, 960}},
            joint = {basics::aabb{128, 384, 1008, 1024}};
        utils::add_trigger_boxes(self, joint, door_09, door_10, door_11, door_32);

        auto jroom = [&](int room_number) { self.next<acts::avg_scene>("dorm_room", points.dorm_room_front_door, std::to_string(floor_number * 100 + room_number)); };
        utils::coll_event_table colls {
            { {person.ptr(), joint.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_corridor_middle", points.dorm_corridor_middle_joint_left, std::to_string(floor_number)); } },
            { {person.ptr(), door_09.ptr()}, [&](){ jroom( 9); } },
            { {person.ptr(), door_10.ptr()}, [&](){ jroom(10); } },
            { {person.ptr(), door_11.ptr()}, [&](){ jroom(11); } },
            { {person.ptr(), door_32.ptr()}, [&](){ jroom(32); } }
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({simu_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
