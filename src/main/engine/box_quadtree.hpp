#pragma once
#ifndef __NAIVE_QUADTREE__
#define __NAIVE_QUADTREE__

#include <vector>
#include <unordered_set>
#include <list>
#include <span>
#include <coutils.hpp>
#include <silly_framework/rewheel/hash_utils.hpp>
#include "./basics.hpp"

namespace naive_engine {

namespace sf = silly_framework;

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
public:
    struct hitbox {
        void* parent;
        basics::aabb box;
        basics::vec2 offset;
        auto abs_box() { return box.offset(offset); }
    };

    using hitbox_list = std::list<hitbox>;
    using handle_type = hitbox_list::iterator;
    using hitbox_set = sf::utils::listit_uset<handle_type>;

protected:
    template <typename T>
    using vtree_type = std::vector<T>;

    unsigned char max_depth;
    hitbox_list box_data;
    vtree_type<hitbox_set> vtree;
    double scene_width, scene_height;

    basics::aabb normalize(const basics::aabb& box);
    hitbox_set& box_pos(const basics::aabb& box);

public:
    grid_loose_quadtree(unsigned char depth, double width, double height);
    ~grid_loose_quadtree();

    // 获取场景大小 
    std::tuple<double, double> scene_size() const { return {scene_width, scene_height}; }
    // 重设场景大小，会导致重新建树
    void scene_size(double width, double height) {
        scene_width = width; scene_height = height;
        retree_boxes();
    }
    // 清除树内容
    void reset();
    // 清除树内容并重设场景大小
    void reset(double width, double height) {
        reset(); scene_width = width; scene_height = height;
    }

    // 添加碰撞箱
    handle_type add_box(void* parent, const basics::aabb& box, const basics::vec2& offset);
    // 删除碰撞箱
    void del_box(handle_type hbox);
    // 移动碰撞箱
    void move_box(handle_type hbox, const basics::vec2& v);
    // 重新建树
    void retree_boxes();
    // 枚举相交的aabb
    auto may_collide(handle_type hbox) -> coutils::generator<handle_type>;
};

} // namespace naive_engine

#endif // __NAIVE_QUADTREE__
