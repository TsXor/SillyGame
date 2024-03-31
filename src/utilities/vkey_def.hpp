#pragma once
#ifndef __VKEY_DEF__
#define __VKEY_DEF__

#include <string>
#include <unordered_set>

namespace vkey {

enum class code {
    NO_VKEY, // 自行控制键盘事件
    LEFT, RIGHT, UP, DOWN,
    CHECK, // 触发事件/点击按钮/继续对话
    MENU, // 打开菜单
    SKIP, // 跳过对话及其他
    CANCEL, // 上一级菜单/重置操作
};

enum class mode {
    VKEY_ONLY, // 仅接受存在对应虚拟按键码的按键
    RKEY_ONLY, // 接受所有按键，虚拟按键码永远为NO_VKEY
    MIXED, // 接受所有按键，无虚拟按键码则为NO_VKEY
};

struct info {
    std::string name; // 保存时使用的名称
    std::unordered_set<code> compatible; // 可使用同一物理按键
};

// 虚拟按键码的总数量
size_t code_count();

// 获取指定虚拟按键码的信息
const info& code_info(code c);

} // namespace vkey

#endif // __VKEY_DEF__
