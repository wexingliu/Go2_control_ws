# Go2 ROS 2 Control

This workspace retains Go2 control with Unitree MuJoCo or the real robot. Both use the `hardware_unitree` DDS plugin. See the [guide controller README](controllers/unitree_guide_controller/README.md) for launch instructions and real-robot preparation.

```bash
colcon build --packages-up-to unitree_guide_controller go2_description keyboard_input --symlink-install
source install/setup.bash
# Start Unitree MuJoCo separately, configured for Go2, before this command.
ros2 launch unitree_guide_controller mujoco.launch.py
# For the real robot, use the Ethernet interface connected to Go2.
ros2 launch unitree_guide_controller real_robot.launch.py network_interface:=enp5s0
```

Gazebo and other robot descriptions are not included. OCS2 and RL controllers remain available for Go2 but require their own dependencies.
