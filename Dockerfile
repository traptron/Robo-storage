FROM ubuntu:24.04

# Избежать интерактивных запросов во время установки
ENV DEBIAN_FRONTEND=noninteractive
ENV ROS_DISTRO=jazzy

# Обновить пакеты и установить базовые утилиты
RUN apt-get update && apt-get install -y \
    curl \
    gnupg \
    lsb-release \
    software-properties-common \
    git \
    wget \
    nano \
    vim \
    htop \
    tmux \
    && rm -rf /var/lib/apt/lists/*

# Добавить ROS2 репозиторий и ключ
RUN curl -sSL https://repo.ros2.org/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | \
    tee /etc/apt/sources.list.d/ros2.list > /dev/null

# Установить ROS2 Jazzy Desktop
RUN apt-get update && apt-get install -y \
    ros-${ROS_DISTRO}-desktop \
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Инициализировать rosdep
RUN rosdep init && rosdep update

# Установить дополнительные зависимости для сборки
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libssl-dev \
    libffi-dev \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

# Установить зависимости для SLAM и навигации
RUN apt-get update && apt-get install -y \
    ros-${ROS_DISTRO}-slam-toolbox \
    ros-${ROS_DISTRO}-navigation2 \
    ros-${ROS_DISTRO}-nav2-bringup \
    ros-${ROS_DISTRO}-rplidar-ros \
    ros-${ROS_DISTRO}-ros2-control \
    ros-${ROS_DISTRO}-ros2-controllers \
    ros-${ROS_DISTRO}-gazebo-ros \
    ros-${ROS_DISTRO}-gazebo-ros-pkgs \
    ros-${ROS_DISTRO}-rviz2 \
    ros-${ROS_DISTRO}-teleop-twist-keyboard \
    ros-${ROS_DISTRO}-teleop-twist-joy \
    ros-${ROS_DISTRO}-joy \
    ros-${ROS_DISTRO}-robot-localization \
    && rm -rf /var/lib/apt/lists/*

# Установить Python пакеты
RUN pip3 install --upgrade pip && \
    pip3 install \
    vcstool \
    rosdep \
    numpy \
    scipy \
    matplotlib \
    pyyaml

# Создать рабочий каталог
WORKDIR /root/robo_ws

# Скопировать исходный код проекта
COPY . ./src/robo-storage

# Установить зависимости через rosdep
RUN cd /root/robo_ws && \
    rosdep install --from-paths src --ignore-src -r -y 2>/dev/null || true

# Импортировать зависимости через vcs (если есть .repos)
RUN cd /root/robo_ws/src/robo-storage && \
    if [ -f ros2.repos ]; then \
      vcs import /root/robo_ws/src < ros2.repos || true; \
    fi

# Собрать проект ROS2
RUN bash -c 'source /opt/ros/${ROS_DISTRO}/setup.bash && \
    cd /root/robo_ws && \
    colcon build --symlink-install 2>&1 || true'

# Добавить source в bashrc
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc && \
    echo "source /root/robo_ws/install/setup.bash" >> ~/.bashrc

# Настроить переменные окружения
ENV ROS_DOMAIN_ID=42
ENV ROS_LOCALHOST_ONLY=1

# Точка входа
ENTRYPOINT ["/bin/bash"]
CMD ["-c", "source ~/.bashrc && /bin/bash"]
