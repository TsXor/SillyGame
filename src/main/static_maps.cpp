// 这个文件是自动生成的，别改。

#include "./static_maps.hpp"

using namespace silly_framework;

using map_blocks = std::vector<map2d::block_pos>;

const map2d& maps::dorm_balcony() {
    static const map2d map("dorm_balcony.png",
        map_blocks{
            {{0, 128, 0, 88}, {0, 0}}
        }
    );
    return map;
}
const map2d& maps::dorm_corridor_left() {
    static const map2d map("dorm_corridor_left.png",
        map_blocks{
            {{0, 128, 0, 256}, {0, 0}}
        }
    );
    return map;
}
const map2d& maps::dorm_room() {
    static const map2d map("dorm_room.png",
        map_blocks{
            {{0, 128, 0, 264}, {0, 0}}
        }
    );
    return map;
}
