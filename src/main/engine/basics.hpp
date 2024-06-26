#pragma once
#ifndef __ENGINE_BASICS__
#define __ENGINE_BASICS__

#include <algorithm>
#include <span>
#include <cmath>
#include <cfloat>
#include <vector>
#include <variant>

namespace naive_engine::basics {

static inline bool dbl_nonzero(double x) { return std::fabs(x) > DBL_EPSILON; }
static inline bool dbl_equal(double a, double b) { return !dbl_nonzero(a - b); }

struct vec2 {
    double x, y;
    
    double mag() const { return std::hypot(x, y); } // 模长
    vec2 normal() const { return {-y, x}; } // 法向量
    inline vec2 unit() const;
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
vec2 vec2::unit() const { return (*this) / mag(); }

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
    vec2 center() { return {(left + right) / 2, (top + bottom) / 2}; }
};

static inline aabb operator+(const aabb& box, const vec2& off) {
    return {box.left + off.x, box.right + off.x, box.top + off.y, box.bottom + off.y};
}
static inline aabb operator-(const aabb& box, const vec2& off) {
    return {box.left - off.x, box.right - off.x, box.top - off.y, box.bottom - off.y};
}

using poly_verts = std::vector<basics::vec2>;
using poly_verts_view = std::span<vec2>;
using poly_verts_const_view = std::span<const vec2>;
using convex_variant = std::variant<basics::aabb, basics::poly_verts>;

static inline auto make_convex_variant(const aabb& rect) {
    return convex_variant(std::in_place_type<basics::aabb>, rect);
}
static inline auto make_convex_variant(const poly_verts_const_view& poly) {
    return convex_variant(std::in_place_type<basics::poly_verts>, poly.begin(), poly.end());
}
static inline auto make_convex_variant(poly_verts&& poly) {
    return convex_variant(std::in_place_type<basics::poly_verts>, std::move(poly));
}

static inline aabb coarse(const convex_variant& convex) {
    if (std::holds_alternative<aabb>(convex)) {
        return std::get<aabb>(convex);
    } else if (std::holds_alternative<poly_verts>(convex)) {
        aabb box{DBL_MAX, DBL_MIN, DBL_MAX, DBL_MIN};
        for (auto&& vert : std::get<poly_verts>(convex)) {
            box.left = std::min(box.left, vert.x);
            box.right = std::max(box.right, vert.x);
            box.top = std::min(box.top, vert.x);
            box.bottom = std::max(box.bottom, vert.x);
        }
        return box;
    } else {
        return {};
    }
}

// 检查多边形碰撞并返回1离开2所需的MTV（Minimum Translation Vector，最小平移向量）
vec2 polygon_colldet(const convex_variant& poly1, const convex_variant& poly2, const vec2& offset21);

} // namespace naive_engine::basics

#endif // __ENGINE_BASICS__
