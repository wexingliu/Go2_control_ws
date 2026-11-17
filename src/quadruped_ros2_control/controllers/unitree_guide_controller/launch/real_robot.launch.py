import os

import xacro
from ament_index_python.packages import get_package_prefix, get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction, RegisterEventHandler
from launch.event_handlers import OnProcessExit
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def launch_setup(context, *args, **kwargs):
    domain = LaunchConfiguration("domain").perform(context)
    network_interface = LaunchConfiguration("network_interface").perform(context)
    sdk_library_path = LaunchConfiguration("sdk_library_path").perform(context)

    description_share = get_package_share_directory("go2_description")
    xacro_file = os.path.join(description_share, "xacro", "robot.xacro")
    robot_description = xacro.process_file(
        xacro_file,
        mappings={"domain": domain, "network_interface": network_interface},
    ).toxml()
    controller_config = os.path.join(description_share, "config", "robot_control.yaml")
    guide_config = os.path.join(
        get_package_share_directory("unitree_guide_controller"),
        "config",
        "go2_real_robot.yaml",
    )

    existing_library_path = os.environ.get("LD_LIBRARY_PATH", "")
    library_path = os.pathsep.join(
        path for path in (sdk_library_path, existing_library_path) if path
    )

    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="screen",
        parameters=[{"robot_description": robot_description}],
    )

    controller_manager = Node(
        package="controller_manager",
        executable="ros2_control_node",
        name="controller_manager",
        output="both",
        parameters=[controller_config],
        remappings=[("~/robot_description", "/robot_description")],
        additional_env={"LD_LIBRARY_PATH": library_path},
    )

    joint_state_broadcaster = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
        output="screen",
    )

    guide_controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "unitree_guide_controller",
            "--controller-manager",
            "/controller_manager",
            "--param-file",
            guide_config,
        ],
        output="screen",
    )

    keyboard_input = Node(
        package="keyboard_input",
        executable="keyboard_input",
        name="keyboard_input_node",
        output="screen",
        emulate_tty=True,
    )

    return [
        robot_state_publisher,
        controller_manager,
        joint_state_broadcaster,
        RegisterEventHandler(
            OnProcessExit(target_action=joint_state_broadcaster, on_exit=[guide_controller])
        ),
        RegisterEventHandler(
            OnProcessExit(target_action=guide_controller, on_exit=[keyboard_input])
        ),
    ]


def generate_launch_description():
    return LaunchDescription(
        [
            DeclareLaunchArgument("domain", default_value="0", description="Unitree DDS domain ID"),
            DeclareLaunchArgument(
                "network_interface",
                default_value="enp5s0",
                description="Ethernet interface connected to Go2",
            ),
            DeclareLaunchArgument(
                "sdk_library_path",
                default_value=os.path.join(get_package_prefix("hardware_unitree_mujoco"), "lib"),
                description="Directory containing libddsc.so and libddscxx.so "
                "(defaults to the hardware plugin's own lib dir, which ships them)",
            ),
            OpaqueFunction(function=launch_setup),
        ]
    )
