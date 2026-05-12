cmake_minimum_required(VERSION 3.22)

add_library(vertebra OBJECT
  motor.cpp
  motors/motor_m3508.cpp
  connection/can_sender.cpp
  connection/can_receiver.cpp
  connection/can_port.cpp
  task/vtb_task.cpp
  task/cycle_task.cpp
  bridge/vtb_bridge.cpp
  components.cpp
)

target_include_directories(vertebra
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vertebra stm32cubemx)