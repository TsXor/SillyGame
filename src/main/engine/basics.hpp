#pragma once
#ifndef __ENGINE_BASICS__
#define __ENGINE_BASICS__

#include <algorithm>
#include <span>
#include <cmath>
#include <cfloat>
#include <vector>

namespace naive_engine::basics {

static inline bool dbl_nonzero(double x) { return std::fabs(x) > DBL_EPSILON; }
static inline bool dbl_equal(double a, double b) { return !dbl_nonzero(a - b); }

struct vec2 {
    double x, y;
    
    double mag() const { return std::hypot(x, y); } // 模长
    vec2 normal() const { return {-y, x}; } // 法向量
    bool nonzero() const { return dbl_nonzero(x) || dbl_nonzero(y); }
    vec2& operator+=(const vec2& other) {
        x += other.x; y += other.y;
        return *this;
    }
    vec2& operator-=(const vec2& other) {
        x -= other.x; y -= other.y;
        return *this;
    }
    vec2 operator-() const { return {-x, -y}; }
};
static inline vec2 operator+(const vec2& a, const vec2& b) {
    return vec2{a.x + b.x, a.y + b.y};
}
static inline vec2 operator-(const vec2& a, const vec2& b) {
    return vec2{a.x - b.x, a.y - b.y};
}
static inline vec2 operator*(const vec2& v, double k) {
    return vec2{v.x * k, v.y * k};
}
static inline vec2 operator/(const vec2& v, double k) {
    return vec2{v.x / k, v.y / k};
}
static inline double operator*(const vec2& a, const vec2& b) {
    return a.x * b.x + a.y * b.y;
}

struct aabb {
    double left, right, top, bottom;
    
    double width() const { return right - left; }
    double height() const { return bottom - top; }
    aabb offset(vec2 v) const { return {left + v.x, right + v.x, top + v.y, bottom + v.y}; }
    static bool have_overlap(const aabb& box1, const aabb& box2) {
        return std::min(box1.right, box2.right) > std::max(box1.left, box2.left)
            && std::min(box1.bottom, box2.bottom) > std::max(box1.top, box2.top);
    }
    bool have_overlap(const aabb& other) const { return have_overlap(*this, other); }
};

using poly_verts = std::vector<basics::vec2>;
using poly_verts_view = std::span<const vec2>;

struct polygon_ref {
    const aabb& box;
    const poly_verts_view vertices;
};

// 检查多边形碰撞并返回1离开2所需的MTV（Minimum Translation Vector，最小平移向量）
vec2 polygon_colldet(const polygon_ref& poly1, const polygon_ref& poly2, const vec2& offset21);

} // namespace naive_engine::basics

#endif // __ENGINE_BASICS__
