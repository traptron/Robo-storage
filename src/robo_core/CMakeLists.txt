cmake_minimum_required(VERSION 3.8)
project(robo_core)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Найти зависимости
find_package(ament_cmake REQUIRED)

# Метапакет не имеет исполняемых файлов или библиотек
# Только группирует зависимости

ament_package()
