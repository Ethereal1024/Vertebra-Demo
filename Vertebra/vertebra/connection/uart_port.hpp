#ifndef VTB_UART_PORT
#define VTB_UART_PORT

#include "callback.hpp"
#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "vertebra/design/loon.hpp"
#include <vector>

namespace vtb::uart
{

using Handle = UART_HandleTypeDef;

class Port : public Loon
{
public:
  explicit Port(Handle & huart, uint8_t * buffer, uint32_t size);

  explicit operator bool() const;

  void awake() override;

  const USART_TypeDef * get_instance() const;

  bool transmit(const uint8_t * data, size_t len);

  bool transmit_blocking(const uint8_t * data, size_t len, uint32_t timeout = HAL_MAX_DELAY);

  void add_tx_callback(Callback callback);
  void add_rx_callback(Callback callback);

  static void notify_tx(Handle *huart);
  static void notify_rx(Handle *huart);
  static void notify_rxh(Handle * huart);
  static void notify_idle(Handle *huart, uint16_t size);

  void exec_tx_callbacks();
  void exec_rx_callbacks();
  void exec_rxh_callbacks();
  void exec_idle_callbacks(uint16_t size);

private:
  void check_status();

  void switch_buffer();

  uint32_t get_dma_cr() const;
  bool dma_is_circular(uint32_t cr) const;

  static void reset_rx_it(Port* port);
  static void reset_rx_dma(Port* port);

  static bool bl_tx(Handle * handle, const uint8_t * data, size_t len);
  static bool it_tx(Handle * handle, const uint8_t * data, size_t len);
  static bool dma_tx(Handle * handle, const uint8_t * data, size_t len);

  Handle & huart_;
  uint8_t * buffer_;
  uint32_t size_;  // expected to be the actual size of buffer
  uint32_t half_;
  bool buf_half_used_ = false;

  bool (*tx_)(Handle * handle, const uint8_t * data, size_t len);
  void (*reset_rx_)(Port* port) = nullptr;

  struct __status
  {
    bool enable = false;
    struct __interrupt
    {
      bool tx = false;
      bool tc = false;
      bool rx = false;
    } interrupt;
    struct __dma
    {
      bool tx = false;
      bool rx = false;
      bool circular = false;
    } dma;
    bool idle = false;
  } status_;

  std::vector<Callback> tx_callbacks_;
  std::vector<Callback> rx_callbacks_;

  static std::vector<Port *> ports_;
};

}  // namespace vtb::uart

#endif

#endif