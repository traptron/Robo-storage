# Firmware ESP32 для робо-тележки с micro-ROS

Этот скетч Arduino для ESP32 управляет моторами, читает энкодеры, вычисляет одометрию и публикует её через micro-ROS в ROS2.

## Аппаратное обеспечение

- **Контроллер**: ESP32 DevKit v1 или аналог
- **Драйвер моторов**: L298N, BTS7960 или аналог
- **Моторы**: 2x DC мотора с энкодерами (например, JGA25-370 с энкодерами)
- **Энкодеры**: 2x инкрементальные энкодеры (A/B каналы)
- **Питание**: 7.4V-12V для моторов, 5V для ESP32

## Подключение пинов

```
ESP32 Pin | Компонент
----------|----------
GPIO 16   | Мотор левый PWM
GPIO 17   | Мотор левый DIR
GPIO 18   | Мотор правый PWM
GPIO 19   | Мотор правый DIR
GPIO 34   | Энкодер левый A (только вход!)
GPIO 35   | Энкодер левый B (только вход!)
GPIO 36   | Энкодер правый A (только вход!)
GPIO 39   | Энкодер правый B (только вход!)
```

## Установка зависимостей

### 1. Установить Arduino IDE (версия 2.x)

### 2. Установить micro-ROS для Arduino

```bash
# В Arduino IDE:
# Sketch -> Include Library -> Manage Libraries
# Найти и установить: "micro_ros_arduino"
```

### 3. Настроить ESP32 board manager

```
# File -> Preferences -> Additional Board Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# Tools -> Board -> Boards Manager
# Найти и установить: "esp32 by Espressif Systems"
```

## Компиляция и загрузка

1. Открыть `esp32_robot.ino` в Arduino IDE
2. Выбрать плату: **Tools -> Board -> ESP32 Dev Module**
3. Настроить параметры:
   - Upload Speed: 115200
   - CPU Frequency: 240MHz
   - Flash Frequency: 80MHz
   - Flash Mode: QIO
   - Flash Size: 4MB
   - Partition Scheme: Default 4MB with spiffs
4. Подключить ESP32 через USB
5. Выбрать порт: **Tools -> Port -> COMx** (Windows) или **/dev/ttyUSBx** (Linux)
6. Нажать Upload

## Настройка micro-ROS Agent

На компьютере с ROS2 Jazzy:

```bash
# Установить micro-ROS agent
sudo apt install ros-jazzy-micro-ros-agent

# Запустить agent (замените на ваш порт)
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0 -b 115200
```

## Проверка работы

После загрузки firmware и запуска micro-ROS agent:

```bash
# Проверить топики
ros2 topic list

# Должны появиться:
# /odom
# /cmd_vel

# Проверить одометрию
ros2 topic echo /odom

# Отправить команду движения
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.2, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}" --once
```

## Калибровка

### Параметры робота (настроить в коде):

```cpp
const float WHEEL_RADIUS = 0.065;        // Радиус колеса (м)
const float WHEEL_SEPARATION = 0.35;     // Расстояние между колесами (м)
const int ENCODER_TICKS_PER_REV = 1440;  // Импульсов энкодера на оборот
```

### Определение направления моторов:

1. Запустить робота с командой `linear.x > 0`
2. Если робот едет назад, инвертировать `LEFT_MOTOR_DIR` или `RIGHT_MOTOR_DIR`
3. Если робот поворачивает, поменять местами моторы

## Отладка

Для отладки включить Serial Monitor (115200 baud). Firmware выводит:
- Статус подключения к micro-ROS agent
- Значения энкодеров
- Вычисленную одометрию
- Команды скорости

## Решение проблем

**Проблема**: ESP32 не подключается к micro-ROS agent
- Проверить правильность порта и скорости
- Перезагрузить ESP32
- Проверить USB кабель (должен поддерживать данные)

**Проблема**: Моторы не вращаются
- Проверить питание драйвера моторов
- Проверить подключение пинов PWM и DIR
- Проверить команды `ros2 topic pub /cmd_vel`

**Проблема**: Одометрия неправильная
- Калибровать параметры WHEEL_RADIUS и WHEEL_SEPARATION
- Проверить количество импульсов энкодера ENCODER_TICKS_PER_REV
- Проверить направление счёта энкодеров

## Лицензия

MIT License
