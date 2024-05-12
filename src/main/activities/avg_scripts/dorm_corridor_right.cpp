#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::dorm_corridor_right(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&, arg]() -> coutils::async_fn<void> {
        int floor_number = std::stoi(arg);
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_corridor_right_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(512, 1920);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_corridor_right(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{128, 384, 0, 128}},
            {basics::aabb{0, 112, 0, 1920}},
            {basics::aabb{112, 128, 0, 320}},
            {basics::aabb{112, 128, 416, 608}},
            {basics::aabb{112, 128, 704, 896}},
            {basics::aabb{112, 128, 992, 1184}},
            {basics::aabb{112, 128, 1280, 1472}},
            {basics::aabb{112, 128, 1568, 1760}},
            {basics::aabb{112, 128, 1856, 1920}},
            {basics::aabb{400, 512, 0, 1920}},
            {basics::aabb{384, 400, 0, 320}},
            {basics::aabb{384, 400, 416, 608}},
            {basics::aabb{384, 400, 704, 896}},
            {basics::aabb{384, 400, 992, 1184}},
            {basics::aabb{384, 400, 1280, 1472}},
            {basics::aabb{384, 400, 1568, 1760}},
            {basics::aabb{384, 400, 1856, 1920}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            door_01 = {basics::aabb{112, 128, 320, 416}},
            door_02 = {basics::aabb{112, 128, 608, 704}},
            door_03 = {basics::aabb{112, 128, 896, 992}},
            door_04 = {basics::aabb{112, 128, 1184, 1280}},
            door_05 = {basics::aabb{112, 128, 1472, 1568}},
            door_06 = {basics::aabb{112, 128, 1760, 1856}},
            door_38 = {basics::aabb{384, 400, 320, 416}},
            door_37 = {basics::aabb{384, 400, 608, 704}},
            door_36 = {basics::aabb{384, 400, 896, 992}},
            door_35 = {basics::aabb{384, 400, 1184, 1280}},
            door_34 = {basics::aabb{384, 400, 1472, 1568}},
            door_33 = {basics::aabb{384, 400, 1760, 1856}},
            joint = {basics::aabb{128, 384, 1904, 1920}};
        utils::add_trigger_boxes(self, joint, door_01, door_02, door_03, door_04, door_05, door_06, door_33, door_34, door_35, door_36, door_37, door_38);

        auto jroom = [&](int room_number) { self.next<acts::avg_scene>("dorm_room", points.dorm_room_front_door, std::to_string(floor_number * 100 + room_number)); };
        utils::coll_event_table colls {
            { {person.ptr(), joint.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_corridor_middle", points.dorm_corridor_middle_joint_right, std::to_string(floor_number)); } },
            { {person.ptr(), door_01.ptr()}, [&](){ jroom( 1); } },
            { {person.ptr(), door_02.ptr()}, [&](){ jroom( 2); } },
            { {person.ptr(), door_03.ptr()}, [&](){ jroom( 3); } },
            { {person.ptr(), door_04.ptr()}, [&](){ jroom( 4); } },
            { {person.ptr(), door_05.ptr()}, [&](){ jroom( 5); } },
            { {person.ptr(), door_06.ptr()}, [&](){ jroom( 6); } },
            { {person.ptr(), door_33.ptr()}, [&](){ jroom(33); } },
            { {person.ptr(), door_34.ptr()}, [&](){ jroom(34); } },
            { {person.ptr(), door_35.ptr()}, [&](){ jroom(35); } },
            { {person.ptr(), door_36.ptr()}, [&](){ jroom(36); } },
            { {person.ptr(), door_37.ptr()}, [&](){ jroom(37); } },
            { {person.ptr(), door_38.ptr()}, [&](){ jroom(38); } },
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({simu_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
