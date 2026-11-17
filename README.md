# Go2_control_ws

Go2 四足 ROS 2 控制工作区。源码位于 `src/quadruped_ros2_control/`，Unitree SDK2 位于 `thirdparty/unitree_sdk2/`。

## 分支

| 分支 | 内容 |
|---|---|
| `master` | 原始源代码（上游 `quadruped_ros2_control` @ a808acb，未修改） |
| `main` | 修改后的代码：本机跑通真机控制所需的改动 |
| `unitree_guide` | 在 main 的基础上新增 ROS 2 与 Go2 DDS 版 Unitree Guide，详见 `src/unitree_guide/README.md` |

## 说明

- 项目保留 Go2 的 MuJoCo 仿真和实机控制，其他机型描述与 Gazebo 专用组件已移除。
- `main` 包含 44MB 的 Unitree SDK2 预编译产物，无需单独下载。需要替换为本机 SDK 时，可用
  `scripts/fetch_unitree_sdk2.sh /实际的/unitree_sdk2/install`，将指定的 SDK 安装目录复制到工作区根目录的 `thirdparty/unitree_sdk2/`。该操作会替换工作区已有的 SDK。
- `main` 的改动清单见 `src/quadruped_ros2_control/README_CHANGES.md`。
