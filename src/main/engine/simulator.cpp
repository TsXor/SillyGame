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

auto simulator::colldet(handle_type entity) -> cppcoro::generator<std::pair<handle_type, basics::vec2>> {
    std::unordered_set<handle_type::pointer> found_coll;
    for (size_t i = 0; i < entity->hbox->boxes.size(); ++i) {
        grouped_hitbox_ref boxptr{*entity->hbox, i};
        for (auto&& possible : boxtree.may_collide(boxptr)) {
            auto other = handle_from_boxref(possible);
            if (found_coll.contains(&*other)) { continue; }
            auto mtv = basics::polygon_colldet(
                entity->poly_at(i), other->poly_at(possible.idx),
                other->hbox->offset - entity->hbox->offset
            );
            if (mtv.nonzero()) {
                found_coll.emplace(&*other);
                co_yield {other, mtv};
            }
        }
    }
}

void simulator::tick(double dt) {
    for (auto&& entity : entities) {
        boxtree.move_box(entity.hbox, entity.velocity * dt);
    }
}
void simulator::render(game_window& wnd) {
    for (auto&& entity : entities) {
        auto pos = entity.hbox->offset;
        glut::position rpos;
        rpos.left = (int)pos.x;
        rpos.right = rpos.left + entity.data->width;
        rpos.top = (int)pos.y;
        rpos.bottom = rpos.top + entity.data->height;
        entity.data->spr.render(wnd, rpos);
    }
}
