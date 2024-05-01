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
    // 出生点
    basics::vec2 spawn_pos = sf::utils::value_or(spawn_points, arg, spawn_points.at(""));
    
    self.simu.emplace(512, 352);
    auto person = self.simu->add_entity(basics::aabb(-16, 16, -12, 12), spawn_pos);
    self.simu->add_colldet_source(person);
    self.keyboard_controlled = {person, 1.0};
    self.camera_attached = person;

    self.bound_map.emplace(&maps::dorm_balcony(), 4.0);
    self.bound_sprites[person] = {&sprites::container_small(), 4.0, {0, -36}};

    // 地图边界固定阻挡物
    utils::add_map_obstacle(self, {0, 128, 0, 352});
    utils::add_map_obstacle(self, {384, 512, 0, 352});
    utils::add_map_obstacle(self, {128, 384, 0, 128});
    utils::add_map_obstacle(self, {128, 224, 320, 352});
    utils::add_map_obstacle(self, {288, 384, 320, 352});
    
    coutils::sync::unleash(utils::listen_exits(self, person, {
        {utils::add_trigger_box(self, {224, 288, 320, 352}), [](acts::avg_scene& self) { self.next<acts::avg_scene>("dorm_room", "back_door"); }}
    }));
}
