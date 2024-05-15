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

`cmake`构建成功后如下执行命令即可打包：
```bash
python select_pack.py <build文件夹>
```
例如：
```bash
python select_pack.py build/RelWithDebInfo
```

注：
- 编译器需要支持`C++20`，所以请不要使用上古版本`Visual Studio`。
  - 注：gcc 11.2.0由于`format`不在标准库中应该也不能编译，直接使用`fmtlib`可以解决。
- 写的时候是按跨平台写的，但是我没拿其他平台测试过。
- 即使你是从gitee镜像克隆本仓库，你仍需要一些github加速手段来同步子模块。

## 随机冷知识
- 本项目没有使用Qt、MFC、EasyX等框架。
- 对于一些压缩软件，直接在压缩包里点击运行是不会成功的。这是因为你点击运行时只解压了`exe`，而它依赖的`dll`和图片都没有解压出来。运行软件请**完整解压**再运行。
- 如果你的程序源文件中出现了类似`"C:\\Users\\SillyMan\\Pictures\\C++项目\\kobe.png"`的字符串，那你就不用指望这个东西能在别人电脑上运行起来了，因为你使用了绝对路径。
  - 使用Qt一般不会遇到这个问题，但请注意你在`qrc`文件中设置的prefix。
- 如果你Windows电脑的账户名是中文，你用Qt Designer编译项目时得开个纯英文名的新用户来做，这是因为Qt Designer在编译时会用到你用户文件夹下的临时文件夹，而它不能处理中文路径。
  - **在设置里改掉你的用户名没有用**，Windows只会更改你的显示名称，用户文件夹不变。
- 如果你写的程序使用了外部库，请不要`using namespace std;`，因为这很可能会造成名称遮盖，用人话说就是重名了编译器不知道挑哪个。
- 你的Git存储库里不应该存放编译结果，请把它放在release里面。
  - 当然，这是不妨碍你完成大作业的，请放心应付。
- Git是**版本管理工具**，不是你的网盘。
  - 笑点解析：Git会忠实地保留每个提交节点，所以任何人都能看到你的，以及我的代码中的某些黑历史，令人忍俊不禁。

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
