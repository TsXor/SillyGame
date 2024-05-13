#include "box_quadtree.hpp"

using bitvec = std::vector<bool>;

using namespace naive_engine;

static inline size_t layer_start(uint8_t n) {
    return ((size_t(1) << (2 * n)) - 1) / 3; // (4^n - 1) / 3 
}

static inline uint8_t size_depth(double l) {
    double ls = 1.0; char depth = 0;
    do { ls /= 2.0; ++depth; } while (ls > l);
    return depth - 1;
}

static inline uint8_t box_depth(const basics::aabb& box) {
    double width = box.right - box.left;
    double height = box.bottom - box.top;
    return std::min(size_depth(width), size_depth(height));
}

static inline ssize_t pos_idx(double pos, uint8_t depth) {
    if (depth == 0) { return 0; }
    return pos * static_cast<double>(size_t(1) << depth);
}

static inline size_t combine_idx(size_t x, size_t y, uint8_t depth) {
    return layer_start(depth) + ((y << depth) | x);
}

grid_loose_quadtree::grid_loose_quadtree(uint8_t depth, double width, double height):
max_depth(depth), scene_width(width), scene_height(height) {}

grid_loose_quadtree::~grid_loose_quadtree() {}

basics::aabb grid_loose_quadtree::normalize(const basics::aabb& box) {
    return { box.left / scene_width, box.right / scene_width, box.top / scene_height, box.bottom / scene_height };
}

size_t grid_loose_quadtree::max_idx() { return layer_start(max_depth); }

auto grid_loose_quadtree::box_pos(const basics::aabb& box) -> std::tuple<size_t, size_t, uint8_t> {
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
    if (it == decltype(it)() || it == vtree.right.end()) { return; }
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

auto grid_loose_quadtree::upsearch(hitbox* hbox) -> coutils::generator<hitbox*> {
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

    static constexpr std::pair<ssize_t, ssize_t> search_offsets[] = {
        {-1, -1}, { 0, -1}, {+1, -1},
        {-1,  0}, { 0,  0}, {+1,  0},
        {-1, +1}, { 0, +1}, {+1, +1},
    };
    static constexpr std::pair<ssize_t, ssize_t> search_offsets_half[] = {
        {-1, -1}, { 0, -1}, {+1, -1},
        {-1,  0},
    };

    // 比较本层网格的左上方4个
    for (auto&& [dix, diy] : search_offsets_half) {
        ssize_t rix = ix + dix, riy = iy + diy;
        ssize_t max_idx = (ssize_t(1) << depth) - 1;
        if (rix < 0 || rix > max_idx || riy < 0 || riy > max_idx) { continue; }
        size_t idx = combine_idx(rix, riy, depth);
        for (auto&& res : check_at_idx(idx)) { co_yield res; }
    }

    // 比较上层对应网格的周围9个
    for (uint8_t i = 1; i <= depth; ++i) {
        uint8_t rd = depth - i;
        for (auto&& [dix, diy] : search_offsets) {
            ssize_t rix = (ix >> i) + dix, riy = (iy >> i) + diy;
            ssize_t max_idx = (ssize_t(1) << rd) - 1;
            if (rix < 0 || rix > max_idx || riy < 0 || riy > max_idx) { continue; }
            size_t idx = combine_idx(rix, riy, rd);
            for (auto&& res : check_at_idx(idx)) { co_yield res; }
        }
    }
}

bool box_have_overlap(grid_loose_quadtree::hitbox* a, grid_loose_quadtree::hitbox* b) {
    return basics::aabb::have_overlap(a->abs_box(), b->abs_box());
}

auto grid_loose_quadtree::colldet() -> coutils::generator<std::pair<hitbox*, hitbox*>> {
    for (auto rgl = vtree.left.begin(); rgl != vtree.left.end();) {
        auto rgr = rgl;
        do { ++rgr; } while (rgr != vtree.left.end() && rgr->first == rgl->first);
        for (auto it_a = rgl; it_a != rgr; ++it_a) {
            auto& box_a = it_a->second;
            // 向上搜索
            for (auto&& res : upsearch(box_a)) {
                co_yield coutils::inituple(box_a, res);
            }
            // 同网格搜索
            for (auto it_b = it_a; it_b != rgr; ++it_b) {
                auto& box_b = it_b->second;
                if (box_have_overlap(box_a, box_b)) {
                    co_yield coutils::inituple(box_a, box_b);
                }
            }
        }
        rgl = rgr;
    }
}
