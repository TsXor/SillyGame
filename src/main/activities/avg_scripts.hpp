#pragma once
#ifndef __AVG_SCRIPTS__
#define __AVG_SCRIPTS__

#include "./avg_scene.hpp"

namespace acts::avg_scripts {

void dorm_room(avg_scene& self, const std::string& arg);
void dorm_balcony(avg_scene& self, const std::string& arg);
void dorm_corridor_left(avg_scene& self, const std::string& arg);

} // namespace acts::avg_scripts

#endif // __AVG_SCRIPTS__
