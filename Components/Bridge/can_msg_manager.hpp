#ifndef VTB_CAN_MSG_MANAGER
#define VTB_CAN_MSG_MANAGER

#include "main.h"

#include "can_port.hpp"

namespace CAN
{

enum class FrameType : uint32_t
{
  Standard = CAN_ID_STD,
  Extended = CAN_ID_EXT
};

enum class RemoteType : uint32_t
{
  Data = CAN_RTR_DATA,
  Remote = CAN_RTR_REMOTE
};

class MsgManager
{
public:
  explicit MsgManager::MsgManager(const Port& port);

  void send_data(const uint8_t * data);

  void send_data(const uint8_t * data, uint8_t len);

  MsgManager & set_frame_id(uint32_t frame_id);
  MsgManager & set_frame_type(FrameType frame_type);
  MsgManager & set_remote_type(RemoteType remote_type);
  MsgManager & set_time_req(bool required);
  MsgManager & set_data_len(uint8_t len);

private:
  CAN_TxHeaderTypeDef header_;
  const Port& port_;
};

}  // namespace CAN

#endif