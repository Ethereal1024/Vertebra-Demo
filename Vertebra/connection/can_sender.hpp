#ifndef VTB_CAN_MSG_MANAGER
#define VTB_CAN_MSG_MANAGER

#include "can_port.hpp"
#include "main.h"

namespace vtb::can
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

class Sender
{
public:
  explicit Sender(const Port & port);

  void send(const uint8_t * data);

  void send(const uint8_t * data, uint8_t len);

  Sender & set_frame_id(uint32_t frame_id);
  Sender & set_frame_type(FrameType frame_type);
  Sender & set_remote_type(RemoteType remote_type);
  Sender & set_time_req(bool required);
  Sender & set_data_len(uint8_t len);

private:
  CAN_TxHeaderTypeDef header_;
  const Port & port_;
};

}  // namespace vtb::can

#endif