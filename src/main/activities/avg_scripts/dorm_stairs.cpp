#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "activities/render_utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::dorm_stairs(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        int floor_number = std::stoi(arg);
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.dorm_stairs_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(640, 384);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_stairs(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        self.cust_render = [&, floor_number]() {
            render_number(self.parent, floor_number, {24, 48, 24, 72});
        };

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 128, 0, 384}},
            {basics::aabb{512, 640, 0, 384}},
            {basics::aabb{128, 256, 0, 128}},
            {basics::aabb{384, 512, 0, 128}},
            {basics::aabb{128, 512, 288, 384}},
            {basics::aabb{304, 336, 240, 288}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            downstairs = {basics::aabb{128, 304, 256, 288}},
            upstairs = {basics::aabb{336, 512, 256, 288}},
            door = {basics::aabb{256, 384, 0, 128}};
        utils::add_trigger_boxes(self, door, upstairs, downstairs);

        utils::coll_event_table colls {
            { {person.ptr(), door.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_corridor_middle", points.dorm_corridor_middle_door, std::to_string(floor_number)); } },
            { {person.ptr(), downstairs.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_stairs_middle", points.dorm_stairs_middle_up, std::to_string(floor_number)); } },
            { {person.ptr(), upstairs.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_stairs_middle", points.dorm_stairs_middle_down, std::to_string(floor_number + 1)); } },
        };

        if (floor_number == 7) {
            self.bound_map->map = &maps::dorm_stairs_top();
            door.data.flags.repulsive = true;
            colls.erase({person.ptr(), door.ptr()}); // 我门难开
            colls.erase({person.ptr(), upstairs.ptr()}); // 上面没了
        }
        if (floor_number == 1) {
            colls[{person.ptr(), downstairs.ptr()}] = [&](){ self.next<acts::avg_scene>("dorm_stairs_bottom", points.dorm_stairs_bottom_up); };
        }

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({simu_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
