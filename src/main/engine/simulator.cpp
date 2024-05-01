#include "./simulator.hpp"

using namespace naive_engine;

simulator::simulator(double width, double height):
    boxtree(6, width, height) {}
simulator::~simulator() {}

simulator::handle_type simulator::make_entity(const basics::vec2& pos) {
    auto ent = --entities.end();
    ent->hbox = boxtree.add_box(std::bit_cast<void*>(ent), ent->coarse(), pos);
    return ent;
}
void simulator::del_entity(handle_type entity) {
    boxtree.del_box(entity->hbox);
    entities.erase(entity);
}

void simulator::teleport_entity_offset(handle_type entity, basics::vec2 offset) {
    boxtree.move_box(entity->hbox, offset);
}
void simulator::teleport_entity_center(handle_type entity, basics::vec2 center) {
    boxtree.move_box(entity->hbox, center - entity->hbox->offset);
}

auto simulator::colldet_from(handle_type entity) -> coutils::generator<std::pair<handle_type, basics::vec2>> {
    auto box = entity->hbox->box + entity->hbox->offset;
    auto [scw, sch] = boxtree.scene_size();
    // 检查边缘碰撞
    if (box.width() >= scw || box.height() >= sch) { co_return; }
    basics::vec2 border_mtv = {0, 0};
    if (box.left < 0) { border_mtv.x = -box.left; }
    if (box.right > scw) { border_mtv.x = scw - box.right; }
    if (box.top < 0) { border_mtv.y = -box.top; }
    if (box.bottom > sch) { border_mtv.y = sch - box.bottom; }
    if (border_mtv.nonzero()) { co_yield coutils::inituple(null_handle(), border_mtv); }
    // 检查真实碰撞
    for (auto&& possible : boxtree.may_collide(entity->hbox)) {
        auto other = std::bit_cast<handle_type>(possible->parent);
        auto mtv = basics::polygon_colldet(
            entity->convex, other->convex,
            other->hbox->offset - entity->hbox->offset
        );
        if (mtv.nonzero()) { co_yield coutils::inituple(other, mtv); }
    }
}

auto simulator::colldet_from_sources() -> coutils::generator<std::tuple<handle_type, handle_type, basics::vec2>> {
    for (auto&& ent : colldet_source) {
        for (auto&& [other, mtv] : colldet_from(ent)) {
            if (!mtv.nonzero()) { continue; }
            if (other == null_handle()) {
                teleport_entity_offset(ent, mtv);
            } else if (ent->flags.repulsive && other->flags.repulsive) {
                if (!ent->flags.fixed && !other->flags.fixed) {
                    teleport_entity_offset(ent, mtv / 2.0);
                    teleport_entity_offset(other, -mtv / 2.0);
                } else if (!ent->flags.fixed && other->flags.fixed) {
                    teleport_entity_offset(ent, mtv);
                } else if (ent->flags.fixed && !other->flags.fixed) {
                    teleport_entity_offset(other, -mtv);
                }
            }
            co_yield coutils::inituple(ent, other, mtv);
        }
    }
    co_return;
}

void simulator::on_tick(double dt) {
    for (auto&& entity : entities) {
        boxtree.move_box(entity.hbox, entity.velocity * dt);
    }
}
