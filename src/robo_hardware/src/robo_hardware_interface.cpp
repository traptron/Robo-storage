#include "robo_hardware/robo_hardware_interface.hpp"

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"

namespace robo_hardware
{

hardware_interface::CallbackReturn RoboHardwareInterface::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (
    hardware_interface::SystemInterface::on_init(info) !=
    hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }

  robot_name_ = info_.name;
  
  // Проверка на режим симуляции
  auto it = info_.hardware_parameters.find("use_simulation");
  use_simulation_ = (it != info_.hardware_parameters.end() && it->second == "true");

  // Инициализация векторов состояния
  hw_positions_.resize(info_.joints.size(), 0.0);
  hw_velocities_.resize(info_.joints.size(), 0.0);
  hw_commands_.resize(info_.joints.size(), 0.0);

  // Сохранение имен суставов
  for (const hardware_interface::ComponentInfo & joint : info_.joints)
  {
    joint_names_.push_back(joint.name);
  }

  RCLCPP_INFO(
    rclcpp::get_logger("RoboHardwareInterface"),
    "Инициализация завершена для робота '%s' (симуляция: %s)",
    robot_name_.c_str(),
    use_simulation_ ? "да" : "нет");

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn RoboHardwareInterface::on_configure(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  RCLCPP_INFO(rclcpp::get_logger("RoboHardwareInterface"), "Конфигурация интерфейса...");

  if (!use_simulation_)
  {
    // Для реального робота: инициализация связи с ESP32 через micro-ROS
    // TODO: Настроить подписку на топики одометрии от ESP32
    // TODO: Настроить публикацию команд скорости к ESP32
    RCLCPP_INFO(
      rclcpp::get_logger("RoboHardwareInterface"),
      "Режим реального робота: ожидание подключения к ESP32 через micro-ROS");
  }
  else
  {
    RCLCPP_INFO(
      rclcpp::get_logger("RoboHardwareInterface"),
      "Режим симуляции: используется Gazebo");
  }

  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface>
RoboHardwareInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;
  
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    state_interfaces.emplace_back(hardware_interface::StateInterface(
      info_.joints[i].name, hardware_interface::HW_IF_POSITION, &hw_positions_[i]));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
      info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_velocities_[i]));
  }

  return state_interfaces;
}

std::vector<hardware_interface::CommandInterface>
RoboHardwareInterface::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;
  
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
      info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_commands_[i]));
  }

  return command_interfaces;
}

hardware_interface::CallbackReturn RoboHardwareInterface::on_activate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  RCLCPP_INFO(rclcpp::get_logger("RoboHardwareInterface"), "Активация интерфейса...");

  // Инициализация начальных значений
  for (size_t i = 0; i < hw_positions_.size(); i++)
  {
    hw_positions_[i] = 0.0;
    hw_velocities_[i] = 0.0;
    hw_commands_[i] = 0.0;
  }

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn RoboHardwareInterface::on_deactivate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  RCLCPP_INFO(rclcpp::get_logger("RoboHardwareInterface"), "Деактивация интерфейса...");
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type RoboHardwareInterface::read(
  const rclcpp::Time & /*time*/, const rclcpp::Duration & period)
{
  // Чтение состояния от оборудования
  
  if (use_simulation_)
  {
    // В симуляции состояние обновляется Gazebo
    // Здесь можно добавить дополнительную логику обработки
  }
  else
  {
    // Для реального робота: чтение данных от ESP32
    // TODO: Получить данные одометрии от micro-ROS топиков
    // TODO: Обновить hw_positions_ и hw_velocities_
    
    // Пока используем простое интегрирование для тестирования
    for (size_t i = 0; i < hw_positions_.size(); i++)
    {
      hw_positions_[i] += hw_velocities_[i] * period.seconds();
    }
  }

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type RoboHardwareInterface::write(
  const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
  // Отправка команд на оборудование
  
  if (use_simulation_)
  {
    // В симуляции команды отправляются в Gazebo
    for (size_t i = 0; i < hw_commands_.size(); i++)
    {
      hw_velocities_[i] = hw_commands_[i];
    }
  }
  else
  {
    // Для реального робота: отправка команд на ESP32
    // TODO: Публиковать hw_commands_ в micro-ROS топики для ESP32
    
    // Временная реализация для тестирования
    for (size_t i = 0; i < hw_commands_.size(); i++)
    {
      hw_velocities_[i] = hw_commands_[i];
    }
  }

  return hardware_interface::return_type::OK;
}

}  // namespace robo_hardware

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(
  robo_hardware::RoboHardwareInterface, hardware_interface::SystemInterface)
