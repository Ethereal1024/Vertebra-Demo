#include "can_port.hpp"

#include <vector>

namespace vtb
{

namespace can
{
CAN_FilterTypeDef Filter::to_hal_filter(uint8_t slave_start)
{
  CAN_FilterTypeDef sf = {};
  sf.FilterActivation = activation ? ENABLE : DISABLE;
  sf.FilterMode = static_cast<uint8_t>(mode);
  sf.FilterScale = static_cast<uint8_t>(scale);
  sf.FilterFIFOAssignment = static_cast<uint8_t>(fifo);
  sf.FilterBank = bank;
  sf.FilterIdHigh = id_high;
  sf.FilterIdLow = id_low;
  sf.FilterMaskIdHigh = mask_high;
  sf.FilterMaskIdLow = mask_low;
  sf.SlaveStartFilterBank = slave_start;
  return sf;
}

Filter FilterGen::gen() { return filter_; }

FilterGen & FilterGen::set_bank(uint8_t bank)
{
  filter_.bank = bank;
  return *this;
}

FilterGen & FilterGen::set_activation(bool activation)
{
  filter_.activation = activation;
  return *this;
}

FilterGen & FilterGen::set_mode(FilterMode mode)
{
  filter_.mode = mode;
  return *this;
}

FilterGen & FilterGen::set_scale(FilterScale scale)
{
  filter_.scale = scale;
  return *this;
}

FilterGen & FilterGen::set_fifo(FilterFIFO fifo)
{
  filter_.fifo = fifo;
  return *this;
}

FilterGen & FilterGen::set_id_high(uint32_t id_high)
{
  filter_.id_high = id_high;
  return *this;
}

FilterGen & FilterGen::set_id_low(uint32_t id_low)
{
  filter_.id_low = id_low;
  return *this;
}

FilterGen & FilterGen::set_mask_high(uint32_t mask_high)
{
  filter_.mask_high = mask_high;
  return *this;
}

FilterGen & FilterGen::set_mask_low(uint32_t mask_low)
{
  filter_.mask_low = mask_low;
  return *this;
}

Port::Port(Handle & hcan, std::vector<const Filter &> filters, uint8_t slave_start) : hcan_(hcan)
{
  if (filters.empty()) filters.push_back(Filter());
  bool FIFO0_enable = false, FIFO1_enable = false;
  for (const auto & f : filters) {
    if (f.fifo == FilterFIFO::FIFO0 && !FIFO0_enable) FIFO0_enable = true;
    if (f.fifo == FilterFIFO::FIFO1 && !FIFO1_enable) FIFO1_enable = true;

    CAN_FilterTypeDef sf = f.to_hal_filter(slave_start);
    if (HAL_CAN_ConfigFilter(&hcan, &sf) != HAL_OK) Error_Handler();
  }
  if (HAL_CAN_Start(&hcan) != HAL_OK) Error_Handler();

  if (FIFO0_enable) {
    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
      Error_Handler();
    }
    fifo0_ports_.emplace_back(this);
  }

  if (FIFO1_enable) {
    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) {
      Error_Handler();
    }
    fifo1_ports_.emplace_back(this);
  }
}

const CAN_TypeDef * Port::get_instance() const { return hcan_.Instance; }

bool Port::transmit(
  const CAN_TxHeaderTypeDef * header, const uint8_t * data, uint32_t * mail_box) const
{
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan_) == 0);
  HAL_StatusTypeDef status;
  if (!mail_box) {
    uint32_t _;
    status = HAL_CAN_AddTxMessage(&hcan_, header, data, &_);
  } else {
    status = HAL_CAN_AddTxMessage(&hcan_, header, data, mail_box);
  }
  return status == HAL_OK;
}

void Port::add_std_callback(uint32_t frame_id, std::function<void(const RcvData &)> callback)
{
  std_callbacks_[frame_id] = callback;
}

void Port::add_ext_callback(uint32_t frame_id, std::function<void(const RcvData &)> callback)
{
  ext_callbacks_[frame_id] = callback;
}

void Port::exec_callback(const CAN_RxHeaderTypeDef & frame_header, const uint8_t * data) const
{
  if (frame_header.RTR == CAN_RTR_REMOTE) return;
  RcvData rcv;
  rcv.size = static_cast<size_t>(frame_header.DLC);
  rcv.data = data;
  if (frame_header.IDE == CAN_ID_STD)
    std_callbacks_.at(frame_header.StdId)(rcv);
  else if (frame_header.IDE == CAN_ID_EXT)
    ext_callbacks_.at(frame_header.ExtId)(rcv);
}

void Port::notify_fifo0(
  CAN_HandleTypeDef * hcan, const CAN_RxHeaderTypeDef & header, const uint8_t * data)
{
  for (const can::Port * port : fifo0_ports_) {
    if (hcan->Instance == port->get_instance()) {
      port->exec_callback(header, data);
    }
  }
}

void Port::notify_fifo1(
  CAN_HandleTypeDef * hcan, const CAN_RxHeaderTypeDef & header, const uint8_t * data)
{
  for (const can::Port * port : fifo1_ports_) {
    if (hcan->Instance == port->get_instance()) {
      port->exec_callback(header, data);
    }
  }
}

}  // namespace can

}  // namespace vtb

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  CAN_RxHeaderTypeDef rxHeader;
  uint8_t rxData[8];
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK) return;
  vtb::can::Port::notify_fifo0(hcan, rxHeader, rxData);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  CAN_RxHeaderTypeDef rxHeader;
  uint8_t rxData[8];
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rxHeader, rxData) != HAL_OK) return;
  vtb::can::Port::notify_fifo1(hcan, rxHeader, rxData);
}
}