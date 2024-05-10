#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

const std::unordered_map<std::string, basics::vec2> spawn_points = {
    {"", {256, 160}},
    {"door", {256, 304}},
};

void acts::avg_scripts::dorm_balcony(avg_scene& self, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        // 出生点
        basics::vec2 spawn_pos = sf::utils::value_or(spawn_points, arg, spawn_points.at(""));
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
        
        simulator::entity_node_t triggers[] = {
            {basics::aabb{224, 288, 320, 352}} // 0, 门
        };
        utils::add_trigger_boxes(self, triggers);

        utils::coll_event_table colls {
            { {person.ptr(), triggers[0].ptr()}, [&](){ self.next<acts::avg_scene>("dorm_room", "back_door"); } }
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({avg_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
