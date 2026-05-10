#include "can_sender.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "main.h"

namespace CAN
{

Sender::Sender(const Port & port) : port_(port) {}

Sender & Sender::set_frame_id(uint32_t frame_id)
{
  header_.StdId = frame_id;
  header_.ExtId = frame_id;
  return *this;
}

Sender & Sender::set_frame_type(FrameType frame_type)
{
  header_.IDE = static_cast<uint32_t>(frame_type);
  return *this;
}

Sender & Sender::set_remote_type(RemoteType remote_type)
{
  header_.RTR = static_cast<uint32_t>(remote_type);
  return *this;
}

Sender & Sender::set_data_len(uint8_t len)
{
  if (len >= 0 && len <= 8) header_.DLC = len;
  return *this;
}

Sender & Sender::set_time_req(bool required)
{
  header_.TransmitGlobalTime = required ? ENABLE : DISABLE;
  return *this;
}

void Sender::send(const uint8_t * data)
{
  if (!port_.transmit(&header_, data)) {
    Error_Handler();
  }
}

void Sender::send(const uint8_t * data, uint8_t len)
{
  uint8_t origin_DLC = header_.DLC;
  set_data_len(len);
  send(data);
  header_.DLC = origin_DLC;
}

}  // namespace CAN