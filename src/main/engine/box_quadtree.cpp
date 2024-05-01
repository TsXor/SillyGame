#include "box_quadtree.hpp"

using bitvec = std::vector<bool>;

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

auto grid_loose_quadtree::add_box(void* parent, const basics::aabb& box, const basics::vec2& offset) -> handle_type {
    auto hbox = box_data.emplace(box_data.end(), parent, box, offset);
    box_pos(normalize(hbox->abs_box())).emplace(hbox);
    return hbox;
}

void grid_loose_quadtree::del_box(handle_type hbox) {
    box_pos(normalize(hbox->abs_box())).erase(hbox);
    box_data.erase(hbox);
}

void grid_loose_quadtree::move_box(handle_type hbox, const basics::vec2& v) {
    auto abs_box = hbox->abs_box();
    auto& old_set = box_pos(normalize(abs_box));
    auto& new_set = box_pos(normalize(abs_box.offset(v)));
    if (&old_set != &new_set) {
        old_set.erase(hbox);
        new_set.emplace(hbox);
    }
    hbox->offset += v;
}

void grid_loose_quadtree::retree_boxes() {
    for (auto&& bset : vtree) { bset.clear(); }
    for (auto hbox = box_data.begin(); hbox != box_data.end(); ++hbox) {
        box_pos(normalize(hbox->box)).emplace(hbox);
    }
}

void grid_loose_quadtree::reset() {
    for (auto&& bset : vtree) { bset.clear(); }
    box_data.clear();
}

#define CHECK_COLLISION_AT_TREE(idx) \
    for (auto&& may_coll : vtree[idx]) { \
        if (hbox != may_coll && abs_box.have_overlap(may_coll->abs_box())) { \
            co_yield may_coll; \
        } \
    }

auto grid_loose_quadtree::may_collide(handle_type hbox) -> coutils::generator<handle_type> {
    auto abs_box = hbox->abs_box();
    auto norm_box = normalize(abs_box);
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
        {-1, -1}, { 0, -1}, {+1, -1},
        {-1,  0}, { 0,  0}, {+1,  0},
        {-1, +1}, { 0, +1}, {+1, +1},
    };

    // 比较同层网格的周围9个及其对应上层
    bitvec visited(vtree.size(), false);
    ssize_t max_idx = (ssize_t(1) << depth) - 1;
    for (auto&& [dix, diy] : search_offsets) {
        auto rix = ix + dix, riy = iy + diy;
        if (rix < 0 || rix > max_idx || riy < 0 || riy > max_idx) { continue; }
        for (unsigned char i = 0; i < depth + 1; ++i) {
            size_t idx = combine_idx(rix >> i, riy >> i, depth - i);
            if (visited[idx]) { continue; }
            visited[idx] = true;
            CHECK_COLLISION_AT_TREE(idx)
        }
    }
}
