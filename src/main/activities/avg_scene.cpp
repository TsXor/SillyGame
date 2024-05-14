#include "./avg_scene.hpp"
#include "./avg_scripts.hpp"

using namespace acts;
using namespace naive_engine;
using namespace silly_framework;

constexpr struct {
    unsigned int width = 1024;
    unsigned int height = 768;
} avg_vs_size;

avg_scene::string_map<acts::avg_scene::script_t> avg_scene::scene_scripts = {
    {"dorm_room", avg_scripts::dorm_room},
    {"dorm_balcony", avg_scripts::dorm_balcony},
    {"dorm_corridor_left", avg_scripts::dorm_corridor_left},
    {"dorm_corridor_middle", avg_scripts::dorm_corridor_middle},
    {"dorm_corridor_right", avg_scripts::dorm_corridor_right},
    {"dorm_stairs", avg_scripts::dorm_stairs},
    {"dorm_stairs_middle", avg_scripts::dorm_stairs_middle},
    {"dorm_stairs_bottom", avg_scripts::dorm_stairs_bottom},
    {"black_jack_entry", avg_scripts::black_jack_entry},
    {"genshin_pursuit_entry", avg_scripts::genshin_pursuit_entry},
    {"genshin_pursuit_author", avg_scripts::genshin_pursuit_author},
    {"genshin_pursuit_main", avg_scripts::genshin_pursuit_main},
};

avg_scene::entity_comp_t avg_scene::default_sprites_sorter() {
    return [](const entity_t& e1, const entity_t& e2) -> bool {
        return e1.hbox.offset.y < e2.hbox.offset.y;
    };
}

avg_scene::avg_scene(sf::game_window& window, const std::string& name, const std::optional<eng::basics::vec2>& spawn, const std::string& arg):
base_activity(window) {
    parent.renman.vs_size(avg_vs_size.width, avg_vs_size.height);
    if (scene_scripts.contains(name)) { scene_scripts[name](*this, spawn, arg); }
}

avg_scene::~avg_scene() {}

void avg_scene::on_tick(double this_time, double last_time) {
    auto&& [kbd_ent, kbd_vel] = keyboard_controlled;
    if (kbd_ent != nullptr) {
        kbd_ent->velocity = velocity_direction * kbd_vel * base_velocity;
    }

    cohost.on_tick(this_time, last_time);
    if (simu) {
        simu->on_tick(this_time - last_time);
        for (auto&& [ent1, ent2, mtv] : simu->colldet_and_react()) {
            cohost.on_collision(ent1, ent2, mtv);
        }
    }

    if (camera_attached != nullptr && bound_map) {
        auto offset = camera_attached->hbox.offset / bound_map->scalev;
        camera_pos = {int(offset.x), int(offset.y)};
    }
}

static inline basics::vec2 kbd_velocity_direction(game_window& parent) {
    basics::vec2 vel{0, 0};
    if (parent.inpman.pressed(vkey::code::LEFT))  { vel.x -= 1.0; }
    if (parent.inpman.pressed(vkey::code::RIGHT)) { vel.x += 1.0; }
    if (parent.inpman.pressed(vkey::code::UP))    { vel.y -= 1.0; }
    if (parent.inpman.pressed(vkey::code::DOWN))  { vel.y += 1.0; }
    return vel;
} 

void avg_scene::on_key_change() {
    cohost.on_key_change();
    velocity_direction = kbd_velocity_direction(parent);
}

void avg_scene::on_key_signal() {
    cohost.on_key_signal();
}

void avg_scene::render() {
    gl::ClearColor(0, 0, 0, 0);
    if (bound_map) { bound_map->render(parent, camera_pos); }
    
    std::vector<decltype(bound_sprites)::value_type*> sprites;
    sprites.reserve(bound_sprites.size());
    for (auto&& val : bound_sprites) { sprites.push_back(&val); }
    
    using sortee_type = decltype(sprites)::value_type;
    std::sort(sprites.begin(), sprites.end(),
        [&](const sortee_type& lhs, const sortee_type& rhs) {
            return sprites_sorter(*lhs->first, *rhs->first);
        }
    );
    for (auto&& ptr : sprites) {
        auto&& [ent, spr] = *ptr;
        auto offset = ent->hbox.offset / spr.scalev;
        spr.render(parent, glut::coord{int(offset.x), int(offset.y)} - camera_pos);
    }
    if (cust_render) { cust_render(); }
}
