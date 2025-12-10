#ifndef ROBO_HARDWARE__ROBO_HARDWARE_INTERFACE_HPP_
#define ROBO_HARDWARE__ROBO_HARDWARE_INTERFACE_HPP_

#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/state.hpp"

namespace robo_hardware
{

/**
 * @brief Hardware интерфейс для робо-тележки с ESP32
 * 
 * Этот класс реализует ros2_control SystemInterface для управления
 * дифференциальным приводом через micro-ROS на ESP32.
 * 
 * В симуляции (Gazebo) использует gazebo_ros2_control.
 * На реальном роботе общается с ESP32 через micro-ROS топики.
 */
class RoboHardwareInterface : public hardware_interface::SystemInterface
{
public:
  RCLCPP_SHARED_PTR_DEFINITIONS(RoboHardwareInterface)

  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  hardware_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override;

  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::return_type read(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  hardware_interface::return_type write(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

private:
  // Параметры конфигурации
  std::string robot_name_;
  bool use_simulation_;

  // Состояние колес (позиция и скорость)
  std::vector<double> hw_positions_;
  std::vector<double> hw_velocities_;
  std::vector<double> hw_commands_;

  // Имена суставов (колес)
  std::vector<std::string> joint_names_;

  // ROS2 node для связи с micro-ROS (на реальном роботе)
  rclcpp::Node::SharedPtr node_;
};

}  // namespace robo_hardware

#endif  // ROBO_HARDWARE__ROBO_HARDWARE_INTERFACE_HPP_
