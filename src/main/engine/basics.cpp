#include "./basics.hpp"
#include <cfloat>


using namespace naive_engine::basics;

static inline vec2 poly_shadow_range(const poly_verts& poly, const vec2& dv) {
    size_t poly_nv = poly.size();
    double cur_pos = 0, pmin = 0, pmax = 0;
    for (size_t i = 0; i < poly_nv; ++i) {
        auto cur_edge = poly[(i + 1) % poly_nv] - poly[i];
        cur_pos += cur_edge * dv;
        pmin = std::min(pmin, cur_pos); pmax = std::max(pmax, cur_pos);
    }
    return {pmin, pmax};
}

static inline vec2 polygon_colldet_impl(const aabb& poly1, const aabb& poly2, const vec2& offset21) {
    // 都是矩形
    vec2 poly_offset = offset21;

    double left_dist = poly1.right - (poly2.left + poly_offset.x);
    double right_dist = (poly2.right + poly_offset.x) - poly1.left;
    if (left_dist < 0.0 || right_dist < 0.0) { return {0, 0}; }
    double hori_mtv_length; vec2 hori_direction;
    if (left_dist < right_dist) {
        hori_direction = {-1, 0};
        hori_mtv_length = left_dist;
    } else {
        hori_direction = {1, 0};
        hori_mtv_length = right_dist;
    }

    double top_dist = poly1.bottom - (poly2.top + poly_offset.y);
    double bottom_dist = (poly2.bottom + poly_offset.y) - poly1.top;
    if (top_dist < 0.0 || bottom_dist < 0.0) { return {0, 0}; }
    double vert_mtv_length; vec2 vert_direction;
    if (top_dist < bottom_dist) {
        vert_direction = {0, -1};
        vert_mtv_length = top_dist;
    } else {
        vert_direction = {0, 1};
        vert_mtv_length = bottom_dist;
    }

    if (hori_mtv_length < vert_mtv_length) {
        return hori_direction * hori_mtv_length;
    } else {
        return vert_direction * vert_mtv_length;
    }
}

static inline vec2 polygon_colldet_impl(const poly_verts& poly1, const poly_verts& poly2, const vec2& offset21) {
    // 都是多边形
    vec2 mtv_direction; double mtv_length = DBL_MAX;
    vec2 poly_offset = poly2[0] - poly1[0] + offset21;
    
    auto check_polys = [&](const vec2& dv) -> bool {
        auto p_off = poly_offset * dv;
        auto [p1_min, p1_max] = poly_shadow_range(poly1, dv);
        auto [p2_min, p2_max] = poly_shadow_range(poly2, dv);
        p2_min += p_off; p2_max += p_off;
        double left = p1_max - p2_min;
        double right = p2_max - p1_min;
        if (left < 0.0 || right < 0.0) { return true; }
        if (left < right) {
            if (left < mtv_length) {
                mtv_direction = -dv;
                mtv_length = left;
            }
        } else {
            if (right < mtv_length) {
                mtv_direction = dv;
                mtv_length = right;
            }
        }
        return false;
    };

    size_t poly1_nv = poly1.size();
    for (size_t i = 0; i < poly1_nv; ++i) {
        size_t j = (i + 1) % poly1_nv;
        vec2 cur_direction = (poly1[j] - poly1[i]).normal().unit();
        if (check_polys(cur_direction)) { return {0, 0}; }
    }
    size_t poly2_nv = poly2.size();
    for (size_t i = 0; i < poly2_nv; ++i) {
        size_t j = (i + 1) % poly2_nv;
        vec2 cur_direction = (poly2[j] - poly2[i]).normal().unit();
        if (check_polys(cur_direction)) { return {0, 0}; }
    }
    return mtv_direction * mtv_length;
}

static inline poly_verts rect2poly(const aabb& rect) {
    return {{rect.left, rect.top}, {rect.right, rect.top}, {rect.right, rect.bottom}, {rect.left, rect.bottom}};
}

static inline vec2 polygon_colldet_impl(const aabb& rect1, const poly_verts& poly2, const vec2& offset21) {
    return polygon_colldet_impl(rect2poly(rect1), poly2, offset21);
}

static inline vec2 polygon_colldet_impl(const poly_verts& poly1, const aabb& rect2, const vec2& offset21) {
    return polygon_colldet_impl(poly1, rect2poly(rect2), offset21);
}

vec2 naive_engine::basics::polygon_colldet(const convex_variant& con1, const convex_variant& con2, const vec2& offset21) {
    return std::visit([&offset21](auto&& sh1, auto&& sh2) { return polygon_colldet_impl(sh1, sh2, offset21); }, con1, con2);
}
