#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "activities/render_utils.hpp"
#include "activities/plane_battle_scene.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

static void go_out(acts::avg_scene& self, int room_number) {
    auto floor_arg = std::to_string(room_number / 100);
    auto jcl = [&](const basics::vec2& pos) { self.next<acts::avg_scene>("dorm_corridor_left", pos, floor_arg); };
    auto jcm = [&](const basics::vec2& pos) { self.next<acts::avg_scene>("dorm_corridor_middle", pos, floor_arg); };
    auto jcr = [&](const basics::vec2& pos) { self.next<acts::avg_scene>("dorm_corridor_right", pos, floor_arg); };
    switch (room_number % 100) {
        case  1: jcr(points.dorm_corridor_door_01); break;
        case  2: jcr(points.dorm_corridor_door_02); break;
        case  3: jcr(points.dorm_corridor_door_03); break;
        case  4: jcr(points.dorm_corridor_door_04); break;
        case  5: jcr(points.dorm_corridor_door_05); break;
        case  6: jcr(points.dorm_corridor_door_06); break;
        case  7: jcm(points.dorm_corridor_door_07); break;
        case  8: jcm(points.dorm_corridor_door_08); break;
        case  9: jcl(points.dorm_corridor_door_09); break;
        case 10: jcl(points.dorm_corridor_door_10); break;
        case 11: jcl(points.dorm_corridor_door_11); break;
        case 32: jcl(points.dorm_corridor_door_32); break;
        case 33: jcr(points.dorm_corridor_door_33); break;
        case 34: jcr(points.dorm_corridor_door_34); break;
        case 35: jcr(points.dorm_corridor_door_35); break;
        case 36: jcr(points.dorm_corridor_door_36); break;
        case 37: jcr(points.dorm_corridor_door_37); break;
        case 38: jcr(points.dorm_corridor_door_38); break;
    }
}

void acts::avg_scripts::dorm_room(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        int room_number = std::stoi(arg);
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_room_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(512, 1056);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_room(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        self.cust_render = [&, room_number]() {
            render_number(self.parent, room_number, {24, 48, 24, 72});
        };

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 128, 0, 1056}},
            {basics::aabb{384, 512, 0, 1056}},
            {basics::aabb{128, 224, 0, 128}},
            {basics::aabb{288, 384, 0, 128}},
            {basics::aabb{128, 224, 1024, 1056}},
            {basics::aabb{288, 384, 1024, 1056}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            front_door = {basics::aabb{224, 288, 1024, 1056}},
            back_door = {basics::aabb{224, 288, 96, 128}},
            table_1 = {basics::aabb{128, 144, 352, 400}},
            table_2 = {basics::aabb{368, 384, 352, 400}},
            table_3 = {basics::aabb{128, 144, 768, 816}},
            table_4 = {basics::aabb{368, 384, 768, 816}};
        utils::add_trigger_boxes(self, front_door, back_door, table_1, table_2, table_3, table_4);

        utils::coll_event_table colls {
            { {person.ptr(), front_door.ptr()}, [&](){ go_out(self, room_number); } },
            { {person.ptr(), back_door.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_balcony", points.dorm_balcony_door, std::to_string(room_number)); } }
        };

        if (room_number == 311) {
            colls[{person.ptr(), table_2.ptr()}] = [&](){ self.next<acts::avg_scene>("black_jack_entry", points.black_jack_entry_left); };
            colls[{person.ptr(), table_3.ptr()}] = [&](){ self.next<acts::avg_scene>("dorm_room", std::nullopt, "508"); };
        }
        if (room_number == 238) {
            colls[{person.ptr(), table_2.ptr()}] = [&](){
                person.data.velocity = {0, 0};
                self.simu->teleport_entity_center(person.ptr(), points.dorm_room_default);
                self.call<acts::plane_battle_scene>();
            };
        }

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({simu_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
