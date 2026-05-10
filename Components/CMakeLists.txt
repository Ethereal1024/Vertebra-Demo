cmake_minimum_required(VERSION 3.22)

add_library(vertebra OBJECT
  motor.cpp
  Motors/motor_m3508.cpp
  bridge/can_sender.cpp
  bridge/can_receiver.cpp
  bridge/can_port.cpp
)

target_include_directories(vertebra PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vertebra stm32cubemx)