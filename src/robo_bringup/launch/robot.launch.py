from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    """
    Launch-файл для запуска робо-тележки на реальном оборудовании
    
    Запускает:
    - URDF модель робота
    - Hardware interface (связь с ESP32 через micro-ROS)
    - RPLIDAR драйвер
    - SLAM Toolbox
    - Nav2 (опционально)
    """
    
    # Пакеты
    robo_description_pkg = FindPackageShare('robo_description')
    robo_sensors_pkg = FindPackageShare('robo_sensors')
    robo_slam_pkg = FindPackageShare('robo_slam')
    
    # Пути
    urdf_file = PathJoinSubstitution([robo_description_pkg, 'urdf', 'robot.urdf.xacro'])
    slam_params = PathJoinSubstitution([robo_slam_pkg, 'config', 'mapper_params_online_async.yaml'])
    
    # Аргументы
    serial_port = DeclareLaunchArgument(
        'serial_port',
        default_value='/dev/ttyUSB0',
        description='Порт для подключения RPLIDAR'
    )
    
    # Robot State Publisher
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'use_sim_time': False,
            'robot_description': open(urdf_file.perform(None)).read()
        }]
    )
    
    # RPLIDAR launch
    rplidar_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                robo_sensors_pkg,
                'launch',
                'rplidar.launch.py'
            ])
        ]),
        launch_arguments={
            'serial_port': LaunchConfiguration('serial_port'),
            'use_sim': 'false'
        }.items()
    )
    
    # SLAM Toolbox
    slam_toolbox = Node(
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox',
        output='screen',
        parameters=[
            slam_params,
            {'use_sim_time': False}
        ]
    )
    
    return LaunchDescription([
        serial_port,
        robot_state_publisher,
        rplidar_launch,
        slam_toolbox,
    ])
