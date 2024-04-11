#include "./basics.hpp"
#include <cfloat>


using namespace naive_engine::basics;

static inline vec2 poly_shadow_range(const polygon_const_ref& poly, const vec2& v, double vmag) {
    size_t poly_nv = poly.vertices.size();
    double cur_pos = 0, pmin = 0, pmax = 0;
    for (size_t i = 0; i < poly_nv; ++i) {
        auto cur_edge = poly.vertices[(i + 1) % poly_nv] - poly.vertices[i];
        cur_pos += cur_edge * v / vmag;
        pmin = std::min(pmin, cur_pos); pmax = std::max(pmax, cur_pos);
    }
    return {pmin, pmax};
}

static inline vec2 rect_shadow_range(const polygon_const_ref& poly, const vec2& v, double vmag) {
    double hori = vec2(poly.box.width(), 0) * v / vmag;
    double vert = vec2(0, poly.box.height()) * v / vmag;
    double pos[] = {0, hori, vert, hori + vert};
    auto&& [pmin, pmax] = std::minmax_element(pos, pos + 4);
    return {*pmin, *pmax};
}

static inline vec2 polygon_colldet_impl_rect_rect(const polygon_const_ref& poly1, const polygon_const_ref& poly2, const vec2& offset21) {
    // 都是矩形
    vec2 poly_offset = vec2(poly2.box.left, poly2.box.top) - vec2(poly1.box.left, poly1.box.top) + offset21;

    double left_dist = poly1.box.right - (poly2.box.left + poly_offset.x);
    double right_dist = (poly2.box.right + poly_offset.x) - poly1.box.left;
    if (left_dist < 0.0 || right_dist < 0.0) { return {0, 0}; }
    double hori_mtv_length; vec2 hori_direction;
    if (left_dist < right_dist) {
        hori_direction = {-1, 0};
        hori_mtv_length = left_dist;
    } else {
        hori_direction = {1, 0};
        hori_mtv_length = right_dist;
    }

    double top_dist = poly1.box.bottom - (poly2.box.top + poly_offset.y);
    double bottom_dist = (poly2.box.bottom + poly_offset.y) - poly1.box.top;
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

static inline vec2 polygon_colldet_impl_rect_poly(const polygon_const_ref& poly1, const polygon_const_ref& poly2, const vec2& offset21) {
    // 一个矩形，一个多边形
    vec2 mtv_direction; double mtv_length = DBL_MAX;
    vec2 poly_offset = poly2.vertices[0] - vec2(poly1.box.left, poly1.box.top) + offset21;
    
    auto check_poly = [&](const vec2& v) -> bool {
        double vmag = v.mag();
        auto p_off = poly_offset * v / vmag;
        auto [p1_min, p1_max] = rect_shadow_range(poly1, v, vmag);
        auto [p2_min, p2_max] = poly_shadow_range(poly2, v, vmag);
        p2_min += p_off; p2_max += p_off;
        double left = p1_max - p2_min;
        double right = p2_max - p1_min;
        if (left < 0.0 || right < 0.0) { return true; }
        if (left < right) {
            if (left < mtv_length) {
                mtv_direction = -v / vmag;
                mtv_length = left;
            }
        } else {
            if (right < mtv_length) {
                mtv_direction = v / vmag;
                mtv_length = right;
            }
        }
        return false;
    };

    auto rect_mtv = polygon_colldet_impl_rect_rect(poly1, poly2, offset21);
    size_t poly_nv = poly2.vertices.size();
    for (size_t i = 0; i < poly_nv; ++i) {
        size_t j = (i + 1) % poly_nv;
        vec2 cur_direction = (poly2.vertices[j] - poly2.vertices[i]).normal();
        if (check_poly(cur_direction)) { return {0, 0}; }
    }
    if (mtv_length < rect_mtv.mag()) {
        return mtv_direction * mtv_length;
    } else {
        return rect_mtv;
    }
}

static inline vec2 polygon_colldet_impl_poly_poly(const polygon_const_ref& poly1, const polygon_const_ref& poly2, const vec2& offset21) {
    // 都是多边形
    vec2 mtv_direction; double mtv_length = DBL_MAX;
    vec2 poly_offset = poly2.vertices[0] - poly1.vertices[0] + offset21;
    
    auto check_polys = [&](const vec2& v) -> bool {
        double vmag = v.mag();
        auto p_off = poly_offset * v / vmag;
        auto [p1_min, p1_max] = poly_shadow_range(poly1, v, vmag);
        auto [p2_min, p2_max] = poly_shadow_range(poly2, v, vmag);
        p2_min += p_off; p2_max += p_off;
        double left = p1_max - p2_min;
        double right = p2_max - p1_min;
        if (left < 0.0 || right < 0.0) { return true; }
        if (left < right) {
            if (left < mtv_length) {
                mtv_direction = -v / vmag;
                mtv_length = left;
            }
        } else {
            if (right < mtv_length) {
                mtv_direction = v / vmag;
                mtv_length = right;
            }
        }
        return false;
    };

    size_t poly1_nv = poly1.vertices.size();
    for (size_t i = 0; i < poly1_nv; ++i) {
        size_t j = (i + 1) % poly1_nv;
        vec2 cur_direction = (poly1.vertices[j] - poly1.vertices[i]).normal();
        if (check_polys(cur_direction)) { return {0, 0}; }
    }
    size_t poly2_nv = poly2.vertices.size();
    for (size_t i = 0; i < poly2_nv; ++i) {
        size_t j = (i + 1) % poly2_nv;
        vec2 cur_direction = (poly2.vertices[j] - poly2.vertices[i]).normal();
        if (check_polys(cur_direction)) { return {0, 0}; }
    }
    return mtv_direction * mtv_length;
}

vec2 naive_engine::basics::polygon_colldet(const polygon_const_ref& poly1, const polygon_const_ref& poly2, const vec2& offset21) {
    bool isrect1 = poly1.vertices.empty(); bool isrect2 = poly2.vertices.empty();
    if (isrect1 && isrect2) {
        return polygon_colldet_impl_rect_rect(poly1, poly2, offset21);
    } else if (!isrect1 && !isrect2) {
        return polygon_colldet_impl_poly_poly(poly1, poly2, offset21);
    } else {
        if (isrect1) {
            return polygon_colldet_impl_rect_poly(poly1, poly2, offset21);
        } else {
            return -polygon_colldet_impl_rect_poly(poly2, poly1, -offset21);
        }
    }
}
