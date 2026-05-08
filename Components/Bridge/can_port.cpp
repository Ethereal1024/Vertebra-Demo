#include "can_port.hpp"

namespace CAN {
Port::Port(Handle& hcan, std::vector<Filter> filters, uint8_t slave_start)
    : hcan_(hcan) {
  if (filters.empty()) filters.push_back(Filter());
  bool FIFO0_enable = false, FIFO1_enable = false;
  for (const auto& f : filters) {
    if (f.fifo == FilterFIFO::FIFO0 && !FIFO0_enable) FIFO0_enable = true;
    if (f.fifo == FilterFIFO::FIFO1 && !FIFO1_enable) FIFO1_enable = true;

    CAN_FilterTypeDef sf = {};
    sf.FilterActivation = f.activation ? ENABLE : DISABLE;
    sf.FilterMode = static_cast<uint8_t>(f.mode);
    sf.FilterScale = static_cast<uint8_t>(f.scale);
    sf.FilterFIFOAssignment = static_cast<uint8_t>(f.fifo);
    sf.FilterBank = f.bank;
    sf.FilterIdHigh = f.id_high;
    sf.FilterIdLow = f.id_low;
    sf.FilterMaskIdHigh = f.mask_high;
    sf.FilterMaskIdLow = f.mask_low;
    sf.SlaveStartFilterBank = slave_start;
    if (HAL_CAN_ConfigFilter(&hcan, &sf) != HAL_OK) Error_Handler();
  }
  if (HAL_CAN_Start(&hcan) != HAL_OK) Error_Handler();

  if (FIFO0_enable) {
    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) !=
        HAL_OK) {
      Error_Handler();
    }
  }

  if (FIFO1_enable) {
    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) !=
        HAL_OK) {
      Error_Handler();
    }
  }
}

}  // namespace CAN