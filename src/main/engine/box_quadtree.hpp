#pragma once
#ifndef __NAIVE_QUADTREE__
#define __NAIVE_QUADTREE__

#include <vector>
#include <unordered_set>
#include <list>
#include <span>
#include <cppcoro/generator.hpp>
#include "./basics.hpp"

namespace naive_engine {

struct hitbox_group {
    void* parent;
    basics::vec2 offset;
    std::span<const basics::aabb> boxes;
};

struct grouped_hitbox_ref {
    hitbox_group& parent;
    size_t idx;
    basics::aabb operator*() const { return parent.boxes[idx].offset(parent.offset); }
};

static inline bool operator==(const grouped_hitbox_ref& r1, const grouped_hitbox_ref& r2) {
    return (&r1.parent == &r2.parent) && (r1.idx == r2.idx);
}

} // namespace naive_engine

template<> struct std::hash<naive_engine::grouped_hitbox_ref> {
    std::size_t operator()(const naive_engine::grouped_hitbox_ref& ref) const noexcept{
        std::size_t h1 = std::hash<void*>{}(&ref.parent);
        std::size_t h2 = std::hash<size_t>{}(ref.idx);
        return h1 ^ (h2 << 1);
    }
};

namespace naive_engine {

/**
 *  一秒钟寄个弹词：
 *  grid 网格
 *  loose 宽松的
 *  quad- 四
 *  tree 树
 *  grid loose quadtree 网格宽松四叉树
 *  你寄住了吗
 *  我不管你寄没寄，我是寄了反正
 */

/**
 * 网格宽松四叉树的简单实现，用于记录动态物体
 */
class grid_loose_quadtree {
protected:
    template <typename T>
    using vtree_type = std::vector<T>;
    using hitbox_set = std::unordered_set<grouped_hitbox_ref>;

    unsigned char max_depth;
    vtree_type<hitbox_set> vtree;
    std::list<hitbox_group> box_data;
    double scene_width, scene_height;

    basics::aabb normalize(const basics::aabb& box);
    hitbox_set& box_pos(const basics::aabb& box);

public:
    using handle_type = decltype(box_data)::iterator;

    grid_loose_quadtree(unsigned char depth, double width, double height);
    ~grid_loose_quadtree();

    std::tuple<double, double> scene_size() const { return {scene_width, scene_height}; }

    // 添加碰撞箱
    handle_type add_box(void* parent, const basics::vec2& offset, const std::span<const basics::aabb>& boxes);
    // 删除碰撞箱
    void del_box(handle_type group);
    // 移动碰撞箱
    void move_box(handle_type group, const basics::vec2& v);
    // 枚举相交的aabb
    auto may_collide(const grouped_hitbox_ref& boxptr) -> cppcoro::generator<const grouped_hitbox_ref&>;
};

} // namespace naive_engine

#endif // __NAIVE_QUADTREE__
