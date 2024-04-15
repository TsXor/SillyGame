#include "./simulator.hpp"

using namespace naive_engine;

simulator::simulator(double width, double height) : boxtree(6, width, height) {}
simulator::~simulator() {
    for (auto&& entity : entities) {
        boxtree.del_box(entity.hbox);
    }
}

simulator::handle_type simulator::add_entity(const entity::static_data& data, const basics::vec2& center) {
    basics::vec2 center_offset = center - basics::vec2(data.width / 2, data.height / 2);
    auto handle = entities.emplace(entities.end(), data);
    static_assert(sizeof(handle) == sizeof(void*));
    handle->hbox = boxtree.add_box(std::bit_cast<void*>(handle), center_offset, data.boxes);
    return handle;
}
void simulator::del_entity(handle_type entity) {
    boxtree.del_box(entity->hbox);
    entities.erase(entity);
}

void simulator::teleport_entity_offset(handle_type entity, basics::vec2 offset) {
    boxtree.move_box(entity->hbox, offset);
}
void simulator::teleport_entity_center(handle_type entity, basics::vec2 center) {
    basics::vec2 center_offset = center - basics::vec2(entity->data->width / 2, entity->data->height / 2);
    boxtree.move_box(entity->hbox, center_offset - entity->hbox->offset);
}

static inline auto handle_from_boxref(const naive_engine::grouped_hitbox_ref& boxref) {
    return std::bit_cast<simulator::handle_type>(boxref.parent.parent);
}

auto simulator::colldet(handle_type entity) -> coutils::generator<std::pair<handle_type, basics::vec2>> {
    std::unordered_set<handle_type::pointer> found_coll;
    for (size_t i = 0; i < entity->hbox->boxes.size(); ++i) {
        grouped_hitbox_ref boxptr{*entity->hbox, i};
        // 检查边缘碰撞
        auto box = *boxptr;
        auto [scw, sch] = boxtree.scene_size();
        if (box.width() >= scw || box.height() >= sch) { continue; }
        basics::vec2 border_mtv = {0, 0};
        if (box.left < 0) { border_mtv.x = -box.left; }
        if (box.right > scw) { border_mtv.x = scw - box.right; }
        if (box.top < 0) { border_mtv.y = -box.top; }
        if (box.bottom > sch) { border_mtv.y = sch - box.bottom; }
        if (border_mtv.nonzero()) {
            co_yield coutils::inituple(handle_type(), border_mtv);
        }
        // 检查真实碰撞
        for (auto&& possible : boxtree.may_collide(boxptr)) {
            auto other = handle_from_boxref(possible);
            if (found_coll.contains(&*other)) { continue; }
            auto mtv = basics::polygon_colldet(
                entity->poly_at(i), other->poly_at(possible.idx),
                other->hbox->offset - entity->hbox->offset
            );
            if (mtv.nonzero()) {
                found_coll.emplace(&*other);
                co_yield coutils::inituple(other, mtv);
            }
        }
    }
}

void simulator::tick(double dt) {
    for (auto&& entity : entities) {
        boxtree.move_box(entity.hbox, entity.velocity * dt);
    }
}
void simulator::render(sf::game_window& wnd, basics::vec2 pos) {
    if (current_map) {
        auto&& [map, scalev] = *current_map;
        map.render(wnd, pos.x / scalev, pos.y / scalev, scalev);
    }
    auto&& [vs_w, vs_h] = wnd.renman.vs_size();
    auto center_offset = basics::vec2(vs_w / 2, vs_h / 2);
    if (render_sorter) { entities.sort(render_sorter); }
    for (auto&& entity : entities) {
        auto center_relpos = entity.hbox->offset - pos + center_offset;
        sf::glut::position render_pos;
        render_pos.left = (int)center_relpos.x;
        render_pos.right = render_pos.left + entity.data->width;
        render_pos.top = (int)center_relpos.y;
        render_pos.bottom = render_pos.top + entity.data->height;
        entity.data->spr.render(wnd, render_pos);
    }
}
