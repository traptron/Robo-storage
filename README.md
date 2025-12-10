# Robo-Storage: Система автономного склада

![ROS2 Jazzy](https://img.shields.io/badge/ROS2-Jazzy-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Status](https://img.shields.io/badge/status-В%20разработке-yellow)

Проект автономной складской системы для маркетплейсов на базе ROS2 Jazzy с робо-тележками на ESP32 и micro-ROS.

## 📋 Описание

**Robo-Storage** — это open-source решение для автоматизации складской логистики с использованием автономных роботов. Роботы доставляют шкафы с ячейками к конвейеру, где манипуляторы загружают/разгружают товары.

### Ключевые особенности

- 🤖 **Автономная навигация** — SLAM Toolbox + Nav2
- 💰 **Низкая стоимость** — ESP32 + RPLIDAR A2M8 (~$350 на робота)
- ⚡ **Высокая производительность** — дифференциальный привод, скорость до 0.5 м/с
- 🔧 **Модульность** — легко масштабируется и адаптируется
- 🐍 **Гибкость разработки** — C++ для критичных частей, Python для логики

## 🏗️ Архитектура системы

### Структура пакетов

```
Robo-storage/
├── src/
│   ├── robo_core/              # Метапакет (зависимости)
│   ├── robo_hardware/          # Управление ESP32 (моторы, одометрия)
│   ├── robo_sensors/           # RPLIDAR A2M8 конфигурация
│   ├── robo_description/       # URDF модель робота
│   ├── robo_slam/              # SLAM Toolbox настройки
│   ├── robo_navigation/        # Nav2 параметры
│   ├── robo_simulation/        # Gazebo мир склада
│   └── robo_bringup/           # Launch-файлы
├── firmware/
│   └── esp32_robot/            # micro-ROS прошивка для ESP32
├── ros2.repos                  # Зависимости для vcs
└── README.md
```

### Технологический стек

| Компонент | Технология |
|-----------|-----------|
| **ROS2** | Jazzy (LTS) |
| **Контроллер** | ESP32 + micro-ROS |
| **LIDAR** | RPLIDAR A2M8 (10Hz, 16м дальность) |
| **SLAM** | SLAM Toolbox (sync mode) |
| **Навигация** | Nav2 (SMAC Planner + Regulated Pure Pursuit) |
| **Симуляция** | Gazebo Classic |
| **Языки** | C++ (моторы, SLAM), Python (управление, запуск) |

## 🚀 Быстрый старт

### Требования

- **OS**: Ubuntu 24.04 (рекомендуется) или Ubuntu 22.04
- **ROS2**: Jazzy Jalisco
- **Python**: 3.10+
- **Память**: минимум 8GB RAM
- **Место**: ~10GB свободного места

### 1. Установка ROS2 Jazzy

```bash
# Добавить ROS2 репозиторий
sudo apt update && sudo apt install software-properties-common
sudo add-apt-repository universe
sudo apt update && sudo apt install curl -y
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg

echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

# Установить ROS2 Jazzy Desktop
sudo apt update
sudo apt install ros-jazzy-desktop
```

### 2. Клонирование и установка зависимостей

```bash
# Создать workspace
mkdir -p ~/robo_ws/src
cd ~/robo_ws/src
git clone https://github.com/your-username/Robo-storage.git

# Установить зависимости через vcs
cd ~/robo_ws
vcs import src < src/Robo-storage/ros2.repos

# Установить системные зависимости
sudo apt install python3-rosdep
sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src -r -y

# Установить дополнительные инструменты
sudo apt install python3-colcon-common-extensions
```

### 3. Сборка проекта

```bash
cd ~/robo_ws
source /opt/ros/jazzy/setup.bash
colcon build --symlink-install

# Добавить в bashrc для автозагрузки
echo "source ~/robo_ws/install/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

### 4. Запуск симуляции

```bash
# Запустить Gazebo с роботом и SLAM
ros2 launch robo_bringup simulation.launch.py

# В другом терминале: управление клавиатурой
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

## 🔧 Прошивка ESP32

Для реального робота нужно загрузить firmware на ESP32. Подробная инструкция: [firmware/esp32_robot/README.md](firmware/esp32_robot/README.md)

**Краткая инструкция:**

1. Установить Arduino IDE 2.x
2. Установить `micro_ros_arduino` библиотеку
3. Открыть `firmware/esp32_robot/esp32_robot.ino`
4. Настроить пины моторов и энкодеров
5. Загрузить на ESP32 (Tools → Upload)
6. Запустить micro-ROS agent:
   ```bash
   ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0 -b 115200
   ```

## 📊 Roadmap развития

### ✅ Фаза 1: Базовая мобильность (Недели 1-2)

- [x] Управление моторами через ESP32
- [x] Одометрия от энкодеров
- [x] Интеграция micro-ROS
- [x] Телеуправление
- [ ] **Тестирование на реальном роботе**

### 🔄 Фаза 2: Восприятие окружения (Недели 3-4)

- [x] Драйвер RPLIDAR A2M8
- [x] Симуляция в Gazebo
- [ ] Калибровка LIDAR
- [ ] IMU интеграция (опционально)

### 📍 Фаза 3: SLAM и картографирование (Недели 5-6)

- [x] SLAM Toolbox конфигурация
- [ ] Создание карты склада
- [ ] Loop closure оптимизация
- [ ] Сохранение/загрузка карт

### 🗺️ Фаза 4: Локализация (Неделя 7)

- [ ] SLAM Toolbox localization mode
- [ ] Sensor fusion (одометрия + LIDAR + IMU)
- [ ] Тестирование точности позиционирования

### 🎯 Фаза 5: Автономная навигация (Недели 8-9)

- [x] Nav2 интеграция
- [ ] Планирование пути между шкафами
- [ ] Избегание динамических препятствий
- [ ] Recovery behaviors

### 🏭 Фаза 6: Интеграция с системой склада (Недели 10+)

- [ ] API для отправки задач роботам
- [ ] Fleet management (управление парком роботов)
- [ ] Интеграция с манипуляторами
- [ ] Web-интерфейс мониторинга

## 🛠️ Аппаратное обеспечение

### Робо-тележка (примерная стоимость: $350)

| Компонент | Модель | Цена (USD) |
|-----------|--------|------------|
| Контроллер | ESP32 DevKit v1 | $5 |
| LIDAR | RPLIDAR A2M8 | $220 |
| Моторы | 2x DC мотор с энкодером | $40 |
| Драйвер моторов | L298N или BTS7960 | $10 |
| Шасси | Алюминиевая платформа | $30 |
| Батарея | LiPo 3S 5000mAh | $35 |
| Прочее | Провода, крепёж, колёса | $10 |

### Геометрия робота

- **Размеры**: 0.5м (Д) × 0.4м (Ш) × 0.3м (В)
- **Колёсная база**: 0.35м
- **Радиус колеса**: 0.065м
- **Макс. скорость**: 0.5 м/с
- **Макс. угловая скорость**: 1.5 рад/с

## 📚 Документация

- [Инструкция по сборке робота](docs/assembly.md) *(coming soon)*
- [Руководство по калибровке](docs/calibration.md) *(coming soon)*
- [API документация](docs/api.md) *(coming soon)*
- [FAQ и решение проблем](docs/faq.md) *(coming soon)*

## 🤝 Вклад в проект

Мы приветствуем любой вклад! Пожалуйста:

1. Fork репозиторий
2. Создайте feature ветку (`git checkout -b feature/amazing-feature`)
3. Commit изменения (`git commit -m 'Add amazing feature'`)
4. Push в ветку (`git push origin feature/amazing-feature`)
5. Откройте Pull Request

### Структура веток

- `main` — стабильная версия
- `dev` — разработка
- `feature/phase1-mobility` — Фаза 1 (мобильность)
- `feature/phase2-perception` — Фаза 2 (восприятие)
- и т.д.

## 📝 Лицензия

Распространяется под лицензией MIT. См. `LICENSE` для подробностей.

## 🙏 Благодарности

- [ROS2 Jazzy](https://docs.ros.org/en/jazzy/)
- [SLAM Toolbox](https://github.com/SteveMacenski/slam_toolbox)
- [Navigation2](https://github.com/ros-planning/navigation2)
- [micro-ROS](https://micro.ros.org/)
- [SLAMTEC RPLIDAR](https://github.com/Slamtec/rplidar_ros)

## 📧 Контакты

- **Email**: trapmpage@gmail.com
- **Telegram**: @trapmpage
- **GitHub Issues**: [Создать issue](https://github.com/traptron/Robo-storage/issues)

---

**⭐ Поставьте звезду, если проект вам полезен!**
