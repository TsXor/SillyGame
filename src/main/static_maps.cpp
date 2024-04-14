// 这个文件是自动生成的，别改。

#include "./static_maps.hpp"

using namespace silly_framework;

using map_blocks = std::vector<map2d::block_pos>;

const map2d& maps::dorm() {
    static const map2d map("dorm.png",
        map_blocks{
            {{0, 128, 0, 256}, {0, 0}}
        }
    );
    return map;
}
