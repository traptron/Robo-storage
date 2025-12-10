cmake_minimum_required(VERSION 3.8)
project(robo_hardware)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Найти зависимости
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(controller_interface REQUIRED)
find_package(pluginlib REQUIRED)

# Включить директории заголовков
include_directories(include)

# Создать библиотеку hardware interface
add_library(${PROJECT_NAME}_system_interface SHARED
  src/robo_hardware_interface.cpp
)

target_include_directories(${PROJECT_NAME}_system_interface PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(${PROJECT_NAME}_system_interface
  rclcpp
  hardware_interface
  pluginlib
)

# Экспорт hardware interface плагина
pluginlib_export_plugin_description_file(hardware_interface robo_hardware_interface_plugin.xml)

# Установка
install(
  TARGETS ${PROJECT_NAME}_system_interface
  DESTINATION lib
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  DIRECTORY config launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME}_system_interface)
ament_export_dependencies(
  rclcpp
  hardware_interface
  pluginlib
)

ament_package()
