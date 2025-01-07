---
comments: true
meta:
  - property: og:title
    content: 项目状态
---

# 项目状态

目前 Plant-OS 分为以下几个部分

- boot ![](https://img.shields.io/badge/ia32-orange)
  - state: `Final`
  - author: min0911Y & duoduo70 & ZhouZhihaos
- boot ![amd64](https://img.shields.io/badge/amd64-green)
  - state: `TODO`
  - author: ~~copi143~~
- loader ![](https://img.shields.io/badge/ia32-orange)
  - state: `Semi-abandoned` (仅在需要时同步 kernel 的更新)
  - author: min0911Y & duoduo70 & ZhouZhihaos & copi143
- [kernel](kernel/index.md) ![ia32](https://img.shields.io/badge/ia32-orange)
  - state: `In-progress`
  - author: min0911Y & duoduo70 & ZhouZhihaos & copi143
- libc-base ![](https://img.shields.io/badge/ia32-orange) ![amd64](https://img.shields.io/badge/amd64-green)<br>
  用于内核的 C 标准库 (C 库中不依赖内核的内容)
  - state: `In-progress`
  - author: copi143
- pl_readline
  - state: `Final`
  - author: min0911Y
- data-structure
  - state: `In-progress`
  - author: copi143 & min0911Y
- algorithm
  - state: `TODO`
  - author: ~~copi143~~
- fs<br>
  文件系统支持
  - state: `In-progress`
  - author: copi143 & min0911Y & XIAOYI12
  - remark: The fatfs part comes from [abbrev/fatfs](https://github.com/abbrev/fatfs).
  - remark: The iso9660 part comes from [xiaoyi1212/CoolPotOS/src/fs](https://github.com/xiaoyi1212/CoolPotOS/blob/main/src/fs/iso9660.c)
- sys<br>
  Plant-OS 系统调用封装
  - state: `In-progress`
  - author: copi143
- libc<br>
  用于应用程序的 C 标准库
  - state: `TODO`
  - author: ~~copi143~~
- libc++<br>
  用于应用程序的 C++ 标准库
  - state: `TODO`
  - author: ~~copi143~~
- misc
  - state: `In-progress`
  - author: copi143
- font
  - state: `Paused`
  - author: copi143
- plty
  - state: `Paused` 考虑只将其作为最简实现，plos 将使用 [os-terminal](https://github.com/plos-clan/libos-terminal)
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

## Plant-OS User Interface

**Plant-OS 用户界面**<span style="margin: 0 10px;"></span>[[github]](https://github.com/plos-clan/Plant-OS)

[旧储存库](https://github.com/plos-clan/plui)已废弃不使用，迁移到 Plant-OS 储存库中

## Plant-OS apps

- state: `TODO`
- author: ~~copi143 & min0911Y~~

> 可以从[旧储存库](https://github.com/min0911Y/Plant-OS)移植

## 项目状态的解释

```md
`TODO`: 画大饼，准备做但几乎没有开始
`In-progress`: 正在编写但仍未达到可以发布
`Released`: 已发布并会持续更新
`Semi-abandoned`: 很长一段时间内不会被更新
`Final`: 已是最终版本，除修复 bug 外不会执行其它更新
`Paused`: 暂停更新
```

*[TODO]: 画大饼，准备做但几乎没有开始
*[In-progress]: 正在编写但仍未达到可以发布
*[Released]: 已发布并会持续更新
*[Semi-abandoned]: 很长一段时间内不会被更新
*[Final]: 已是最终版本，除修复 bug 外不会执行其它更新
*[Paused]: 由于各种原因暂停更新
