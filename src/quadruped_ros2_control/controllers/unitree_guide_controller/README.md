# Unitree Guide Controller

This is a ros2-control controller based on unitree guide. The original unitree guide project could be
found [here](https://github.com/unitreerobotics/unitree_guide). I used KDL for the kinematic and dynamic calculation, so
the controller performance has difference with the original one (sometimes very unstable).

Tested environment:

* Ubuntu 24.04
    * ROS2 Jazzy
* Ubuntu 22.04
    * ROS2 Humble

[![](http://i1.hdslb.com/bfs/archive/310e6208920985ac43015b2da31c01ec15e2c5f9.jpg)](https://www.bilibili.com/video/BV1aJbAeZEuo/)

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

## 2. Build

```bash
cd ~/ros2_ws
colcon build --packages-up-to unitree_guide_controller
```

## 3. Launch

### 3.1 Mujoco Simulation
> **Warm Reminder**: You need to launch [Unitree Mujoco C++ Simulation](https://github.com/legubiao/unitree_mujoco) before launch the controller.
```bash
source ~/ros2_ws/install/setup.bash
ros2 launch unitree_guide_controller mujoco.launch.py pkg_description:=go2_description
```

### 3.2 Gazebo Classic 11 (ROS2 Humble)
```bash
source ~/ros2_ws/install/setup.bash
ros2 launch unitree_guide_controller gazebo_classic.launch.py pkg_description:=go2_description
```

### 3.3 Gazebo Harmonic (ROS2 Jazzy)
```bash
source ~/ros2_ws/install/setup.bash
ros2 launch unitree_guide_controller gazebo.launch.py pkg_description:=go2_description
```

### 3.4 Go2 real robot

Connect the computer to the robot over Ethernet. The real robot launch defaults to DDS domain `0` and interface `enp5s0`. The MuJoCo target defaults to domain `1` and loopback interface `lo`. Match these settings to your simulator or robot; they are independent of `ROS_DOMAIN_ID`.

```bash
source /opt/ros/jazzy/setup.bash
source ~/00u2/Go2_control_ws/install/setup.bash
ros2 launch unitree_guide_controller real_robot.launch.py network_interface:=enp5s0
```

The launch starts the hardware interface, state broadcasters, guide controller, then keyboard input. Press `2` once to begin the fixed-down transition, then press `2` again after about 1.5 seconds to enter fixed stand. Press `1` to return to passive mode. Keep the robot supported while first checking the controller and joint feedback.

Override the DDS settings or SDK library directory when needed:

```bash
ros2 launch unitree_guide_controller real_robot.launch.py domain:=0 network_interface:=enp5s0
# For a custom SDK library directory, use the direct real-robot launch:
ros2 launch unitree_guide_controller real_robot.launch.py sdk_library_path:=/path/to/sdk/lib
```
