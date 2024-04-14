#include "box_quadtree.hpp"

using namespace naive_engine;

static inline size_t layer_start(unsigned char n) {
    return ((size_t(1) << (2 * n)) - 1) / 3; // (4^n - 1) / 3 
}

static inline unsigned char size_depth(double l) {
    double ls = 1.0; char depth = 0;
    do { ls /= 2.0; ++depth; } while (ls > l);
    return depth - 1;
}

static inline unsigned char box_depth(const basics::aabb& box) {
    double width = box.right - box.left;
    double height = box.bottom - box.top;
    return std::min(size_depth(width), size_depth(height));
}

static inline ssize_t pos_idx(double pos, unsigned char depth) {
    if (depth == 0) { return 0; }
    return pos * static_cast<double>(size_t(1) << depth);
}

static inline size_t combine_idx(ssize_t x, ssize_t y, unsigned char depth) {
    return layer_start(depth) + ((y << depth) | x);
}

grid_loose_quadtree::grid_loose_quadtree(unsigned char depth, double width, double height):
max_depth(depth), vtree(layer_start(depth)), scene_width(width), scene_height(height) {}

grid_loose_quadtree::~grid_loose_quadtree() {}

basics::aabb grid_loose_quadtree::normalize(const basics::aabb& box) {
    return { box.left / scene_width, box.right / scene_width, box.top / scene_height, box.bottom / scene_height };
}

grid_loose_quadtree::hitbox_set& grid_loose_quadtree::box_pos(const basics::aabb& box) {
    auto depth = std::min(box_depth(box), max_depth);
    double bcx = (box.left + box.right) / 2;
    double bcy = (box.top + box.bottom) / 2;
    ssize_t ix = pos_idx(bcx, depth), iy = pos_idx(bcy, depth);
    ssize_t max_idx = (ssize_t(1) << depth) - 1;
    auto clamp = [&](ssize_t v) -> ssize_t {
        if (v < 0) return 0;
        if (v > max_idx) return max_idx;
        return v;
    };
    return vtree[combine_idx(clamp(ix), clamp(iy), depth)];
}

auto grid_loose_quadtree::add_box(void* parent, const basics::vec2& offset, const std::span<const basics::aabb>& boxes) -> grid_loose_quadtree::handle_type {
    auto group = box_data.emplace(box_data.end(), parent, offset, boxes);
    for (size_t i = 0; i < group->boxes.size(); ++i) {
        grouped_hitbox_ref boxptr{*group, i};
        box_pos(normalize(*boxptr)).emplace(boxptr);
    }
    return group;
}

void grid_loose_quadtree::del_box(handle_type group) {
    for (size_t i = 0; i < group->boxes.size(); ++i) {
        grouped_hitbox_ref boxptr{*group, i};
        box_pos(normalize(*boxptr)).erase(boxptr);
    }
    box_data.erase(group);
}

void grid_loose_quadtree::move_box(handle_type group, const basics::vec2& v) {
    for (size_t i = 0; i < group->boxes.size(); ++i) {
        grouped_hitbox_ref boxptr{*group, i};
        auto box = *boxptr;
        auto& old_set = box_pos(normalize(box));
        auto& new_set = box_pos(normalize(box.offset(v)));
        if (&old_set != &new_set) {
            old_set.erase(boxptr);
            new_set.emplace(boxptr);
        }
    }
    group->offset += v;
}

#define CHECK_COLLISION_AT_TREE(idx) \
    for (auto&& may_coll : vtree[idx]) { \
        if (boxptr != may_coll && basics::aabb::have_overlap(box, *may_coll)) { \
            co_yield may_coll; \
        } \
    }

auto grid_loose_quadtree::may_collide(const grouped_hitbox_ref& boxptr) -> coutils::generator<grouped_hitbox_ref> {
    auto box = *boxptr;
    auto norm_box = normalize(box);
    auto depth = std::min(box_depth(norm_box), max_depth);
    ssize_t ix, iy;
    if (depth == 0) {
        CHECK_COLLISION_AT_TREE(0)
        co_return;
    } else if (depth == 1) {
        for (size_t i = 0; i < 5; ++i) {
            CHECK_COLLISION_AT_TREE(i)
        }
        co_return;
    } else {
        double bcx = (norm_box.left + norm_box.right) / 2;
        double bcy = (norm_box.top + norm_box.bottom) / 2;
        ix = pos_idx(bcx, depth); iy = pos_idx(bcy, depth);
    }

    static constexpr std::pair<ssize_t, ssize_t> search_offsets[] = {
        { 0,  0},
        {-1,  0},
        {+1,  0},
        { 0, -1},
        { 0, +1},
        {-1, -1},
        {+1, -1},
        {-1, +1},
        {+1, +1},
    };

    // 比较同层网格的周围9个及其对应上层
    std::unordered_set<size_t> visited;
    ssize_t max_idx = (ssize_t(1) << depth) - 1;
    for (auto&& [dix, diy] : search_offsets) {
        auto rix = ix + dix, riy = iy + diy;
        if (rix < 0 || rix > max_idx || riy < 0 || riy > max_idx) { continue; }
        for (unsigned char i = 0; i < depth + 1; ++i) {
            size_t idx = combine_idx(rix >> i, riy >> i, depth - i);
            bool unseen = visited.emplace(idx).second;
            if (!unseen) { continue; }
            CHECK_COLLISION_AT_TREE(idx)
        }
    }
}
