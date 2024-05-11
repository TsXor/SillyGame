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

auto simulator::colldet() -> coutils::generator<coll_info> {
    auto [scw, sch] = boxtree.scene_size();
    // 检查边缘碰撞
    for (auto&& entity : entities) {
        auto box = entity.data.hbox.abs_box();
        if (box.width() > scw || box.height() > sch) { continue; }
        basics::vec2 border_mtv = {0, 0};
        if (box.left < 0) { border_mtv.x = -box.left; }
        if (box.right > scw) { border_mtv.x = scw - box.right; }
        if (box.top < 0) { border_mtv.y = -box.top; }
        if (box.bottom > sch) { border_mtv.y = sch - box.bottom; }
        if (border_mtv.nonzero()) { co_yield coutils::inituple(entity.ptr(), nullptr, border_mtv); }
    }
    // 检查真实碰撞
    for (auto&& [coll_a, coll_b] : boxtree.colldet()) {
        auto ent_a = hitbox_to_entity(coll_a);
        auto ent_b = hitbox_to_entity(coll_b);
        auto mtv = basics::polygon_colldet(
            ent_a->convex, ent_b->convex,
            ent_b->hbox.offset - ent_a->hbox.offset
        );
        if (mtv.nonzero()) { co_yield coutils::inituple(ent_a, ent_b, mtv); }
    }
}

auto simulator::colldet_and_react() -> coutils::generator<coll_info> {
    std::vector<coll_info> colls; colls.reserve(512);
    for (auto&& res : colldet()) { colls.push_back(res); }
    for (auto&& [ent_a, ent_b, mtv] : colls) {
        if (ent_b == nullptr) {
            teleport_entity_offset(ent_a, mtv);
        } else if (ent_a->flags.repulsive && ent_b->flags.repulsive) {
            if (!ent_a->flags.fixed && !ent_b->flags.fixed) {
                teleport_entity_offset(ent_a, mtv / 2.0);
                teleport_entity_offset(ent_b, -mtv / 2.0);
            } else if (!ent_a->flags.fixed && ent_b->flags.fixed) {
                teleport_entity_offset(ent_a, mtv);
            } else if (ent_a->flags.fixed && !ent_b->flags.fixed) {
                teleport_entity_offset(ent_b, -mtv);
            }
        }
        co_yield coutils::inituple(ent_a, ent_b, mtv);
    }
}

void simulator::on_tick(double dt) {
    for (auto&& entity : entities) {
        boxtree.move_box(&entity.data.hbox, entity.data.velocity * dt);
    }
}
