#ifndef __CAN_MSG_MANAGER__
#define __CAN_MSG_MANAGER__

#include "main.h"

enum class CANFrameType : uint32_t {
  Standard = CAN_ID_STD,
  Extended = CAN_ID_EXT
};

enum class CANRemoteType : uint32_t {
  Data = CAN_RTR_DATA,
  Remote = CAN_RTR_REMOTE
};

class CANMsgManager {
 public:
  CANMsgManager() = delete;
  CANMsgManager(CAN_HandleTypeDef* hcan);

  void send_data(const uint8_t* data);
  void send_data(const uint8_t* data, size_t data_len);

  CANMsgManager& set_frame_id(uint32_t frame_id);
  CANMsgManager& set_frame_type(CANFrameType frame_type);
  CANMsgManager& set_remote_type(CANRemoteType remote_type);
  CANMsgManager& set_data_len(size_t data_len);
  CANMsgManager& set_time_req(bool required);

 private:
  CAN_TxHeaderTypeDef tx_header_;
  CAN_HandleTypeDef* hcan_;
};

#endif