#pragma once
#ifndef __NAIVE_SIMULATOR__
#define __NAIVE_SIMULATOR__

#include <vector>
#include <functional>
#include <unordered_set>
#include <type_traits>
#include "engine/box_quadtree.hpp"
#include <boost/intrusive/list.hpp>
#include <silly_framework/utilities/coro_host.hpp>

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
        const entity_t* ptr() const { return &data; }
    };

    // entity_t为标准布局类型，其指针可以直接与hitbox的互转
    static_assert(std::is_standard_layout_v<entity_t>);

    struct entity_node_ptr_eq {
        using is_transparent = void;
        bool operator()(const entity_node_t* n1, const entity_node_t* n2) const noexcept { return n1->ptr() == n2->ptr(); }
        bool operator()(const entity_node_t* node, const entity_t* ent) const noexcept { return node->ptr() == ent; }
        bool operator()(const entity_t* ent, const entity_node_t* node) const noexcept { return (*this)(node, ent); }
    };
    struct entity_node_ptr_hash {
        using is_transparent = void;
        size_t operator()(const entity_node_t* n) const noexcept { return std::hash<const entity_t*>{}(n->ptr()); }
        size_t operator()(const entity_t* ent) const noexcept { return std::hash<const entity_t*>{}(ent); }
    };
    template <typename T>
    using entity_node_ptr_uset = std::unordered_set<T, entity_node_ptr_hash, entity_node_ptr_eq>;

protected:
    grid_loose_quadtree boxtree;
    intr::list<entity_node_t, intr::constant_time_size<false>> entities;

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
    
    using coll_info = std::tuple<entity_t*, entity_t*, basics::vec2>;
    auto colldet() -> coutils::generator<coll_info>;
    auto colldet_and_react() -> coutils::generator<coll_info>;

    void on_tick(double dt);
};

class simu_coro_host : public silly_framework::coro_host {
    using entity_t = simulator::entity_t;
public:
    enum avg_events {
        EVT_COLLISION = EVT_BASIC_MAX
    };
    struct evt_collision {
        static constexpr size_t code = EVT_COLLISION;
        using return_type = std::tuple<entity_t*, entity_t*, basics::vec2>;
    };
    void on_collision(entity_t* ent1, entity_t* ent2, basics::vec2 mtv) {
        process_all_of<evt_collision>(ent1, ent2, mtv);
    }
};

} // namespace naive_engine

#endif // __NAIVE_SIMULATOR__
