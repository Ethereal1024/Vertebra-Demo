cmake_minimum_required(VERSION 3.22)

add_library(vertebra OBJECT
  vertebra/device/motor/motor.cpp
  vertebra/connection/can_sender.cpp
  vertebra/connection/can_receiver.cpp
  vertebra/connection/can_port.cpp
  vertebra/task/vtb_task.cpp
  vertebra/bridge/vtb_bridge.cpp
  vertebra/components.cpp
  vertebra/design/loon.cpp
  vertebra/control/pid_controller.cpp
  vertebra/connection/uart_port.cpp
  vertebra/connection/callback.cpp
  vertebra/error.hpp
  vertebra/error.cpp
)

target_include_directories(vertebra
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vertebra stm32cubemx)