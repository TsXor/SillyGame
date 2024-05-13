#include <algorithm>
#include <random>
#include <format>
#include "./plane_battle_scene.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts;
using namespace naive_engine;
using namespace silly_framework;

static std::random_device randev;

template <typename Rand>
ssize_t naive_randint(Rand&& rangen, ssize_t lo, ssize_t hi) {
    auto rf = double(rangen()) / double(rangen.max() - rangen.min());
    return lo + rf * double(hi - lo);
}

template <typename T, typename Pred>
static inline std::optional<std::pair<T, T>> pair_check(const T& a, const T& b, Pred&& pred) {
    if (pred(a)) { return std::make_pair(a, b); }
    if (pred(b)) { return std::make_pair(b, a); }
    return std::nullopt;
}

static constexpr ssize_t player_init_hp = 200;
static constexpr ssize_t player_damage = 25;
static constexpr ssize_t enemy_init_hp = 100;
static constexpr ssize_t enemy_damage = 25;
static constexpr ssize_t enemy_score = 100;

struct time_checker {
    using base_duration = std::chrono::microseconds;
    using clock = std::chrono::steady_clock;

    clock::time_point start;
    base_duration interval;
    size_t last_n = 0;

    template <typename DurRep, typename DurPer>
    time_checker(std::chrono::duration<DurRep, DurPer> interval_): start(clock::now()),
        interval(std::chrono::duration_cast<base_duration>(interval_)) {}

    bool is_ready(clock::time_point tnow) {
        auto dt = std::chrono::duration_cast<base_duration>(tnow - start);
        auto n = dt / interval;
        if (n > last_n) { last_n = n; return true; }
        else { return false; }
    };
};

