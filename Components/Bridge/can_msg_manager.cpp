#include "can_msg_manager.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "main.h"

CANMsgManager::CANMsgManager(CANHandle& hcan, std::vector<CANFilter> filters,
                             uint8_t slave_start = 14)
    : hcan_(hcan) {}

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

template <uint8_t DATA_LEN>
CANMsgManager& CANMsgManager::set_data_len() {
  static_assert(
      DATA_LEN >= 0 && DATA_LEN <= 8,
      "CAN data length must be the int greater than 0 and less than 8")
      tx_header_.DLC = DATA_LEN;
  return *this;
}

CANMsgManager& CANMsgManager::set_time_req(bool required) {
  tx_header_.TransmitGlobalTime = required ? ENABLE : DISABLE;
  return *this;
}

void CANMsgManager::send_data(const uint8_t* data) {
  uint32_t _;
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan_) == 0);
  HAL_StatusTypeDef status =
      HAL_CAN_AddTxMessage(&hcan_, &tx_header_, data, &_);
  if (status != HAL_OK) {
    Error_Handler();
  }
}

template <uint8_t DATA_LEN>
void CANMsgManager::send_data(const uint8_t* data) {
  uint8_t origin_DLC = tx_header_.DLC;
  set_data_len<DATA_LEN>();
  send_data(data);
  tx_header_.DLC = origin_DLC;
}