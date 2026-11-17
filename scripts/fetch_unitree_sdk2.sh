#!/bin/bash
# main 分支不含 44MB 的 Unitree SDK2 预编译产物，用本脚本还原。
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/src/quadruped_ros2_control/hardwares/hardware_unitree_mujoco/thirdparty/unitree_sdk2"
SRC="${1:-$HOME/dog/unitree_sdk2/install}"
if [ ! -d "$SRC" ]; then
  echo "找不到 SDK2 安装目录: $SRC" >&2
  echo "  git clone https://github.com/unitreerobotics/unitree_sdk2.git ~/dog/unitree_sdk2" >&2
  echo "  cmake -S ~/dog/unitree_sdk2 -B ~/dog/unitree_sdk2/build && cmake --build ~/dog/unitree_sdk2/build -j" >&2
  echo "  cmake --install ~/dog/unitree_sdk2/build --prefix ~/dog/unitree_sdk2/install" >&2
  exit 1
fi
rm -rf "$DEST"; mkdir -p "$(dirname "$DEST")"; cp -a "$SRC" "$DEST"
echo "已还原: $DEST"
