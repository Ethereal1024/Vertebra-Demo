cmake_minimum_required(VERSION 3.22)

add_library(vertebra OBJECT
  motor.cpp
  Motors/motor_m3508.cpp
  Bridge/can_msg_manager.cpp
)

target_include_directories(vertebra PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vertebra stm32cubemx)