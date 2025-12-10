from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    """
    Launch-файл для запуска RPLIDAR A2M8
    
    Параметры:
        serial_port: Порт для подключения LIDAR (по умолчанию /dev/ttyUSB0)
        use_sim: Использовать ли симуляцию (по умолчанию false)
    """
    
    # Путь к конфигурационному файлу
    config_dir = get_package_share_directory('robo_sensors')
    rplidar_config = os.path.join(config_dir, 'config', 'rplidar.yaml')
    
    # Объявление аргументов
    serial_port_arg = DeclareLaunchArgument(
        'serial_port',
        default_value='/dev/ttyUSB0',
        description='Последовательный порт для RPLIDAR'
    )
    
    use_sim_arg = DeclareLaunchArgument(
        'use_sim',
        default_value='false',
        description='Использовать симуляцию вместо реального LIDAR'
    )
    
    # Узел RPLIDAR (запускается только если use_sim=false)
    rplidar_node = Node(
        package='rplidar_ros',
        executable='rplidar_composition',
        name='rplidar_node',
        parameters=[rplidar_config, {
            'serial_port': LaunchConfiguration('serial_port'),
        }],
        output='screen',
        condition=lambda context: context.launch_configurations['use_sim'] == 'false'
    )
    
    return LaunchDescription([
        serial_port_arg,
        use_sim_arg,
        rplidar_node,
    ])
