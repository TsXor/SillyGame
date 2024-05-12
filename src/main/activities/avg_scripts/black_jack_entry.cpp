#include "../avg_scripts.hpp"
#include "activities/black_jack.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::black_jack_entry(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.black_jack_entry_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(1024, 512);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::black_jack_entry(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 1024, 0, 256}},
            {basics::aabb{0, 1024, 448, 512}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            left = {basics::aabb{0, 16, 256, 448}},
            right = {basics::aabb{1008, 1024, 256, 448}};
        utils::add_trigger_boxes(self, left, right);
        utils::coll_event_table colls {
            { {person.ptr(), left.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_room", std::nullopt, "311"); } },
            { {person.ptr(), right.ptr()}, [&](){
                person.data.velocity = {0, 0};
                self.simu->teleport_entity_center(person.ptr(), points.black_jack_entry_right);
                self.call<acts::black_jack_scene>();
            } },
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({avg_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
