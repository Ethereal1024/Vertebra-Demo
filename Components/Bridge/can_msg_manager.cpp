#include "can_msg_manager.hpp"

#include <cstdint>
#include <stdexcept>

#include "main.h"

CANMsgManager::CANMsgManager(CAN_HandleTypeDef* hcan) : hcan_(hcan) {}

CANMsgManager& CANMsgManager::set_frame_id(uint32_t frame_id) {
  tx_header_.StdId = frame_id;
  tx_header_.ExtId = frame_id;
  return *this;
}

CANMsgManager& CANMsgManager::set_frame_type(CANFrameType frame_type) {
  tx_header_.IDE = static_cast<uint32_t>(frame_type);
  return *this;
}

CANMsgManager& CANMsgManager::set_remote_type(CANRemoteType remote_type) {
  tx_header_.RTR = static_cast<uint32_t>(remote_type);
  return *this;
}

CANMsgManager& CANMsgManager::set_data_len(size_t data_len) {
  if (data_len > 8) {
    throw std::out_of_range("CAN data length cannot greater than 8");
  }
  tx_header_.DLC = static_cast<uint8_t>(data_len);
  return *this;
}

CANMsgManager& CANMsgManager::set_time_req(bool required) {
  tx_header_.TransmitGlobalTime = required ? ENABLE : DISABLE;
  return *this;
}

void CANMsgManager::send_data(const uint8_t* data) {
  uint32_t _;
  while (HAL_CAN_GetTxMailboxesFreeLevel(hcan_) == 0);
  HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(hcan_, &tx_header_, data, &_);
  if (status != HAL_OK) {
    Error_Handler();
  }
}

void CANMsgManager::send_data(const uint8_t* data, size_t data_len) {
  uint8_t origin_DLC = tx_header_.DLC;
  set_data_len(data_len);
  send_data(data);
  tx_header_.DLC = origin_DLC;
}