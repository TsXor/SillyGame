#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

const std::unordered_map<std::string, basics::vec2> spawn_points = {
    {"", {256, 512}},
    {"door_unbox", {364, 336}}, // 笑点解析：unbox直译为拆箱，意译为开盒
};

void acts::avg_scripts::dorm_corridor_left(avg_scene& self, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        // 出生点
        basics::vec2 spawn_pos = sf::utils::value_or(spawn_points, arg, spawn_points.at(""));
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(512, 1024);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::dorm_corridor_left(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{128, 384, 0, 128}},
            {basics::aabb{0, 128, 0, 288}},
            {basics::aabb{0, 128, 288, 384}}, // 左边第一个门
            {basics::aabb{0, 128, 384, 768}},
            {basics::aabb{0, 128, 768, 960}}, // 这个位置是水房门，水房画好就会移除
            {basics::aabb{0, 128, 960, 1024}},
            {basics::aabb{384, 512, 0, 288}},
            {basics::aabb{384, 512, 384, 576}},
            {basics::aabb{384, 512, 576, 672}}, // 左边第二个门
            {basics::aabb{384, 512, 672, 864}},
            {basics::aabb{384, 512, 864, 960}}, // 左边第三个门
            {basics::aabb{384, 512, 960, 1024}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t triggers[] = {
            {basics::aabb{384, 512, 288, 384}} // 0, 开盒之门
        };
        utils::add_trigger_boxes(self, triggers);

        utils::coll_event_table colls {
            { {person.ptr(), triggers[0].ptr()}, [&](){ self.next<acts::avg_scene>("dorm_room", "front_door"); } }
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({avg_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
