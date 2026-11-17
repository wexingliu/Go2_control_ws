# Go2_control_ws

Go2 四足 ROS 2 控制工作区，只包含 `src/quadruped_ros2_control/`。

## 分支

| 分支 | 内容 |
|---|---|
| `master` | 原始源代码（上游 `quadruped_ros2_control` @ a808acb，未修改） |
| `main` | 修改后的代码：本机跑通真机控制所需的改动 |

## 说明

- 为控制仓库体积，剔除了 4 个 >20MB 的**非 Go2 机型**网格文件（go1/b2 的 `trunk.dae`）；Go2 自身网格保留。
- `main` 不含 `hardwares/hardware_unitree_mujoco/thirdparty/unitree_sdk2/`（44MB 的 Unitree SDK2 预编译产物），
  用 `scripts/fetch_unitree_sdk2.sh` 还原。
- `main` 的改动清单见 `src/quadruped_ros2_control/README_CHANGES.md`。
