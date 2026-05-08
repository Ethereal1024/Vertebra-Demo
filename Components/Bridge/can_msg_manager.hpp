#ifndef VTB_CAN_MSG_MANAGER
#define VTB_CAN_MSG_MANAGER

#include "main.h"

using CANHandle = CAN_HandleTypeDef;

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
  explicit CANMsgManager::CANMsgManager(CANHandle& hcan,
                                        std::vector<CANFilter> filters,
                                        uint8_t slave_start = 14);

  void send_data(const uint8_t* data);

  template <uint8_t DATA_LEN>
  void send_data(const uint8_t* data);

  CANMsgManager& set_frame_id(uint32_t frame_id);
  CANMsgManager& set_frame_type(CANFrameType frame_type);
  CANMsgManager& set_remote_type(CANRemoteType remote_type);
  CANMsgManager& set_time_req(bool required);

  template <uint8_t DATA_LEN>
  CANMsgManager& set_data_len();

 private:
  CAN_TxHeaderTypeDef tx_header_;
  CANHandle& hcan_;
};

#endif