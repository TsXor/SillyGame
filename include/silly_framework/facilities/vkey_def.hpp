#pragma once
#ifndef __SF_VKEY_DEF__
#define __SF_VKEY_DEF__

#include <string>
#include <unordered_set>

namespace silly_framework::vkey {

enum class code {
    NO_VKEY, // 占位
    LEFT, RIGHT, UP, DOWN,
    CHECK, // 触发事件/点击按钮/继续对话
    MENU, // 打开菜单
    SKIP, // 跳过对话及其他
    CANCEL, // 上一级菜单/重置操作
};

// 笑点解析：这个结构体是为了自定义键位映射设计的，
//           但是我觉得我很显然写不到那了。
struct info {
    std::string name; // 保存时使用的名称
    std::unordered_set<code> compatible; // 可使用同一物理按键
};

// 虚拟按键码的总数量
size_t code_count();

// 获取指定虚拟按键码的信息
const info& code_info(code c);

} // namespace silly_framework::vkey

#endif // __SF_VKEY_DEF__
