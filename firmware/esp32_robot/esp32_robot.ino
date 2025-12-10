/*
 * ESP32 micro-ROS Firmware для робо-тележки
 * 
 * Функционал:
 * - Управление 2 DC моторами (дифференциальный привод)
 * - Чтение инкрементальных энкодеров
 * - Вычисление одометрии на ESP32
 * - Публикация одометрии в ROS2 через micro-ROS (nav_msgs/Odometry)
 * - Подписка на команды скорости из ROS2 (geometry_msgs/Twist)
 * 
 * Автор: Robo-Storage Team
 * Версия: 0.1.0
 */

#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <nav_msgs/msg/odometry.h>
#include <std_msgs/msg/int32.h>

// ============= КОНФИГУРАЦИЯ ПИНОВ =============
// Моторы
#define LEFT_MOTOR_PWM 16
#define LEFT_MOTOR_DIR 17
#define RIGHT_MOTOR_PWM 18
#define RIGHT_MOTOR_DIR 19

// Энкодеры (используйте только GPIO 34-39 для входов!)
#define LEFT_ENC_A 34
#define LEFT_ENC_B 35
#define RIGHT_ENC_A 36
#define RIGHT_ENC_B 39

// ============= ПАРАМЕТРЫ РОБОТА =============
const float WHEEL_RADIUS = 0.065;        // Радиус колеса (м)
const float WHEEL_SEPARATION = 0.35;     // Расстояние между колесами (м)
const int ENCODER_TICKS_PER_REV = 1440;  // Импульсов энкодера на оборот
const int PWM_FREQUENCY = 1000;          // Частота ШИМ (Гц)
const int PWM_RESOLUTION = 8;            // Разрядность ШИМ (8 бит = 0-255)
const int PWM_CHANNEL_LEFT = 0;
const int PWM_CHANNEL_RIGHT = 1;

// ============= ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ =============
// Энкодеры
volatile long left_encoder_count = 0;
volatile long right_encoder_count = 0;

// Одометрия
float robot_x = 0.0;
float robot_y = 0.0;
float robot_theta = 0.0;
unsigned long last_odom_time = 0;

// Команды скорости
float cmd_vel_linear = 0.0;
float cmd_vel_angular = 0.0;

// micro-ROS
rcl_publisher_t odom_publisher;
rcl_subscription_t cmd_vel_subscriber;
nav_msgs__msg__Odometry odom_msg;
geometry_msgs__msg__Twist cmd_vel_msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// ============= ПРЕРЫВАНИЯ ЭНКОДЕРОВ =============
void IRAM_ATTR leftEncoderISR() {
  if (digitalRead(LEFT_ENC_B) == HIGH) {
    left_encoder_count++;
  } else {
    left_encoder_count--;
  }
}

void IRAM_ATTR rightEncoderISR() {
  if (digitalRead(RIGHT_ENC_B) == HIGH) {
    right_encoder_count++;
  } else {
    right_encoder_count--;
  }
}

// ============= ФУНКЦИЯ ОШИБКИ =============
void error_loop() {
  while(1) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(100);
  }
}

// ============= CALLBACK ДЛЯ CMD_VEL =============
void cmd_vel_callback(const void *msgin) {
  const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;
  cmd_vel_linear = msg->linear.x;
  cmd_vel_angular = msg->angular.z;
  
  // Вычисление скоростей колес
  float left_wheel_vel = cmd_vel_linear - (cmd_vel_angular * WHEEL_SEPARATION / 2.0);
  float right_wheel_vel = cmd_vel_linear + (cmd_vel_angular * WHEEL_SEPARATION / 2.0);
  
  // Преобразование в PWM (255 = максимальная скорость)
  int left_pwm = constrain((int)(left_wheel_vel / (2 * WHEEL_RADIUS) * 255), -255, 255);
  int right_pwm = constrain((int)(right_wheel_vel / (2 * WHEEL_RADIUS) * 255), -255, 255);
  
  // Установка направления и PWM
  setMotor(LEFT_MOTOR_DIR, LEFT_MOTOR_PWM, PWM_CHANNEL_LEFT, left_pwm);
  setMotor(RIGHT_MOTOR_DIR, RIGHT_MOTOR_PWM, PWM_CHANNEL_RIGHT, right_pwm);
}

// ============= УПРАВЛЕНИЕ МОТОРОМ =============
void setMotor(int dir_pin, int pwm_pin, int channel, int speed) {
  if (speed >= 0) {
    digitalWrite(dir_pin, HIGH);
    ledcWrite(channel, abs(speed));
  } else {
    digitalWrite(dir_pin, LOW);
    ledcWrite(channel, abs(speed));
  }
}

