#ifndef VTB_UART_PORT
#define VTB_UART_PORT

#include "callback.hpp"
#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include <vector>

#include "vertebra/design/loon.hpp"

namespace vtb::uart {

using Handle = UART_HandleTypeDef;

class PortBase : public Loon {
 public:
  explicit PortBase(Handle& huart, bool idle, uint8_t* buffer, uint32_t size);

  explicit operator bool() const;

  void awake() override;

  const USART_TypeDef* get_instance() const;

  bool transmit(const uint8_t* data, size_t len) const;

  bool transmit_blocking(const uint8_t* data, size_t len,
                         uint32_t timeout = HAL_MAX_DELAY) const;

  void add_tx_callback(Callback callback);
  void add_rx_callback(Callback callback);

  static void notify_tx(Handle* huart);
  static void notify_rx(Handle* huart);
  static void notify_rxh(Handle* huart);
  static void notify_idle(Handle* huart, uint16_t size);

 private:
  void check_status();
  IRQn_Type get_uart_irqn(const Handle& huart) const;

  void switch_buffer();

  void reset_rx();

  void exec_tx_callbacks();
  void exec_rx_callbacks();
  void exec_rxh_callbacks();
  void exec_idle_callbacks(uint16_t size);

  static bool bl_tx(Handle* handle, const uint8_t* data, size_t len);
  static bool it_tx(Handle* handle, const uint8_t* data, size_t len);
  static bool dma_tx(Handle* handle, const uint8_t* data, size_t len);

  Handle& huart_;
  uint8_t* buffer_;
  uint32_t size_;  // expected to be the actual size of buffer
  uint32_t half_;
  bool buf_front_half_ = false;

  bool (*tx_)(Handle* handle, const uint8_t* data, size_t len);
  HAL_StatusTypeDef (*launch_rx_)(Handle* huart, uint8_t* pData,
                                  uint16_t Size) = nullptr;

  struct __status {
    bool enable = false;
    bool interrupt = false;
    struct __dma {
      struct __rtx {
        bool enable = false;
        bool circular = false;
      } tx, rx;
      bool circular = false;
    } dma;
    bool idle = false;
  } status_;

  std::vector<Callback> tx_callbacks_;
  std::vector<Callback> rx_callbacks_;

  static std::vector<PortBase*> ports_;
};

template <uint32_t SIZE>
class Port : public PortBase {
 public:
  explicit Port(Handle& huart, bool idle=true) : PortBase(huart, idle, buffer_, SIZE * 2) {}

 private:
  uint8_t buffer_[SIZE * 2];
};

}  // namespace vtb::uart

#endif

#endif