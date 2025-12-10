from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    """Запуск LIDAR LDS (от робота-пылесоса, LDS-01/02) через hls_lfcd_lds_driver."""

    config_dir = get_package_share_directory('robo_sensors')
    lds_config = os.path.join(config_dir, 'config', 'lds.yaml')

    serial_port_arg = DeclareLaunchArgument(
        'serial_port',
        default_value='/dev/ttyUSB0',
        description='Последовательный порт для LDS'
    )

    lds_node = Node(
        package='hls_lfcd_lds_driver',
        executable='hlds_laser_publisher',
        name='lds_driver',
        parameters=[lds_config, {
            'port': LaunchConfiguration('serial_port'),
        }],
        output='screen'
    )

    return LaunchDescription([
        serial_port_arg,
        lds_node,
    ])