// ============= CALLBACK ТАЙМЕРА ОДОМЕТРИИ =============
void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    // Вычисление времени
    unsigned long current_time = millis();
    float dt = (current_time - last_odom_time) / 1000.0;  // Секунды
    last_odom_time = current_time;
    
    // Чтение энкодеров
    long left_ticks = left_encoder_count;
    long right_ticks = right_encoder_count;
    
    // Вычисление пройденного расстояния каждым колесом
    float left_distance = (left_ticks / (float)ENCODER_TICKS_PER_REV) * (2 * PI * WHEEL_RADIUS);
    float right_distance = (right_ticks / (float)ENCODER_TICKS_PER_REV) * (2 * PI * WHEEL_RADIUS);
    
    // Вычисление линейной и угловой скорости
    float linear_vel = (left_distance + right_distance) / 2.0 / dt;
    float angular_vel = (right_distance - left_distance) / WHEEL_SEPARATION / dt;
    
    // Обновление позиции робота
    float delta_theta = (right_distance - left_distance) / WHEEL_SEPARATION;
    float delta_x = (left_distance + right_distance) / 2.0 * cos(robot_theta + delta_theta / 2.0);
    float delta_y = (left_distance + right_distance) / 2.0 * sin(robot_theta + delta_theta / 2.0);
    
    robot_x += delta_x;
    robot_y += delta_y;
    robot_theta += delta_theta;
    
    // Нормализация угла [-PI, PI]
    while (robot_theta > PI) robot_theta -= 2 * PI;
    while (robot_theta < -PI) robot_theta += 2 * PI;
    
    // Сброс счётчиков энкодеров
    left_encoder_count = 0;
    right_encoder_count = 0;
    
    // Заполнение сообщения одометрии
    odom_msg.header.stamp.sec = current_time / 1000;
    odom_msg.header.stamp.nanosec = (current_time % 1000) * 1000000;
    odom_msg.header.frame_id.data = "odom";
    odom_msg.child_frame_id.data = "base_link";
    
    // Позиция
    odom_msg.pose.pose.position.x = robot_x;
    odom_msg.pose.pose.position.y = robot_y;
    odom_msg.pose.pose.position.z = 0.0;
    
    // Ориентация (quaternion из yaw)
    odom_msg.pose.pose.orientation.x = 0.0;
    odom_msg.pose.pose.orientation.y = 0.0;
    odom_msg.pose.pose.orientation.z = sin(robot_theta / 2.0);
    odom_msg.pose.pose.orientation.w = cos(robot_theta / 2.0);
    
    // Скорость
    odom_msg.twist.twist.linear.x = linear_vel;
    odom_msg.twist.twist.linear.y = 0.0;
    odom_msg.twist.twist.angular.z = angular_vel;
    
    // Публикация
    RCSOFTCHECK(rcl_publish(&odom_publisher, &odom_msg, NULL));
  }
}

// ============= SETUP =============
void setup() {
  // Инициализация Serial для отладки
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("Инициализация ESP32 micro-ROS...");
  
  // Настройка пинов
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(LEFT_ENC_A, INPUT_PULLUP);
  pinMode(LEFT_ENC_B, INPUT_PULLUP);
  pinMode(RIGHT_ENC_A, INPUT_PULLUP);
  pinMode(RIGHT_ENC_B, INPUT_PULLUP);
  
  // Настройка ШИМ
  ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(LEFT_MOTOR_PWM, PWM_CHANNEL_LEFT);
  ledcAttachPin(RIGHT_MOTOR_PWM, PWM_CHANNEL_RIGHT);
  
  // Прерывания энкодеров
  attachInterrupt(digitalPinToInterrupt(LEFT_ENC_A), leftEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENC_A), rightEncoderISR, RISING);
  
  // Инициализация micro-ROS
  set_microros_transports();
  
  allocator = rcl_get_default_allocator();
  
  // Support
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  
  // Node
  RCCHECK(rclc_node_init_default(&node, "esp32_robot_node", "", &support));
  
  // Publisher (одометрия)
  RCCHECK(rclc_publisher_init_default(
    &odom_publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
    "odom"));
  
  // Subscriber (команды скорости)
  RCCHECK(rclc_subscription_init_default(
    &cmd_vel_subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
    "cmd_vel"));
  
  // Timer (публикация одометрии каждые 50мс = 20Hz)
  const unsigned int timer_timeout = 50;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));
  
  // Executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &cmd_vel_subscriber, &cmd_vel_msg, &cmd_vel_callback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
  
  // Инициализация времени
  last_odom_time = millis();
  
  Serial.println("ESP32 готов к работе!");
}

// ============= LOOP =============
void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}
