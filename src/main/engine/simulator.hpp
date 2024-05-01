#pragma once
#ifndef __NAIVE_SIMULATOR__
#define __NAIVE_SIMULATOR__

#include <vector>
#include <functional>
#include "engine/box_quadtree.hpp"

namespace naive_engine {

class simulator {
public:
    struct entity;
    using entity_list = std::list<entity>;
    using handle_type = entity_list::iterator;

protected:
    grid_loose_quadtree boxtree;
    entity_list entities;
    sf::utils::listit_uset<handle_type> colldet_source;

public:
    simulator(double width, double height);
    ~simulator();

    auto scene_size() const { return boxtree.scene_size(); }
    void scene_size(double width, double height) { boxtree.scene_size(width, height); }
    void reset() { boxtree.reset(); }
    void reset(double width, double height) { boxtree.reset(width, height); }

    static handle_type null_handle() { return handle_type(nullptr); }
private:
    handle_type make_entity(const basics::vec2& pos);
public:
    handle_type add_entity(const basics::aabb& rect, const basics::vec2& pos = {0, 0}) {
        entities.emplace_back(rect); return make_entity(pos);
    }
    handle_type add_entity(const basics::poly_verts_const_view& poly, const basics::vec2& pos = {0, 0}) {
        entities.emplace_back(poly); return make_entity(pos);
    }
    handle_type add_entity(basics::poly_verts&& poly, const basics::vec2& pos = {0, 0}) {
        entities.emplace_back(poly); return make_entity(pos);
    }
    void del_entity(handle_type entity);

    void teleport_entity_offset(handle_type entity, basics::vec2 offset);
    void teleport_entity_center(handle_type entity, basics::vec2 center);
    auto colldet_from(handle_type entity) -> coutils::generator<std::pair<handle_type, basics::vec2>>;

    bool add_colldet_source(handle_type entity) { return colldet_source.emplace(entity).second; }
    bool del_colldet_source(handle_type entity) { return colldet_source.erase(entity); }
    auto colldet_from_sources() -> coutils::generator<std::tuple<handle_type, handle_type, basics::vec2>>;

    void on_tick(double dt);
};

struct simulator::entity {
    grid_loose_quadtree::handle_type hbox;
    basics::convex_variant convex;
    basics::vec2 velocity = {0.0, 0.0};
    struct flags_t {
        bool repulsive:1 = true; // 排斥其他物体
        bool fixed:1 = false; // 被排斥时不移动
    } flags;

    entity(const basics::aabb& rect):
        convex(std::in_place_type<basics::aabb>, rect) {}
    entity(const basics::poly_verts_const_view& poly):
        convex(std::in_place_type<basics::poly_verts>, poly.begin(), poly.end()) {}
    entity(basics::poly_verts&& poly):
        convex(std::in_place_type<basics::poly_verts>, std::move(poly)) {}
    ~entity() {}

    basics::aabb coarse() {
        if (std::holds_alternative<basics::aabb>(convex)) {
            return std::get<basics::aabb>(convex);
        } else if (std::holds_alternative<basics::poly_verts>(convex)) {
            basics::aabb box{DBL_MAX, DBL_MIN, DBL_MAX, DBL_MIN};
            for (auto&& vert : std::get<basics::poly_verts>(convex)) {
                box.left = std::min(box.left, vert.x);
                box.right = std::max(box.right, vert.x);
                box.top = std::min(box.top, vert.x);
                box.bottom = std::max(box.bottom, vert.x);
            }
            return box;
        } else {
            return {}; // 不会发生
        }
    }
};

} // namespace naive_engine

#endif // __NAIVE_SIMULATOR__
