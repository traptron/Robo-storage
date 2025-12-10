# Docker для Robo-Storage

Dockerfile для быстрого развертывания окружения ROS2 Jazzy с предустановленными зависимостями проекта.

## 📋 Требования

- Docker (19.03+)
- Docker Compose (1.29+)
- 10+ GB свободного места
- 4+ GB RAM

## 🚀 Быстрый старт

### Вариант 1: Использование docker-compose (рекомендуется)

```bash
# Клонировать репозиторий
git clone https://github.com/your-username/Robo-storage.git
cd Robo-storage

# Собрать и запустить контейнер
docker-compose up -d

# Подключиться к контейнеру
docker-compose exec robo-storage bash

# Проверить установку ROS2
source ~/.bashrc
ros2 --version
```

### Вариант 2: Использование docker напрямую

```bash
# Собрать образ
docker build -t robo-storage:jazzy-latest .

# Запустить контейнер
docker run -it \
  --name robo-storage-dev \
  --volume $(pwd)/src:/root/robo_ws/src \
  --volume $(pwd)/build:/root/robo_ws/build \
  --volume $(pwd)/install:/root/robo_ws/install \
  --volume $(pwd)/log:/root/robo_ws/log \
  --device /dev/ttyUSB0 \
  robo-storage:jazzy-latest

# В контейнере
source ~/.bashrc
ros2 launch robo_bringup simulation.launch.py
```

## 🔧 Конфигурация

### Пробросить USB (для LIDAR и ESP32)

**На Linux:**
```bash
# docker-compose.yml уже содержит:
devices:
  - /dev/ttyUSB0:/dev/ttyUSB0
  - /dev/ttyUSB1:/dev/ttyUSB1
```

**На Windows/Mac с Docker Desktop:**
```bash
# Включить доступ к USB в Docker Desktop Settings
# Settings -> Resources -> File Sharing -> Add /dev
```

### Включить GUI (X11 forwarding)

**На Linux:**
```bash
# Раскомментировать в docker-compose.yml:
volumes:
  - /tmp/.X11-unix:/tmp/.X11-unix
  - ~/.Xauthority:/root/.Xauthority:ro

# Запустить
docker-compose up -d
docker-compose exec robo-storage rviz2
```

**На Mac (с XQuartz):**
```bash
# Установить XQuartz
brew install xquartz

# Позволить подключения от сети
xhost + 127.0.0.1

# Запустить docker с X11
docker run -it \
  -e DISPLAY=host.docker.internal:0 \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  robo-storage:jazzy-latest
```

### GPU поддержка (NVIDIA)

**Если у вас NVIDIA GPU:**

1. Установить NVIDIA Docker runtime:
```bash
distribution=$(. /etc/os-release;echo $ID$VERSION_ID)
curl -s -L https://nvidia.github.io/nvidia-docker/gpgkey | sudo apt-key add -
curl -s -L https://nvidia.github.io/nvidia-docker/$distribution/nvidia-docker.list | \
  sudo tee /etc/apt/sources.list.d/nvidia-docker.list

sudo apt-get update && sudo apt-get install -y nvidia-docker2
sudo systemctl restart docker
```

2. Раскомментировать в docker-compose.yml:
```yaml
runtime: nvidia
environment:
  - NVIDIA_VISIBLE_DEVICES=all
```

## 📦 Команды

### Работа с контейнером

```bash
# Запустить
docker-compose up -d

# Остановить
docker-compose down

# Посмотреть логи
docker-compose logs -f robo-storage

# Подключиться к запущенному контейнеру
docker-compose exec robo-storage bash

# Пересобрать образ (если изменился Dockerfile)
docker-compose build --no-cache
```

### ROS2 команды в контейнере

```bash
# Запустить симуляцию
ros2 launch robo_bringup simulation.launch.py

# Запустить на реальном роботе
ros2 launch robo_bringup robot.launch.py serial_port:=/dev/ttyUSB0

# Телеуправление
ros2 run teleop_twist_keyboard teleop_twist_keyboard

# Просмотр топиков
ros2 topic list
ros2 topic echo /scan

# Просмотр узлов
ros2 node list

# Собрать проект
cd /root/robo_ws
colcon build

# Собрать конкретный пакет
colcon build --packages-select robo_hardware
```

## 📊 Размер образа

- Base Ubuntu: ~200 MB
- ROS2 Jazzy: ~2.5 GB
- Зависимости проекта: ~1.5 GB
- **Итого: ~4.2 GB**

## 🐛 Решение проблем

### Проблема: "Cannot connect to display"

**Решение:** Включить X11 forwarding (см. раздел "GUI поддержка")

### Проблема: "Permission denied" при доступе к /dev/ttyUSB0

**Решение:**
```bash
# На хост-машине
sudo chmod 666 /dev/ttyUSB0

# Или добавить пользователя в группу dialout
sudo usermod -a -G dialout $USER
```

### Проблема: "Docker daemon not running" на Windows

**Решение:**
1. Запустить Docker Desktop
2. Убедиться, что WSL2 backend включен (Settings -> General -> Use WSL 2 based engine)

### Проблема: Медленная сборка колcon build

**Решение:**
```bash
# Использовать параллельную сборку
colcon build --parallel-workers 4

# Или собрать только измененные пакеты
colcon build --packages-select-build-failed

# Включить кеш CMake
colcon build --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## 📝 Dockerfile структура

```dockerfile
FROM ubuntu:24.04          # Base image
├── ROS2 Jazzy установка
├── Зависимости SLAM/Nav2
├── Зависимости проекта
├── Сборка исходного кода
└── Точка входа (bash)
```

## 🔐 Безопасность

- Образ использует `root` пользователя (для разработки)
- Для production используйте non-root user:

```dockerfile
RUN useradd -m -s /bin/bash robo && \
    usermod -aG docker robo

USER robo
```

## 📚 Дополнительные ресурсы

- [Docker документация](https://docs.docker.com/)
- [Docker Compose документация](https://docs.docker.com/compose/)
- [ROS2 Docker образы](https://hub.docker.com/r/osrf/ros)
- [NVIDIA Docker](https://github.com/NVIDIA/nvidia-docker)

## 💡 Советы

1. **Использовать named volumes** для сохранения данных между запусками:
   ```bash
   docker volume create robo-storage-data
   ```

2. **Собирать образ с кешем** для ускорения разработки

3. **Использовать .dockerignore** для исключения ненужных файлов

4. **Регулярно обновлять зависимости**:
   ```dockerfile
   RUN apt-get update && apt-get upgrade -y
   ```

---

**Вопросы?** Откройте issue в репозитории!
