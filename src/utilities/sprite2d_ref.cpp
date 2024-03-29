#include "sprite2d_ref.hpp"
#include "facilities/game_window.hpp"

sprite2d_ref::sprite2d_ref(game_window& wnd_, const sprite2d& spr_) : wnd(wnd_), spr(spr_) {
    wnd.texman.want_texture(spr.file_path.c_str());
}

void sprite2d_ref::blit(const position& xy, const glm::mat4& transform) {
    wnd.renman.blit(spr, xy, transform);
}
void sprite2d_ref::blit(const position& xy) {
    wnd.renman.blit(spr, xy);
}
