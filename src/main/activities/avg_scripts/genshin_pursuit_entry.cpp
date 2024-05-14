#include "../avg_scripts.hpp"
#include "./utils.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"

using namespace acts::avg_scripts;
using namespace naive_engine;
using namespace silly_framework;

void acts::avg_scripts::genshin_pursuit_entry(avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg) {
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        // 出生点
        basics::vec2 spawn_pos = spawn.value_or(points.genshin_pursuit_entry_default);
        simulator::entity_node_t person(basics::aabb(-16, 16, -12, 12), spawn_pos);
        self.simu.emplace(512, 640);
        utils::add_main_char(self, person);

        self.bound_map.emplace(&maps::genshin_pursuit_entry(), 4.0);
        self.bound_sprites[person.ptr()] = {&sprites::container_small(), 4.0, {0, -36}};

        // 地图边界固定阻挡物
        simulator::entity_node_t obstacles[] = {
            {basics::aabb{0, 192, 0, 576}},
            {basics::aabb{320, 512, 0, 640}},
        };
        utils::add_map_obstacles(self, obstacles);
        
        simulator::entity_node_t
            hidden = {basics::aabb{0, 16, 576, 640}},
            launch = {basics::aabb{192, 320, 0, 208}},
            leave = {basics::aabb{192, 320, 624, 640}};
        utils::add_trigger_boxes(self, hidden, launch, leave);
        utils::coll_event_table colls {
            { {person.ptr(), hidden.ptr()}, [&](){ self.next<acts::avg_scene>("genshin_pursuit_author", points.genshin_pursuit_author_right); } },
            { {person.ptr(), launch.ptr()}, [&](){ self.next<acts::avg_scene>("genshin_pursuit_main", points.genshin_pursuit_main_bottom); } },
            { {person.ptr(), leave.ptr()}, [&](){ self.next<acts::avg_scene>("dorm_room", std::nullopt, "605"); } },
        };

        while (true) {
            auto [_, evt_data] = co_await self.cohost.wait_event({simu_coro_host::EVT_COLLISION});
            utils::check_coll_events(self, colls, evt_data);
        }
    });
}
