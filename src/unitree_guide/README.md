# Unitree Guide：ROS 2 + Go2 DDS

此目录将原版 Unitree Guide 控制算法接入 ROS 2 和 Go2。控制器使用 Unitree SDK2 直接通过 DDS 读取 `rt/lowstate`、发布 `rt/lowcmd`，关节顺序为 FR、FL、RR、RL。ROS 2 节点用于配置网络参数；低层控制报文不是 ROS 话题。

Go2 的腿长及关节位置来自相邻 `Go2_control_ws` 的机器人描述；质量根据该描述的各连杆质量求和，整机惯量暂沿用 Go1 的近似值，小跑增益也沿用原 Go1 参数，均尚未在 Go2 上验证或调优。

## 构建

需要 ROS 2 Jazzy、colcon、Eigen3 和 Unitree SDK2。默认使用相邻 `Go2_control_ws/thirdparty/unitree_sdk2` 的 SDK 和 DDS 库；可用 `-DUNITREE_SDK2_ROOT=/path/to/sdk/install` 指定其他安装目录。以下命令在工作区根目录执行：

```bash
source /opt/ros/jazzy/setup.bash
colcon build --packages-select unitree_guide --cmake-args -DPython3_EXECUTABLE=/usr/bin/python3
source install/setup.bash
```

## 运行

确保高层运动服务不占用控制权，机器人的低层控制已启用，并先固定或支撑机器人。使用连接 Go2 的实际网卡名：

```bash
ros2 run unitree_guide junior_ctrl --ros-args -p network_interface:=enp5s0 -p domain:=0
```

`network_interface` 和 `domain` 是 Unitree SDK2 DDS 的网卡和域，与 `ROS_DOMAIN_ID` 无关。连接 MuJoCo DDS 仿真时应先启动兼容的仿真器，并将参数设为其配置，例如 `network_interface:=lo` 和 `domain:=1`。本程序目标周期为 500 Hz，尝试启用实时调度；权限不足时会提示并继续运行。

控制程序需要前台终端接收键盘输入：`2` 从 Passive 切换至站立，`1` 返回 Passive，站立后 `4` 进入小跑；`w/a/s/d` 增减平移速度，`j/l` 增减转向速度，空格清零速度。该流程与参考工程的两次按 `2` 流程不同。

启动时等待状态最多 10 秒，运行时状态超过 100 ms 未更新或检测到非有限数据会停止发命令并退出。**停止发命令不等于已经确认机器人安全停止**；请使用机器人既有的安全措施。Go2 实机和小跑行为尚未由本次移植验证，应先在仿真及受保护条件下逐步测试，不要将参考工程实测结果视为本控制器的验证结果。
