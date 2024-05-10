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

static inline size_t combine_idx(size_t x, size_t y, unsigned char depth) {
    return layer_start(depth) + ((y << depth) | x);
}

grid_loose_quadtree::grid_loose_quadtree(unsigned char depth, double width, double height):
max_depth(depth), scene_width(width), scene_height(height) {}

grid_loose_quadtree::~grid_loose_quadtree() {}

basics::aabb grid_loose_quadtree::normalize(const basics::aabb& box) {
    return { box.left / scene_width, box.right / scene_width, box.top / scene_height, box.bottom / scene_height };
}

size_t grid_loose_quadtree::max_idx() { return layer_start(max_depth); }

auto grid_loose_quadtree::box_pos(const basics::aabb& box) -> std::tuple<size_t, size_t, unsigned char> {
    auto nbox = normalize(box);
    auto depth = std::min(box_depth(nbox), max_depth);
    double bcx = (nbox.left + nbox.right) / 2;
    double bcy = (nbox.top + nbox.bottom) / 2;
    ssize_t ix = pos_idx(bcx, depth), iy = pos_idx(bcy, depth);
    ssize_t max_idx = (ssize_t(1) << depth) - 1;
    auto clamp = [&](ssize_t v) -> size_t {
        if (v < 0) return 0;
        if (v > max_idx) return max_idx;
        return v;
    };
    return { clamp(ix), clamp(iy), depth };
}

size_t grid_loose_quadtree::box_idx(const basics::aabb& box) {
    auto [x, y, d] = box_pos(box);
    return combine_idx(x, y, d);
}

void grid_loose_quadtree::add_box(hitbox* hbox) {
    vtree.insert({box_idx(hbox->abs_box()), hbox});
}

void grid_loose_quadtree::del_box(hitbox* hbox) {
    vtree.right.erase(hbox);
}

void grid_loose_quadtree::move_box(hitbox* hbox, const basics::vec2& v) {
    hbox->offset += v;
    auto new_idx = box_idx(hbox->abs_box());
    auto it = vtree.right.find(hbox);
    if (it->second != new_idx) { vtree.right.replace_data(it, new_idx); }
}

void grid_loose_quadtree::reset() { vtree.clear(); }

void grid_loose_quadtree::retree_boxes() {
    std::vector<hitbox*> ptrs;
    ptrs.reserve(vtree.size());
    for (auto&& [_, ptr] : vtree) { ptrs.push_back(ptr); }
    vtree.clear();
    for (auto&& ptr : ptrs) { add_box(ptr); }
}

auto grid_loose_quadtree::may_collide(hitbox* hbox) -> coutils::generator<hitbox*> {
    auto abs_box = hbox->abs_box();

    auto check_at_idx = [&](size_t idx) -> coutils::generator<hitbox*> {
        for (auto may_coll_it = vtree.left.find(idx); may_coll_it != vtree.left.end() && may_coll_it->first == idx; ++may_coll_it) {
            auto& may_coll = may_coll_it->second;
            if (hbox != may_coll && abs_box.have_overlap(may_coll->abs_box())) {
                co_yield may_coll;
            }
        }
    };

    auto [ix, iy, depth] = box_pos(abs_box);

    if (depth <= 1) {
        auto depth_max_idx = layer_start(depth + 1);
        for (size_t i = 0; i < depth_max_idx; ++i) {
            for (auto&& res : check_at_idx(i)) { co_yield res; }
        }
        co_return;
    }

    static constexpr std::pair<ssize_t, ssize_t> search_offsets[] = {
        {-1, -1}, { 0, -1}, {+1, -1},
        {-1,  0}, { 0,  0}, {+1,  0},
        {-1, +1}, { 0, +1}, {+1, +1},
    };

    // 比较同层网格的周围9个及其对应上层
    bitvec visited(max_idx(), false);
    ssize_t max_idx = (ssize_t(1) << depth) - 1;
    for (auto&& [dix, diy] : search_offsets) {
        auto rix = ix + dix, riy = iy + diy;
        if (rix < 0 || rix > max_idx || riy < 0 || riy > max_idx) { continue; }
        for (unsigned char i = 0; i < depth + 1; ++i) {
            size_t idx = combine_idx(rix >> i, riy >> i, depth - i);
            if (visited[idx]) { continue; }
            visited[idx] = true;
            for (auto&& res : check_at_idx(idx)) { co_yield res; }
        }
    }
}
