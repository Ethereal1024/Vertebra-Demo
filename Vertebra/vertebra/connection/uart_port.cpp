#include "uart_port.hpp"

namespace vtb::uart
{

Port::Port(Handle & huart) : huart_(huart) {}

void Port::awake() {
  
}

}