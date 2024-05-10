#pragma once
#ifndef __NAIVE_SIMULATOR__
#define __NAIVE_SIMULATOR__

#include <vector>
#include <functional>
#include <unordered_set>
#include <type_traits>
#include "engine/box_quadtree.hpp"
#include <boost/intrusive/list.hpp>

namespace naive_engine {

namespace intr = boost::intrusive;
using auto_unlink_hook = intr::list_base_hook<intr::link_mode<intr::auto_unlink>>;

class simulator {
public:
    struct entity_t {
        grid_loose_quadtree::hitbox hbox;
        basics::convex_variant convex;
        basics::vec2 velocity = {0.0, 0.0};
        struct flags_t {
            bool repulsive:1 = true; // 排斥其他物体
            bool fixed:1 = false; // 被排斥时不移动
        } flags;

        template <typename ArgT>
        entity_t(ArgT&& poly_arg, const basics::vec2& pos = {0, 0}): hbox({0, 0}, pos),
            convex(basics::make_convex_variant(std::forward<ArgT>(poly_arg)))
                { hbox.box = basics::coarse(convex); }
        ~entity_t() {}
    };

    struct entity_node_t : public auto_unlink_hook {
        entity_t data;
        template <typename... ArgTs>
        entity_node_t(ArgTs&&... args) : data(std::forward<ArgTs>(args)...) {}
        entity_t* ptr() { return &data; }
    };

    // entity_t为标准布局类型，其指针可以直接与hitbox的互转
    static_assert(std::is_standard_layout_v<entity_t>);

protected:
    grid_loose_quadtree boxtree;
    intr::list<entity_node_t, intr::constant_time_size<false>> entities;
    std::unordered_set<entity_t*> colldet_source;

public:
    simulator(double width, double height);
    ~simulator();

    auto scene_size() const { return boxtree.scene_size(); }
    void scene_size(double width, double height) { boxtree.scene_size(width, height); }
    void reset() { boxtree.reset(); }
    void reset(double width, double height) { boxtree.reset(width, height); }

    void add_entity(entity_node_t& entity);
    void del_entity(entity_node_t& entity);

    void teleport_entity_offset(entity_t* entity, basics::vec2 offset);
    void teleport_entity_center(entity_t* entity, basics::vec2 center);
    
    auto colldet_from(entity_t* entity) -> coutils::generator<std::pair<entity_t*, basics::vec2>>;
    bool add_colldet_source(entity_t* entity) { return colldet_source.emplace(entity).second; }
    bool del_colldet_source(entity_t* entity) { return colldet_source.erase(entity); }
    auto colldet_from_sources() -> coutils::generator<std::tuple<entity_t*, entity_t*, basics::vec2>>;

    void on_tick(double dt);
};

} // namespace naive_engine

#endif // __NAIVE_SIMULATOR__
