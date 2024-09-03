# PlantOS

<div>
  <img id="logo"src="assets/plos.png" width="250" height="250" align="right">
</div>

A simple OS used to study made by several students。

如果你要运行 PlantOS, 请先下载[资源文件](https://copi144.eu.org:2000/index.php/s/7BoGFkD7JR52rFP)并将其放置到 bin 目录下

[网页版文档](https://plos-clan.github.io/doc) 未完成，别看（

<br>
<br>

![Static Badge](https://img.shields.io/badge/License-MIT-blue)
![Static Badge](https://img.shields.io/badge/Language-3-orange)
![Static Badge](https://img.shields.io/badge/hardware-x86-green)
![Static Badge](https://img.shields.io/badge/model-plui_%7C_pl_readline_%7C_plty-8A2BE2)

## 编译 Plant-OS

你至少需要安装 `clang` `cmake` `mtools` `qemu-system-i386` 来编译此项目

> 可以使用 `gcc` 替代 `clang` 我们会尽量保证可以在 `gcc` 下正常编译

如果你只需要 Plant-OS 本体，请编译 `make` 目标

运行使用 `run` 目标

如果你需要编译本机上的 util 和 单元测试 你还需要安装 `gtest` `freetype` 等

如果编译时出现报错 `undefined reference to` `__udivdi3` `__umoddi3` `__divdi3` `__moddi3` 等，可以尝试链接 `libgcc` 库 (`-lgcc`)

如果在 gcc11 或更早版本中编译时出现异常，可删除项目根目录 `CMakeLists.txt` 中的 `include(cmake/lto)`

## 项目结构

目前 PlantOS 分为以下几个部分

- boot
  - state: `Semi-abandoned`
  - author: min0911Y & duoduo70 & ZhouZhihaos
- loader
  - state: `Semi-abandoned`
  - author: min0911Y & duoduo70 & ZhouZhihaos
- kernel
  - state: `In progress`
  - author: min0911Y & duoduo70 & ZhouZhihaos & copi143 (minority)
- apps
  - state: `TODO`
  - author: ~~copi143 & min0911Y~~
- libc-base
  - state: `In progress`
  - author: copi143
- pl_readline
  - state: `Final`
  - author: min0911Y
- data-structure
  - state: `In progress`
  - author: copi143 (majority) & min0911Y (minority)
- algorithm
  - state: `TODO`
  - author: ~~copi143~~
- fs
  - state: `In progress`
  - author: copi143 & min0911Y & XIAOYI12
  - remark: The fatfs part comes from [abbrev/fatfs](https://github.com/abbrev/fatfs).
  - remark: The iso9660 part comes from [xiaoyi1212/CoolPotOS/src/fs](https://github.com/xiaoyi1212/CoolPotOS/blob/main/src/fs/iso9660.c)
- sys
  - state: `In progress`
  - author: copi143
- libc
  - state: `TODO`
  - author: ~~copi143~~
- libc++
  - state: `TODO`
  - author: ~~copi143~~
- misc
  - state: `In progress`
  - author: copi143
- font
  - state: `Paused`
  - author: copi143
- plty
  - state: `Paused`
  - author: copi143
- pl2d
  - state: `Paused`
  - author: copi143
  - Waiting for more developers.
- plds
  - state: `TODO`
  - author: ~~copi143~~
  - Waiting for more developers.
- plui
  - state: `TODO`
  - author: ~~copi143~~
  - Waiting for more developers.
- plac
  - state: `Final`
  - author: copi143
- usersystem
  - state: `TODO`
  - author XIAOYI12

**state** 解释

- `TODO`: 画大饼，准备做但几乎没有开始
- `In progress`: 正在编写但仍未达到可以发布
- `Semi-abandoned`: 很长一段时间内不会被更新
- `Final`: 已是最终版本，除修复 bug 外不会执行其它更新
- `Paused`: 暂停更新

**`python` 构建脚本被暂时删除**

历史版本见 [dev-plasma](https://github.com/plos-clan/Plant-OS/tree/dev-plasma)

# 反馈或讨论问题

您可以在此仓库中提出 Issue

请在 Plant-OS 的 QQ 群 994106260 中进行讨论

也可以发送邮件到 <Yan.Huang24@student.xjtlu.edu.cn>
