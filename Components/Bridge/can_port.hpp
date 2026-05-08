#ifndef VTB_CAN_PORT
#define VTB_CAN_PORT

#include <cstdint>
#include <vector>

#include "main.h"

namespace CAN {

using Handle = CAN_HandleTypeDef;

enum class FilterMode : uint8_t {
  Mask = CAN_FILTERMODE_IDMASK,
  List = CAN_FILTERMODE_IDLIST
};

enum class FilterScale : uint8_t {
  BIT16 = CAN_FILTERSCALE_16BIT,
  BIT32 = CAN_FILTERSCALE_32BIT
};

enum class FilterFIFO : uint8_t {
  FIFO0 = CAN_FILTER_FIFO0,
  FIFO1 = CAN_FILTER_FIFO1
};

struct Filter {
  uint8_t bank = 0;
  bool activation = true;
  FilterMode mode = FilterMode::Mask;
  FilterScale scale = FilterScale::BIT32;
  FilterFIFO fifo = FilterFIFO::FIFO0;
  uint32_t id_high = 0;
  uint32_t id_low = 0;
  uint32_t mask_high = 0;
  uint32_t mask_low = 0;
};

class Port {
 public:
  explicit Port(Handle& hcan, std::vector<Filter> filters,
                uint8_t slave_start = 14);

 private:
  Handle& hcan_;
};

}  // namespace CAN

#endif