plane_battle_scene::plane_battle_scene(sf::game_window& window) : base_activity(window) {
    parent.renman.vs_size(1024, 768);
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        auto wait_key = [&](vkey::code key) -> coutils::async_fn<void> {
            while (true) {
                co_await cohost.wait_event({cohost.EVT_KEY_CHANGE});
                if (parent.inpman.pressed(key)) { break; }
            }
        };

        std::mt19937 rangen(randev());

        // 出生点
        simu.emplace(1024, 768);
        player_plane.emplace(player_init_hp, basics::aabb(-8, 8, -8, 8), basics::vec2{512, 512});
        simu->add_entity(*player_plane);
        co_await wait_key(vkey::code::CHECK);

        time_checker
            player_shoot(std::chrono::milliseconds(500)),
            enemy_shoot(std::chrono::milliseconds(2000)),
            enemy_spawn(std::chrono::milliseconds(3000));
        
        while (true) {
            auto [evt_code, evt_data] = co_await cohost.wait_event({simu_coro_host::EVT_TICK, simu_coro_host::EVT_COLLISION});
            if (evt_code == simu_coro_host::EVT_COLLISION) {
                auto&& [coll_a, coll_b, mtv] = simu_coro_host::extract_data<simu_coro_host::evt_collision>(evt_data);
                if (auto have_wall = pair_check(coll_a, coll_b, [&](entity_t* ent) { return ent == nullptr; }); have_wall) {
                    auto&& [wall, other] = *have_wall;
                    if (auto player_bullet_it = player_bullets.find(other); player_bullet_it != player_bullets.end()) {
                        simu->del_entity(**player_bullet_it);
                        delete *player_bullet_it;
                        player_bullets.erase(player_bullet_it);
                    }
                    if (auto enemy_bullet_it = enemy_bullets.find(other); enemy_bullet_it != enemy_bullets.end()) {
                        simu->del_entity(**enemy_bullet_it);
                        delete *enemy_bullet_it;
                        enemy_bullets.erase(enemy_bullet_it);
                    }
                    if (auto enemy_plane_it = enemy_planes.find(other); enemy_plane_it != enemy_planes.end()) {
                        simu->del_entity(**enemy_plane_it);
                        delete *enemy_plane_it;
                        enemy_planes.erase(enemy_plane_it);
                    }
                } else if (auto have_player = pair_check(coll_a, coll_b, [&](entity_t* ent) { return ent == player_plane->ptr(); }); have_player) {
                    auto&& [player, other] = *have_player;
                    if (auto enemy_bullet_it = enemy_bullets.find(other); enemy_bullet_it != enemy_bullets.end()) {
                        player_plane->hp -= enemy_damage;
                        if (player_plane->hp <= 0) {
                            simu->del_entity(*player_plane);
                            player_plane.reset();
                            break;
                        }
                        simu->del_entity(**enemy_bullet_it);
                        delete *enemy_bullet_it;
                        enemy_bullets.erase(enemy_bullet_it);
                    }
                } else if (auto have_enemy = pair_check(coll_a, coll_b, [&](entity_t* ent) { return enemy_planes.contains(ent); }); have_enemy) {
                    auto&& [enemy, other] = *have_enemy;
                    if (auto player_bullet_it = player_bullets.find(other); player_bullet_it != player_bullets.end()) {
                        auto enemy_plane_it = enemy_planes.find(enemy);
                        auto& enemy_plane = *enemy_plane_it;
                        enemy_plane->hp -= player_damage;
                        if (enemy_plane->hp <= 0) {
                            simu->del_entity(**enemy_plane_it);
                            delete *enemy_plane_it;
                            enemy_planes.erase(enemy_plane_it);
                            score += enemy_score;
                        }
                        simu->del_entity(**player_bullet_it);
                        delete *player_bullet_it;
                        player_bullets.erase(player_bullet_it);
                    }
                }
            } else if (evt_code == simu_coro_host::EVT_TICK) {
                auto tnow = time_checker::clock::now();
                if (player_shoot.is_ready(tnow)) {
                    basics::vec2 bullet_vels[] = {
                        {0, base_velocity * -2.0},
                        {base_velocity * -0.5, base_velocity * -2.0},
                        {base_velocity * 0.5, base_velocity * -2.0},
                    };
                    for (auto&& bv : bullet_vels) {
                        auto bullet = new entity_node_t(basics::aabb(-8, 8, -8, 8), player_plane->data.hbox.offset);
                        bullet->data.flags.repulsive = false;
                        bullet->data.velocity = bv;
                        simu->add_entity(*bullet);
                        player_bullets.emplace(bullet);
                    }
                }
                if (enemy_shoot.is_ready(tnow)) {
                    for (auto&& enemy_plane : enemy_planes) {
                        auto bullet = new entity_node_t(basics::aabb(-8, 8, -8, 8), enemy_plane->data.hbox.offset);
                        bullet->data.flags.repulsive = false;
                        bullet->data.velocity = {0, base_velocity * 1.0};
                        simu->add_entity(*bullet);
                        enemy_bullets.emplace(bullet);
                    }
                }
                if (enemy_spawn.is_ready(tnow)) {
                    auto enemy_x = naive_randint(rangen, 128, 896);
                    auto enemy_plane = new entity_node_with_hp(enemy_init_hp, basics::aabb(-8, 8, -8, 8), basics::vec2{double(enemy_x), 128});
                    enemy_plane->data.velocity = {0, base_velocity * 0.25};
                    simu->add_entity(*enemy_plane);
                    enemy_planes.emplace(enemy_plane);
                }
            }
        }
        
        for (auto&& enemy_plane : enemy_planes) {
            simu->del_entity(*enemy_plane);
            delete enemy_plane;
        } enemy_planes.clear();
        
        for (auto&& player_bullet : player_bullets) {
            simu->del_entity(*player_bullet);
            delete player_bullet;
        } player_bullets.clear();
        
        for (auto&& enemy_bullet : enemy_bullets) {
            simu->del_entity(*enemy_bullet);
            delete enemy_bullet;
        } enemy_bullets.clear();

        co_await wait_key(vkey::code::CHECK);
        finish();
    });
}

plane_battle_scene::~plane_battle_scene() {}

