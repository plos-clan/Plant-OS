---
comments: true
meta:
  - property: og:title
    content: 编译
title: 编译 Plant-OS
---

# Compile Plant-OS / 编译 Plant-OS

你至少需要安装 `clang` `cmake` `mtools` `qemu-system-i386` 来编译此项目。

> 可以使用 `gcc` 替代 `clang` 我们会尽量保证可以在 `gcc` 下正常编译。

我们建议 gcc 版本在 13 以上，clang 版本在 18 以上，如果你的版本过旧可能无法编译。

如果你的系统是 Debian 或 Ubuntu 那么可以使用以下命令安装依赖：

```sh
sudo apt update
sudo apt install cmake nasm mtools qemu-system-i386 extra-cmake-modules
```

上面的命令并不会安装 `gcc` `clang` 请自行安装：

`clang`：

```sh
sudo apt install clang lld lldb
```

`gcc`：

```sh
sudo apt install gcc g++ gdb
```

如果你只安装了默认的 `clang` 包那么大概率你是没有安装 `lld` 的，你必须手动安装它否则会导致编译错误：

```sh
sudo apt install lld
```

如果你要进行开发那么建议安装以下包：

```sh
sudo apt install clangd clang-tidy clang-format libfreetype-dev
```

## 自动配置

### 克隆储存库

```sh
git clone https://github.com/plos-clan/Plant-OS.git
cd Plant-OS
```

### 运行配置脚本

```sh
./init.sh
```

> 注意如果出现下载资源错误，可以更换资源下载地址，也可以手动下载资源文件
>
> 见 init.sh 的 SRC_MIRROR_SITE

### 运行

切换到 build 目录后执行以下命令

```sh
make run
```

也可以不通过包装直接执行启动脚本，在 bin 目录下执行：

```sh
./run.sh
```

### 再次编译

切换到 build 目录下，执行命令：

```sh
make make
```

也可以清理所有编译文件，重新编译：

```sh
rm -r ./* && cmake .. && make make
```

## 手动配置

### 克隆储存库

```sh
git clone https://github.com/plos-clan/Plant-OS.git
cd Plant-OS
```

### 下载资源文件

在 bin 目录下下载以下资源文件（压缩包请解压）：

```plain
https://plos-resources.pages.dev/resource-files.zip
https://plos-resources.pages.dev/ext-resources.zip
https://plos-resources.pages.dev/Ligconsolata-Regular.ttf
https://plos-resources.pages.dev/SourceHanSans-Light.ttc
```

在 apps/lib 目录下下载以下资源文件（压缩包请解压）：

```plain
https://plos-resources.pages.dev/lib.zip
```

默认没有 bin 和 apps/lib 目录，需要手动创建

### 配置

```sh
mkdir build && cd build
cmake ..
```

### 编译

```sh
make make
```

如果你只需要 Plant-OS 本体，请编译 `make` 目标

运行使用 `run` 目标

如果你需要编译本机上的 util 和 单元测试 你还需要安装 `gtest` `freetype` 等

## 若编译时报错

~~如果编译时出现报错 `undefined reference to` `__udivdi3` `__umoddi3` `__divdi3` `__moddi3` 等，可以尝试链接 `libgcc` 库 (`-lgcc`)~~ <span style="color: orange">已经修复</span>

如果在 gcc11 或更早版本中编译时出现异常，可删除项目根目录 `CMakeLists.txt` 中的 `include(cmake/lto)`
