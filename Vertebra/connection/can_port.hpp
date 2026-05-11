#ifndef VTB_CAN_PORT
#define VTB_CAN_PORT

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "main.h"
#include "stm32f4xx_hal_can.h"

namespace vtb
{

namespace can
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

struct RcvData
{
  const uint8_t * data;
  size_t size;
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
  explicit Port(Handle & hcan, std::vector<const Filter &> filters = {}, uint8_t slave_start = 14);

  const CAN_TypeDef * get_instance() const;

  bool transmit(
    const CAN_TxHeaderTypeDef * header, const uint8_t * data, uint32_t * mail_box = nullptr) const;

  void add_std_callback(uint32_t frame_id, std::function<void(const RcvData &)> callback);
  void add_ext_callback(uint32_t frame_id, std::function<void(const RcvData &)> callback);

  static void notify_fifo0(
    CAN_HandleTypeDef * hcan, const CAN_RxHeaderTypeDef & header, const uint8_t * data);
  static void notify_fifo1(
    CAN_HandleTypeDef * hcan, const CAN_RxHeaderTypeDef & header, const uint8_t * data);

private:
  void exec_callback(const CAN_RxHeaderTypeDef & frame_header, const uint8_t * data) const;

  Handle & hcan_;

  std::unordered_map<uint32_t, std::function<void(const RcvData &)>> std_callbacks_;
  std::unordered_map<uint32_t, std::function<void(const RcvData &)>> ext_callbacks_;

  static std::vector<Port *> fifo0_ports_;
  static std::vector<Port *> fifo1_ports_;
};

}  // namespace can

}  // namespace vtb

#endif