static inline basics::vec2 kbd_velocity_direction(game_window& parent) {
    basics::vec2 vel{0, 0};
    if (parent.inpman.pressed(vkey::code::LEFT))  { vel.x -= 1.0; }
    if (parent.inpman.pressed(vkey::code::RIGHT)) { vel.x += 1.0; }
    if (parent.inpman.pressed(vkey::code::UP))    { vel.y -= 1.0; }
    if (parent.inpman.pressed(vkey::code::DOWN))  { vel.y += 1.0; }
    return vel;
} 

void plane_battle_scene::on_key_change() {
    cohost.on_key_change();
    velocity_direction = kbd_velocity_direction(parent);
}

void plane_battle_scene::on_key_signal() {
    cohost.on_key_signal();
}

void plane_battle_scene::on_tick(double this_time, double last_time) {
    if (player_plane) {
        player_plane->data.velocity = velocity_direction * 1.0 * base_velocity;
    }

    cohost.on_tick(this_time, last_time);
    if (simu) {
        simu->on_tick(this_time - last_time);
        for (auto&& [ent1, ent2, mtv] : simu->colldet_and_react()) {
            cohost.on_collision(ent1, ent2, mtv);
        }
    }
}

static inline void render_img(game_window& wnd, const std::string& path, const glut::position& pos) {
    auto&& [vs_w, vs_h] = wnd.renman.vs_size();
    auto img = wnd.texman.get_texture(path);
    if (img != nullptr) { wnd.renman.blit(img->tex, glut::xy_trans(pos, vs_w, vs_h, glut::eye4), glut::full_uv); }
}

static inline void render_number(game_window& wnd, ssize_t n, const glut::position& pos) {
    auto ns = std::to_string(n);
    auto rpos = pos;
    for (auto&& nc : ns) {
        render_img(wnd, std::format("power_numbers/{}.png", nc), rpos);
        rpos += glut::coord(pos.width(), 0);
    }
};

void plane_battle_scene::render() {
    gl::ClearColor(0, 0, 0, 0);
    
    auto center = basics::vec2(512, 384);

    if (player_plane) {
        auto offset = player_plane->data.hbox.offset - center;
        sprites::player_plane().render(parent, int(offset.x), int(offset.y), 1.0);
    }
    for (auto&& enemy_plane : enemy_planes) {
        auto offset = enemy_plane->data.hbox.offset - center;
        sprites::enemy_plane().render(parent, int(offset.x), int(offset.y), 1.0);
    }
    for (auto&& ent : player_bullets) {
        auto offset = ent->data.hbox.offset - center;
        sprites::player_bullet().render(parent, int(offset.x), int(offset.y), 1.0);
    }
    for (auto&& ent : enemy_bullets) {
        auto offset = ent->data.hbox.offset - center;
        sprites::enemy_bullet().render(parent, int(offset.x), int(offset.y), 1.0);
    }

    if (player_plane) {
        constexpr int health_bar_width = 128;
        int health_width = double(player_plane->hp) / double(player_init_hp) * double(health_bar_width);
        parent.renman.fill(255, 0, 0, {32, 32 + health_bar_width, 32, 64});
        parent.renman.fill(255, 255, 0, {32, 32 + health_width, 32, 64});
    }
    for (auto&& enemy_plane : enemy_planes) {
        constexpr int health_bar_width = 64;
        int health_width = double(enemy_plane->hp) / double(enemy_init_hp) * double(health_bar_width);
        auto bar_bg = glut::position(0, health_bar_width, 0, 8);
        auto bar_fg = glut::position(0, health_width, 0, 8);
        bar_fg -= bar_bg.center(); bar_bg -= bar_bg.center();
        auto bar_center = enemy_plane->data.hbox.center() + basics::vec2(0, -40);
        bar_bg += glut::coord(bar_center.x, bar_center.y);
        bar_fg += glut::coord(bar_center.x, bar_center.y);
        parent.renman.fill(255, 0, 0, bar_bg);
        parent.renman.fill(255, 255, 0, bar_fg);
    }
    render_number(parent, score, {256, 280, 24, 72});
}
