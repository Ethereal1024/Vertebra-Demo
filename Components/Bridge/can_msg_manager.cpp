#include "can_msg_manager.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "main.h"

namespace CAN
{

MsgManager::MsgManager(const Port & port) : port_(port) {}

MsgManager & MsgManager::set_frame_id(uint32_t frame_id)
{
  header_.StdId = frame_id;
  header_.ExtId = frame_id;
  return *this;
}

MsgManager & MsgManager::set_frame_type(FrameType frame_type)
{
  header_.IDE = static_cast<uint32_t>(frame_type);
  return *this;
}

MsgManager & MsgManager::set_remote_type(RemoteType remote_type)
{
  header_.RTR = static_cast<uint32_t>(remote_type);
  return *this;
}

MsgManager & MsgManager::set_data_len(uint8_t len)
{
  if (len >= 0 && len <= 8) header_.DLC = len;
  return *this;
}

MsgManager & MsgManager::set_time_req(bool required)
{
  header_.TransmitGlobalTime = required ? ENABLE : DISABLE;
  return *this;
}

void MsgManager::send_data(const uint8_t * data)
{
  if (!port_.transmit(&header_, data)) {
    Error_Handler();
  }
}

void MsgManager::send_data(const uint8_t * data, uint8_t len)
{
  uint8_t origin_DLC = header_.DLC;
  set_data_len(len);
  send_data(data);
  header_.DLC = origin_DLC;
}

}  // namespace CAN