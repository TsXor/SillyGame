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
    // 出生点
    basics::vec2 spawn_pos = sf::utils::value_or(spawn_points, arg, spawn_points.at(""));
    
    self.simu.emplace(512, 1024);
    auto person = self.simu->add_entity(basics::aabb(-16, 16, -12, 12), spawn_pos);
    self.simu->add_colldet_source(person);
    self.keyboard_controlled = {person, 1.0};
    self.camera_attached = person;

    self.bound_map.emplace(&maps::dorm_corridor_left(), 4.0);
    self.bound_sprites[person] = {&sprites::container_small(), 4.0, {0, -36}};

    // 地图边界固定阻挡物
    utils::add_map_obstacle(self, {128, 384, 0, 128});
    utils::add_map_obstacle(self, {0, 128, 0, 288});
    utils::add_map_obstacle(self, {0, 128, 288, 384}); // 左边第一个门
    utils::add_map_obstacle(self, {0, 128, 384, 768});
    utils::add_map_obstacle(self, {0, 128, 768, 960}); // 这个位置是水房门，水房画好就会移除
    utils::add_map_obstacle(self, {0, 128, 960, 1024});
    utils::add_map_obstacle(self, {384, 512, 0, 288});
    utils::add_map_obstacle(self, {384, 512, 384, 576});
    utils::add_map_obstacle(self, {384, 512, 576, 672}); // 左边第二个门
    utils::add_map_obstacle(self, {384, 512, 672, 864});
    utils::add_map_obstacle(self, {384, 512, 864, 960}); // 左边第三个门
    utils::add_map_obstacle(self, {384, 512, 960, 1024});
    
    coutils::sync::unleash(utils::listen_exits(self, person, {
        {utils::add_trigger_box(self, {384, 512, 288, 384}), [](acts::avg_scene& self) { self.next<acts::avg_scene>("dorm_room", "front_door"); }}
    }));
}
