#pragma once
#ifndef __AVG_SCRIPTS__
#define __AVG_SCRIPTS__

#include "./avg_scene.hpp"

namespace acts::avg_scripts {

static inline constexpr struct {
    eng::basics::vec2 dorm_room_default = {256, 512};
    eng::basics::vec2 dorm_room_front_door = {256, 1008};
    eng::basics::vec2 dorm_room_back_door = {256, 144};

    eng::basics::vec2 dorm_balcony_default = {256, 160};
    eng::basics::vec2 dorm_balcony_door = {256, 304};

    eng::basics::vec2 dorm_corridor_left_default = {256, 512};
    eng::basics::vec2 dorm_corridor_left_joint = {256, 988};
    eng::basics::vec2 dorm_corridor_door_32 = {148, 336};
    eng::basics::vec2 dorm_corridor_door_11 = {364, 336};
    eng::basics::vec2 dorm_corridor_door_10 = {364, 624};
    eng::basics::vec2 dorm_corridor_door_09 = {364, 912};

    eng::basics::vec2 dorm_corridor_middle_default = {512, 352};
    eng::basics::vec2 dorm_corridor_middle_joint_left = {36, 352};
    eng::basics::vec2 dorm_corridor_middle_joint_right = {988, 352};
    eng::basics::vec2 dorm_corridor_middle_door = {704, 428};
    eng::basics::vec2 dorm_corridor_door_08 = {304, 276};
    eng::basics::vec2 dorm_corridor_door_07 = {720, 276};

    eng::basics::vec2 dorm_corridor_right_default = {256, 960};
    eng::basics::vec2 dorm_corridor_right_joint = {256, 1884};
    eng::basics::vec2 dorm_corridor_door_01 = {148, 368};
    eng::basics::vec2 dorm_corridor_door_02 = {148, 656};
    eng::basics::vec2 dorm_corridor_door_03 = {148, 944};
    eng::basics::vec2 dorm_corridor_door_04 = {148, 1232};
    eng::basics::vec2 dorm_corridor_door_05 = {148, 1520};
    eng::basics::vec2 dorm_corridor_door_06 = {148, 1808};
    eng::basics::vec2 dorm_corridor_door_38 = {364, 368};
    eng::basics::vec2 dorm_corridor_door_37 = {364, 656};
    eng::basics::vec2 dorm_corridor_door_36 = {364, 944};
    eng::basics::vec2 dorm_corridor_door_35 = {364, 1232};
    eng::basics::vec2 dorm_corridor_door_34 = {364, 1520};
    eng::basics::vec2 dorm_corridor_door_33 = {364, 1808};

    eng::basics::vec2 dorm_stairs_default = {320, 192};
    eng::basics::vec2 dorm_stairs_door = {320, 148};
    eng::basics::vec2 dorm_stairs_down = {216, 236};
    eng::basics::vec2 dorm_stairs_up = {424, 236};

    eng::basics::vec2 dorm_stairs_middle_default = {320, 536};
    eng::basics::vec2 dorm_stairs_middle_down = {216, 580};
    eng::basics::vec2 dorm_stairs_middle_up = {424, 580};

    eng::basics::vec2 dorm_stairs_bottom_default = {160, 384};
    eng::basics::vec2 dorm_stairs_bottom_up = {160, 556};
} points;

#define AVG_SCRIPT_ARGS avg_scene& self, const std::optional<eng::basics::vec2>& spawn, const std::string& arg

void dorm_room(AVG_SCRIPT_ARGS);
void dorm_balcony(AVG_SCRIPT_ARGS);
void dorm_corridor_left(AVG_SCRIPT_ARGS);
void dorm_corridor_middle(AVG_SCRIPT_ARGS);
void dorm_corridor_right(AVG_SCRIPT_ARGS);
void dorm_stairs(AVG_SCRIPT_ARGS);
void dorm_stairs_middle(AVG_SCRIPT_ARGS);
void dorm_stairs_bottom(AVG_SCRIPT_ARGS);

#undef AVG_SCRIPT_ARGS

} // namespace acts::avg_scripts

#endif // __AVG_SCRIPTS__
