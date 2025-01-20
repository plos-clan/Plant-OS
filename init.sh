#!/usr/bin/bash

info() {
  echo -e "\033[32m[Info]\033[0m $1"
  echo -e "\033[32m[Info]\033[0m $1" >>init.log
}

fail() {
  echo -e "\033[31m[Fail]\033[0m $1"
  echo -e "\033[31m[Fail]\033[0m $1" >>init.log
}

check_command() {
  if ! command -v "$1" &>/dev/null; then
    fail "找不到 $1 命令"
    fail "$2"
    if [ -n "$3" ]; then
      echo -e "\033[33m- 尝试 $3\033[0m"
    fi
    exit 1
  fi
}

check() {
  if [ "$GITHUB_ACTIONS" = "true" ]; then
    $*
  else
    echo "$ $*" >>init.log
    $* &>>init.log
  fi
  retcode=$?
  if [ $retcode -ne 0 ]; then
    fail "执行命令 $* 失败"
    fail "返回值: $retcode"
    exit $retcode
  fi
}

check_file() {
  if [ ! -f "$1" ]; then
    fail "文件 $1 不存在"
    exit 1
  fi
}

check_directory() {
  if [ ! -d "$1" ]; then
    fail "文件夹 $1 不存在"
    exit 1
  fi
}

deinit() {
  info 删除旧的配置
  rm -r ./bin
  rm -r ./build
  rm -r ./lib
  rm -r ./apps/lib
}

info 简单检查项目完整性

check_directory './apps'
check_directory './cmake'
check_directory './include'
check_directory './src'
check_directory './test'
check_file './.clang-format'
check_file './.clang-tidy'
check_file './project.yaml'
check_file './LICENSE'
check_file './README.md'

ALREADY_HAS_RESOURCE_FILES=false

if [ -d './bin' ] || [ -d './build' ] || [ -d './lib' ]; then
  if [ "$GITHUB_ACTIONS" = "true" ]; then
    fatal 'GitHub Actions 中不允许存在 bin、build 或 lib 目录'
  fi
  info '检测到存在 bin、build 或 lib 目录，项目可能已经配置完毕。'
  echo -e '\033[33m- y: 删除并继续 n: 取消 f:继续但不重新下载资源文件\033[0m'
  read -p '是否继续？(y/n/f): ' choice
  case "$choice" in
  y | Y) deinit ;;
  n | N) exit 0 ;;
  f | F) ALREADY_HAS_RESOURCE_FILES=true ;;
  *)
    fail 无效的选择
    exit 1
    ;;
  esac
fi

rm ./init.log &>/dev/null

info '开始配置 Plant-OS 项目'

touch init.log

info 创建目录
mkdir -p ./bin
mkdir -p ./bin/resource
mkdir -p ./build
mkdir -p ./lib
mkdir -p ./apps/lib

cd bin
info 创建符号链接
ln -s ../src/boot/run.sh run.sh &>>init.log
ln -s ../src/boot/run-efi.sh run-efi.sh &>>init.log
ln -s ../src/boot/bochs.sh bochs.sh &>>init.log
ln -s ../src/boot/netlog.py netlog.py &>>init.log
ln -s ../src/boot/templates templates &>>init.log
cd ..

if [ "$ALREADY_HAS_RESOURCE_FILES" = "false" ]; then
  # 资源镜像站设置，默认为 'plos-resources.pages.dev'
  SRC_MIRROR_SITE='plos-resources.pages.dev'
  # 由于挂载资源的网站在某些地区无法正常访问，因此我们提供了一些等效的资源网站
  # 开发者如果因网络问题无法下载资源，可以替换域名后尝试
  # 目前支持的镜像资源站有：
  #   plos-rsc.github.io

  cd ./bin/resource
  info 下载资源文件
  check_command curl '需要使用 curl 来下载资源文件' 'apt install curl'
  check_command unzip '需要使用 unzip 来解压资源文件' 'apt install unzip'
  info 下载并解压系统必须资源文件...
  check curl -O "https://${SRC_MIRROR_SITE}/resource-files.zip"
  check unzip resource-files.zip
  check rm resource-files.zip
  if [ "$GITHUB_ACTIONS" = "true" ]; then
    info 'GitHub Actions 中跳过下载并解压其它资源文件'
  else
    info 下载并解压其它资源文件...
    check curl -O "https://${SRC_MIRROR_SITE}/ext-resources.zip"
    check unzip ext-resources.zip
    check rm ext-resources.zip
    check curl -O "https://${SRC_MIRROR_SITE}/Ligconsolata-Regular.ttf"
    check curl -O "https://${SRC_MIRROR_SITE}/SourceHanSans-Light.ttc"
  fi
  cd ../..

  cd ./apps/lib
  info 下载并解压应用程序资源文件...
  check curl -O "https://${SRC_MIRROR_SITE}/lib.zip"
  check unzip lib.zip
  check rm lib.zip
  cd ../..
fi

info '成功配置 Plant-OS 项目'
# ------------------------- 配置 CMake -------------------------

cd build
info '初始化 CMake 并进行编译测试'
check_command gcc '需要使用 gcc 来编译项目' 'apt install gcc g++'
check_command nasm '需要使用 nasm 来编译项目' 'apt install nasm'
check_command mformat '需要使用 mformat 来格式化软盘' 'apt install mtools'
check_command cmake '需要使用 cmake 来编译项目' 'apt install cmake extra-cmake-modules'
check_command qemu-img '需要使用 qemu-img 来创建磁盘镜像' 'apt install qemu-utils'
check_command objcopy '需要使用 objcopy 来处理二进制文件' 'apt install binutils'
check cmake .. $*
info 开始编译...
check make make
cd ..

if [ "$GITHUB_ACTIONS" = "true" ]; then
  info 'GitHub Actions 中跳过运行'
else
  cd bin
  info '运行 Plant-OS'
  check_command qemu-system-i386 '需要使用 qemu-system-i386 来运行 Plant-OS' 'apt install qemu-system-i386'
  check ./run.sh
  cd ..
fi

if [ "$GITHUB_ACTIONS" = "true" ]; then
  info 'GitHub Actions 中拷贝 readme license 和 requirements 文件'
  cp ./README.md bin/
  cp ./LICENSE bin/
  cp ./src/boot/requirements.txt bin/
fi

info 配置脚本结束
