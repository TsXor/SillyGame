#include "./simulator.hpp"

using namespace naive_engine;

simulator::simulator(double width, double height):
    boxtree(6, width, height) {}
simulator::~simulator() {}

auto hitbox_to_entity(grid_loose_quadtree::hitbox* hbox) {
    return reinterpret_cast<simulator::entity_t*>(hbox);
}

void simulator::add_entity(entity_node_t& entity) {
    boxtree.add_box(&entity.data.hbox);
    entities.push_back(entity);
}
void simulator::del_entity(entity_node_t& entity) {
    boxtree.del_box(&entity.data.hbox);
    entity.unlink();
}

void simulator::teleport_entity_offset(entity_t* entity, basics::vec2 offset) {
    boxtree.move_box(&entity->hbox, offset);
}
void simulator::teleport_entity_center(entity_t* entity, basics::vec2 center) {
    boxtree.move_box(&entity->hbox, center - entity->hbox.offset);
}

auto simulator::colldet_from(entity_t* entity) -> coutils::generator<std::pair<entity_t*, basics::vec2>> {
    auto box = entity->hbox.abs_box();
    auto [scw, sch] = boxtree.scene_size();
    // 检查边缘碰撞
    if (box.width() >= scw || box.height() >= sch) { co_return; }
    basics::vec2 border_mtv = {0, 0};
    if (box.left < 0) { border_mtv.x = -box.left; }
    if (box.right > scw) { border_mtv.x = scw - box.right; }
    if (box.top < 0) { border_mtv.y = -box.top; }
    if (box.bottom > sch) { border_mtv.y = sch - box.bottom; }
    if (border_mtv.nonzero()) { co_yield coutils::inituple(nullptr, border_mtv); }
    // 检查真实碰撞
    for (auto&& possible : boxtree.may_collide(&entity->hbox)) {
        auto other = hitbox_to_entity(possible);
        auto mtv = basics::polygon_colldet(
            entity->convex, other->convex,
            other->hbox.offset - entity->hbox.offset
        );
        if (mtv.nonzero()) { co_yield coutils::inituple(other, mtv); }
    }
}

auto simulator::colldet_from_sources() -> coutils::generator<std::tuple<entity_t*, entity_t*, basics::vec2>> {
    for (auto&& ent : colldet_source) {
        for (auto&& [other, mtv] : colldet_from(ent)) {
            if (!mtv.nonzero()) { continue; }
            if (other == nullptr) {
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
        boxtree.move_box(&entity.data.hbox, entity.data.velocity * dt);
    }
}
