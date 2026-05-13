cmake_minimum_required(VERSION 3.22)

add_library(vertebra OBJECT
  vertebra/motor.cpp
  vertebra/motors/motor_m3508.cpp
  vertebra/connection/can_sender.cpp
  vertebra/connection/can_receiver.cpp
  vertebra/connection/can_port.cpp
  vertebra/task/vtb_task.cpp
  vertebra/bridge/vtb_bridge.cpp
  vertebra/components.cpp
)

target_include_directories(vertebra
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vertebra stm32cubemx)