#include "silly_framework/facilities/vkey_def.hpp"
#include <vector>

using namespace silly_framework::vkey;
namespace impl = silly_framework::vkey;

static const std::vector<info> vkey_info_list {
    {"left", {}}, {"right", {}}, {"up", {}}, {"down", {}},
    {"check", {}},
    {"menu", {code::SKIP}},
    {"skip", {code::MENU, code::CANCEL}},
    {"cancel", {code::SKIP}},
};

size_t impl::code_count() { return vkey_info_list.size(); }

const info& impl::code_info(code c) {
    return vkey_info_list[static_cast<size_t>(c) - 1];
}
