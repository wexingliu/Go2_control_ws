# 本仓库 main 分支相对上游的改动

上游基线：`quadruped_ros2_control` @ a808acb。

## 1. hardware_unitree_mujoco（硬件接口）

- `CMakeLists.txt`
  - Unitree SDK2 改为**包内查找**：`thirdparty/unitree_sdk2/`，强制 `unitree_sdk2_DIR` 指向它；
    若要用系统 SDK，配置 `-DUNITREE_SDK2_PREFER_SYSTEM=ON`。
  - 运行库 `libddsc.so` / `libddscxx.so` 以 `libddsc.so.0` / `libddscxx.so.0` 装进插件自己的 `lib/`，
    并设 `RUNPATH=$ORIGIN`，运行时不再需要 `LD_LIBRARY_PATH`。
- `include/hardware_unitree_mujoco/HardwareUnitree.h`
  - 清理临时 UDP 传输桩代码，恢复为纯 DDS 实现。

## 2. descriptions/unitree/go2_description（模型与 ros2_control 接口）

- `xacro/robot.xacro`、`xacro/ros2_control.xacro`：移除临时的 `transport` / `robot_ip` 参数。

## 3. controllers/unitree_guide_controller（控制器与真机启动）

- `launch/real_robot.launch.py`：新增真机启动文件；`sdk_library_path` 默认值改为
  `get_package_prefix("hardware_unitree_mujoco")/lib`（插件自带 DDS 运行库），不再指向 `/tmp`。
- `config/go2_real_robot.yaml`：Go2 真机 FSM 参数（`down_pos` 收腿位、关节顺序 FR/FL/RR/RL 等）。

## 已验证的真机链路

```
电脑(ROS 2 Humble) --有线 enp5s0--> 机器人 DDS 域 0（无安全插件）
  rt/lowstate  ~500 Hz
  rt/lowcmd    200 Hz（FIXEDDOWN / FIXEDSTAND 实测驱动关节）
```

实测：`FIXEDSTAND` 后关节收敛到 (0, 0.67, -1.30)，膝盖力矩约 7.2 Nm 承重，pitch ≈ −0.03 rad，
连续 9 秒数值不变。

## 前置条件（不在本仓库内）

- 机器人需完成 Go2 AIR 的"二次开发"解锁（社区 patched 服务），否则低层帧会被 MCU 丢弃
- `sport_mode` 不得同时占用控制权
- 构建时若默认 `python3` 指向 conda，需加 `--cmake-args -DPython3_EXECUTABLE=/usr/bin/python3`
