# Hardware Unitree Mujoco

This package contains the hardware interface based on [unitree_sdk2](https://github.com/unitreerobotics/unitree_sdk2) to control the Unitree robot in Mujoco simulator. 

It also supports the real Go2 through the same DDS interface; real-robot operation must be validated on the target hardware. You can use Go2 simulation in [unitree_mujoco](https://github.com/legubiao/unitree_mujoco). In this simulation, I add foot force sensor support.

*[x] **[2025-01-16]** Add odometer states for simulation. 

## 1. Interfaces

Required hardware interfaces:

* command:
  * joint position
  * joint velocity
  * joint effort
  * KP
  * KD
* state:
  * joint effort
  * joint position
  * joint velocity
  * imu sensor
    * linear acceleration
    * angular velocity
    * orientation
  * foot force sensor

## 2. Build

Tested environment:
* Ubuntu 24.04
    * ROS2 Jazzy
* Ubuntu 22.04
    * ROS2 Humble

Build Command:
```bash
cd ~/ros2_ws
colcon build --packages-up-to hardware_unitree_mujoco --symlink-install
```

## 3. Go2: simulation and real robot

The same hardware plugin communicates with either Unitree MuJoCo or a real Go2 over DDS. Use the Go2 guide controller launch to select the target; there is no need to edit the xacro:

```bash
source ~/00u2/Go2_control_ws/install/setup.bash
# Start the Unitree MuJoCo simulator separately first.
ros2 launch unitree_guide_controller mujoco.launch.py pkg_description:=go2_description

# Real Go2, connected by Ethernet (replace with the actual interface).
ros2 launch unitree_guide_controller real_robot.launch.py network_interface:=enp5s0
```

MuJoCo defaults to DDS domain `1` and interface `lo`; the real robot defaults to domain `0` and `enp5s0`. Set `domain:=...` and `network_interface:=...` to match the simulator or robot configuration. These are Unitree SDK DDS settings, distinct from `ROS_DOMAIN_ID`. The real launch sends low-level commands: support the robot and check feedback and controller state before enabling motion. Consult the controller README for the input sequence and simulator prerequisites.

The real-robot launch also supports an `sdk_library_path` override. The installed hardware plugin normally resolves its bundled DDS libraries automatically.
