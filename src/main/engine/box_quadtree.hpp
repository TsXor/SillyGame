#pragma once
#ifndef __NAIVE_QUADTREE__
#define __NAIVE_QUADTREE__

#include <coutils.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include "./basics.hpp"

namespace naive_engine {

using boost::bimap;
using boost::bimaps::unordered_set_of;
using boost::bimaps::unordered_multiset_of;

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
        basics::aabb box;
        basics::vec2 offset = {0, 0};
        auto abs_box() { return box.offset(offset); }
    };

protected:
    uint8_t max_depth;
    bimap<unordered_multiset_of<size_t>, unordered_set_of<hitbox*>> vtree;
    double scene_width, scene_height;

    basics::aabb normalize(const basics::aabb& box);
    auto box_pos(const basics::aabb& box) -> std::tuple<size_t, size_t, uint8_t>;
    size_t box_idx(const basics::aabb& box);
    size_t max_idx();

public:
    grid_loose_quadtree(uint8_t depth, double width, double height);
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
    void add_box(hitbox* hbox);
    // 删除碰撞箱
    void del_box(hitbox* hbox);
    // 移动碰撞箱
    void move_box(hitbox* hbox, const basics::vec2& v);
    // 重新建树
    void retree_boxes();
    // 枚举相交的aabb
    auto upsearch(hitbox* hbox) -> coutils::generator<hitbox*>;
    auto colldet() -> coutils::generator<std::pair<hitbox*, hitbox*>>;
};

} // namespace naive_engine

#endif // __NAIVE_QUADTREE__
