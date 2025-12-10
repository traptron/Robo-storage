from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    """
    Главный launch-файл для запуска робо-тележки в симуляции Gazebo
    
    Запускает:
    - Gazebo с миром склада
    - URDF модель робота
    - SLAM Toolbox
    - RVIZ для визуализации
    """
    
    # Пакеты
    robo_description_pkg = FindPackageShare('robo_description')
    robo_simulation_pkg = FindPackageShare('robo_simulation')
    robo_slam_pkg = FindPackageShare('robo_slam')
    
    # Пути
    urdf_file = PathJoinSubstitution([robo_description_pkg, 'urdf', 'robot.urdf.xacro'])
    world_file = PathJoinSubstitution([robo_simulation_pkg, 'worlds', 'warehouse.world'])
    rviz_config = PathJoinSubstitution([robo_simulation_pkg, 'rviz', 'robot_view.rviz'])
    slam_params = PathJoinSubstitution([robo_slam_pkg, 'config', 'mapper_params_online_async.yaml'])
    
    # Аргументы
    use_sim_time = DeclareLaunchArgument(
        'use_sim_time',
        default_value='true',
        description='Использовать время симуляции'
    )
    
    # Robot State Publisher
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'robot_description': open(urdf_file.perform(None)).read()
        }]
    )
    
    # Gazebo
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('gazebo_ros'),
                'launch',
                'gazebo.launch.py'
            ])
        ]),
        launch_arguments={
            'world': world_file,
            'verbose': 'false'
        }.items()
    )
    
    # Spawn робота в Gazebo
    spawn_robot = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=[
            '-topic', 'robot_description',
            '-entity', 'robo_cart',
            '-x', '0.0',
            '-y', '0.0',
            '-z', '0.1'
        ],
        output='screen'
    )
    
    # SLAM Toolbox
    slam_toolbox = Node(
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox',
        output='screen',
        parameters=[
            slam_params,
            {'use_sim_time': LaunchConfiguration('use_sim_time')}
        ]
    )
    
    # RVIZ
    rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config],
        parameters=[{'use_sim_time': LaunchConfiguration('use_sim_time')}],
        output='screen'
    )
    
    return LaunchDescription([
        use_sim_time,
        robot_state_publisher,
        gazebo,
        spawn_robot,
        slam_toolbox,
        rviz,
    ])
