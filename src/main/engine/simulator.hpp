#pragma once
#ifndef __NAIVE_SIMULATOR__
#define __NAIVE_SIMULATOR__

#include <vector>
#include "framework.hpp"
#include "main/engine/box_quadtree.hpp"

namespace naive_engine {

class simulator {
public:
    struct entity {
        struct static_data {
            const sprite2d& spr;
            unsigned int width, height;
            std::vector<basics::aabb> boxes;
            std::vector<basics::poly_verts> polys;
        };

        friend class simulator;
        const static_data* data;
        basics::vec2 velocity = {0.0, 0.0};
        grid_loose_quadtree::handle_type hbox;

        entity(const static_data& data) : data(&data) {}
        ~entity() {}

        basics::vec2 center() const {
            return hbox->offset + basics::vec2(data->width / 2, data->height / 2);
        }
        basics::polygon_const_ref poly_at(size_t idx) const {
            return {data->boxes[idx], data->polys[idx]};
        }
    };

protected:
    grid_loose_quadtree boxtree;
    std::list<entity> entities;

public:
    std::optional<std::pair<map2d, double>> current_map;

    simulator(double width, double height);
    ~simulator();

    using handle_type = decltype(entities)::iterator;

    auto scene_size() const { return boxtree.scene_size(); }

    handle_type add_entity(const entity::static_data& data, const basics::vec2& center = {0, 0});
    void del_entity(handle_type entity);

    void teleport_entity_offset(handle_type entity, basics::vec2 offset);
    void teleport_entity_center(handle_type entity, basics::vec2 center);
    auto colldet(handle_type entity) -> cppcoro::generator<std::pair<handle_type, basics::vec2>>;

    void tick(double dt);
    void render(game_window& wnd, basics::vec2 pos);
};

} // namespace naive_engine

#endif // __NAIVE_SIMULATOR__
