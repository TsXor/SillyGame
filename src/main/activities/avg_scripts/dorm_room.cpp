#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

const std::unordered_map<std::string, basics::vec2> spawn_points = {
    {"", {256, 512}},
    {"front_door", {256, 1008}},
    {"back_door", {256, 144}},
};

void acts::avg_scripts::dorm_room(avg_scene& self, const std::string& arg) {
    // 出生点
    basics::vec2 spawn_pos = sf::utils::value_or(spawn_points, arg, spawn_points.at(""));
    
    self.simu.emplace(512, 1056);
    auto person = self.simu->add_entity(basics::aabb(-16, 16, -12, 12), spawn_pos);
    self.simu->add_colldet_source(person);
    self.keyboard_controlled = {person, 1.0};
    self.camera_attached = person;

    self.bound_map.emplace(&maps::dorm_room(), 4.0);
    self.bound_sprites[person] = {&sprites::container_small(), 4.0, {0, -36}};

    // 地图边界固定阻挡物
    utils::add_map_obstacle(self, {0, 128, 0, 1056});
    utils::add_map_obstacle(self, {384, 512, 0, 1056});
    utils::add_map_obstacle(self, {128, 224, 0, 128});
    utils::add_map_obstacle(self, {288, 384, 0, 128});
    utils::add_map_obstacle(self, {128, 224, 1024, 1056});
    utils::add_map_obstacle(self, {288, 384, 1024, 1056});
    
    coutils::sync::unleash(utils::listen_exits(self, person, {
        {utils::add_trigger_box(self, {224, 288, 1024, 1056}), [](acts::avg_scene& self) { self.next<acts::avg_scene>("dorm_corridor_left", "door_unbox"); }},
        {utils::add_trigger_box(self, {224, 288, 96, 128}), [](acts::avg_scene& self) { self.next<acts::avg_scene>("dorm_balcony", "door"); }}
    }));
}
