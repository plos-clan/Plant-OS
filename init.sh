#!/usr/bin/bash
set -o errtrace
set -o pipefail
function CATCH_ERROR {
  local __LEC=$? __i
  echo "Traceback (most recent call last):" >&2
  for ((__i = ${#FUNCNAME[@]} - 1; __i >= 0; --__i)); do
    printf '  File %q line %s in %q\n' \
      "${BASH_SOURCE[$__i]}" \
      "${BASH_LINENO[$__i]}" \
      "${FUNCNAME[$__i]}" >&2
  done
  echo "Error: [ExitCode: ${__LEC}]" >&2
  exit "${__LEC}"
}
trap CATCH_ERROR ERR

INIT_LOG_PATH=$(pwd)/init.log

INFO=$'\e[32m[Info]\e[m'
FAIL=$'\e[31m[Fail]\e[m'

log() {
  echo "$*" >>"$INIT_LOG_PATH"
}

step() {
  echo "$INFO $1"
  printf '\n## %s\n' "$1" >>"$INIT_LOG_PATH"
}

substep() {
  echo "$INFO $1"
  printf '\n### %s\n' "$1" >>"$INIT_LOG_PATH"
}

info() {
  echo "- $INFO $1"
  printf '\n- [Info] %s\n' "$1" >>"$INIT_LOG_PATH"
}

fail() {
  echo "$FAIL $1"
  printf '\n- [Fail] %s\n' "$1" >>"$INIT_LOG_PATH"
}

enter() {
  info "进入目录 $1"
  cd -- "$1" || return $?
}

check_command() {
  if ! hash -- "$1"; then
    fail "$(printf '找不到 %q 命令' "$1")"
    fail "$2"
    if [ -n "$3" ]; then
      printf '\e[33m- 尝试 %s\e[m\n' "$3"
    fi
    exit 1
  fi
}

run() {
  local retcode=0
  if [ "$GITHUB_ACTIONS" = true ]; then
    "$@"
  else
    echo
    echo '```sh'
    printf '$'
    printf ' %q' "$@"
    echo

    "$@" || retcode=$?

    echo "-> $retcode"
    echo '```'
    return $retcode
  fi &>>"$INIT_LOG_PATH"
}

check() {
  local retcode=0
  if [ "$GITHUB_ACTIONS" = true ]; then
    "$@" || retcode=$?
  else
    echo
    echo '```sh'
    printf '$'
    printf ' %q' "$@"
    echo

    "$@" || retcode=$?

    echo '```'
  fi &>>"$INIT_LOG_PATH"

  if [ $retcode -ne 0 ]; then
    fail "执行命令 ${*@Q} 失败"
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
  run rm -r ./bin
  run rm -r ./build
  run rm -r ./lib
  run rm -r ./apps/lib
}

: >|"$INIT_LOG_PATH"

log '# Plant-OS 自动配置日志'

step 简单检查项目完整性

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
  if [ "$GITHUB_ACTIONS" = true ]; then
    fatal 'GitHub Actions 中不允许存在 bin、build 或 lib 目录'
  fi
  info '检测到存在 bin、build 或 lib 目录，项目可能已经配置完毕。'
  echo -e '\e[33m- y: 删除并继续 n: 取消 f:继续但不重新下载资源文件\e[m'
  read -rp '是否继续？(y/n/f): '
  case "$REPLY" in
  [Yy]) deinit ;;
  [Nn]) exit 0 ;;
  [Ff]) ALREADY_HAS_RESOURCE_FILES=true ;;
  *)
    fail 无效的选择
    exit 1
    ;;
  esac
fi

step '开始配置 Plant-OS 项目'

step 创建目录
run mkdir -p ./bin
run mkdir -p ./bin/resource
run mkdir -p ./build
run mkdir -p ./lib
run mkdir -p ./apps/lib

(
  enter bin

  run ln -s ../src/boot/run.sh run.sh
  run ln -s ../src/boot/run-efi.sh run-efi.sh
  run ln -s ../src/boot/bochsrc.txt bochsrc.txt
  run ln -s ../src/boot/bochs.sh bochs.sh
  run ln -s ../src/boot/netlog.py netlog.py
  run ln -s ../src/boot/templates templates
)

if [ "$ALREADY_HAS_RESOURCE_FILES" = "false" ]; then
  # 资源镜像站设置，默认为 'plos-resources.pages.dev'
  SRC_MIRROR_SITE='plos-resources.pages.dev'
  # 由于挂载资源的网站在某些地区无法正常访问，因此我们提供了一些等效的资源网站
  # 开发者如果因网络问题无法下载资源，可以替换域名后尝试
  # 目前支持的镜像资源站有：
  #   plos-rsc.github.io

  (
    enter ./bin/resource

    step 下载资源文件
    check_command curl '需要使用 curl 来下载资源文件' 'apt install curl'
    check_command unzip '需要使用 unzip 来解压资源文件' 'apt install unzip'
    substep 下载并解压系统必须资源文件...
    check curl -O "https://${SRC_MIRROR_SITE}/resource-files.zip"
    check unzip resource-files.zip
    check rm resource-files.zip
    if [ "$GITHUB_ACTIONS" = true ]; then
      info 'GitHub Actions 中跳过下载并解压其它资源文件'
    else
      substep 下载并解压其它资源文件...
      check curl -O "https://${SRC_MIRROR_SITE}/ext-resources.zip"
      check unzip ext-resources.zip
      check rm ext-resources.zip
      check curl -O "https://${SRC_MIRROR_SITE}/Ligconsolata-Regular.ttf"
      check curl -O "https://${SRC_MIRROR_SITE}/SourceHanSans-Light.ttc"
    fi
  )

  (
    enter ./apps/lib

    substep 下载并解压应用程序资源文件...
    check curl -O "https://${SRC_MIRROR_SITE}/lib.zip"
    check unzip lib.zip
    check rm lib.zip
  )
fi

step '成功配置 Plant-OS 项目'
# ------------------------- 配置 CMake -------------------------

(
  enter build

  step '初始化 CMake 并进行编译测试'
  check_command gcc '需要使用 gcc 来编译项目' 'apt install gcc g++'
  check_command nasm '需要使用 nasm 来编译项目' 'apt install nasm'
  check_command mformat '需要使用 mformat 来格式化软盘' 'apt install mtools'
  check_command cmake '需要使用 cmake 来编译项目' 'apt install cmake extra-cmake-modules'
  check_command qemu-img '需要使用 qemu-img 来创建磁盘镜像' 'apt install qemu-utils'
  check_command objcopy '需要使用 objcopy 来处理二进制文件' 'apt install binutils'
  check cmake .. "$@"
  info 开始编译...
  check make make
)

if [ "$GITHUB_ACTIONS" = true ]; then
  step 'GitHub Actions 中跳过运行'
else
  (
    enter bin

    step '运行 Plant-OS'
    check_command qemu-system-i386 '需要使用 qemu-system-i386 来运行 Plant-OS' 'apt install qemu-system-i386'
    ./run.sh
  )
fi

if [ "$GITHUB_ACTIONS" = true ]; then
  info 'GitHub Actions 中拷贝 readme license 和 requirements 文件'
  run cp ./README.md bin/
  run cp ./LICENSE bin/
  run cp ./src/boot/requirements.txt bin/
fi

step 配置脚本结束
