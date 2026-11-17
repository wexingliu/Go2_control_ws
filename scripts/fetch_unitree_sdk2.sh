#!/bin/bash
# 用指定的本机 SDK2 安装目录替换工作区内的预编译版本。
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd -P)"
DEST="$ROOT/thirdparty/unitree_sdk2"
if [ "$#" -ne 1 ]; then
  echo "用法: $0 <Unitree SDK2 安装目录>" >&2
  echo "例如: $0 /path/to/unitree_sdk2/install" >&2
  echo "当前工作区已包含 SDK2，只有需要替换时才运行此脚本。" >&2
  exit 1
fi
if [ ! -d "$1" ]; then
  echo "找不到 SDK2 安装目录: $1" >&2
  exit 1
fi
SRC="$(cd "$1" && pwd -P)"
case "$SRC/" in
  "$DEST/"*) echo "源目录不能是工作区 SDK2 目录或其子目录: $SRC" >&2; exit 1 ;;
esac
if [ ! -f "$SRC/lib/cmake/unitree_sdk2/unitree_sdk2Config.cmake" ] || \
   [ ! -f "$SRC/lib/libddsc.so" ] || [ ! -f "$SRC/lib/libddscxx.so" ]; then
  echo "不是完整的 SDK2 安装目录: $SRC（缺少 CMake 配置或 DDS 库）" >&2
  exit 1
fi
mkdir -p "$(dirname "$DEST")"
STAGING="$(mktemp -d "$(dirname "$DEST")/.unitree_sdk2.XXXXXX")"
trap 'rm -rf "$STAGING"' EXIT
cp -a "$SRC/." "$STAGING/"
rm -rf "$DEST"
mv "$STAGING" "$DEST"
echo "已替换 SDK2: $DEST"
