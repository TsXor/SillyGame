# SillyGame
你说得对，但是`SillyGame`是一款基于`OpenGL`相关库编写的意义不明的不知道是不是游戏。后面忘了。

## 我超，盒！
南开大学2023级高级语言程序设计2-2大作业。  

## 编译
估计没谁会闲到编译这玩意（乐）

执行以下命令同步子模块：
```bash
git submodule init
git submodule update
```
然后公式`cmake`编译即可。建议使用`mingw`编译。

注：
- 编译器需要支持`C++20`，所以请不要使用上古版本`Visual Studio`。
- 写的时候是按跨平台写的，但是我没拿其他平台测试过。

## ~~暗杀名单~~
### 使用的库
- glad, glfw, glm, oglwrap -- OpenGL编程
- stb libs -- 图片文件解码
- spdlog -- 日志记录
- libuv, uv-cpp -- 文件IO

### 使用的素材
暂无。

### 开发工具
- Visual Studio Code -- IDE
- Pixelorama -- 像素创作

### 开发参考
- Learn OpenGL
- Deltarune Chapter 1 & 2 Demo
