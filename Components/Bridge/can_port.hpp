#ifndef VTB_CAN_PORT
#define VTB_CAN_PORT

#include <cstdint>
#include <vector>

#include "main.h"

namespace CAN
{

using Handle = CAN_HandleTypeDef;

enum class FilterMode : uint8_t
{
  Mask = CAN_FILTERMODE_IDMASK,
  List = CAN_FILTERMODE_IDLIST
};

enum class FilterScale : uint8_t
{
  BIT16 = CAN_FILTERSCALE_16BIT,
  BIT32 = CAN_FILTERSCALE_32BIT
};

enum class FilterFIFO : uint8_t
{
  FIFO0 = CAN_FILTER_FIFO0,
  FIFO1 = CAN_FILTER_FIFO1
};

struct Filter
{
  uint8_t bank = 0;
  bool activation = true;
  FilterMode mode = FilterMode::Mask;
  FilterScale scale = FilterScale::BIT32;
  FilterFIFO fifo = FilterFIFO::FIFO0;
  uint32_t id_high = 0;
  uint32_t id_low = 0;
  uint32_t mask_high = 0;
  uint32_t mask_low = 0;

  CAN_FilterTypeDef to_hal_filter(uint8_t slave_start = 14);
};

class FilterGen
{
public:
  FilterGen() = default;
  Filter gen();

  FilterGen & set_bank(uint8_t bank);
  FilterGen & set_activation(bool activation);
  FilterGen & set_mode(FilterMode mode);
  FilterGen & set_scale(FilterScale scale);
  FilterGen & set_fifo(FilterFIFO fifo);
  FilterGen & set_id_high(uint32_t id_high);
  FilterGen & set_id_low(uint32_t id_low);
  FilterGen & set_mask_high(uint32_t mask_high);
  FilterGen & set_mask_low(uint32_t mask_low);

private:
  Filter filter_;
};

class Port
{
public:
  explicit Port(Handle & hcan, std::vector<const Filter &> filters, uint8_t slave_start = 14);

  bool transmit(
    const CAN_TxHeaderTypeDef * header, const uint8_t * data, uint32_t * mail_box = nullptr) const;

private:
  Handle & hcan_;
};

}  // namespace CAN

#endif