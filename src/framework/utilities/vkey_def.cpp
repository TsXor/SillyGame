#include "vkey_def.hpp"
#include <vector>

static const std::vector<vkey::info> vkey_info_list {
    {"left", {}}, {"right", {}}, {"up", {}}, {"down", {}},
    {"check", {}},
    {"menu", {vkey::code::SKIP}},
    {"skip", {vkey::code::MENU, vkey::code::CANCEL}},
    {"cancel", {vkey::code::SKIP}},
};

size_t vkey::code_count() { return vkey_info_list.size(); }

const vkey::info& vkey::code_info(vkey::code c) {
    return vkey_info_list[static_cast<size_t>(c) - 1];
}
