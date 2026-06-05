#ifndef VTB_ERROR
#define VTB_ERROR

#include "connection/callback.hpp"
#include "vertebra/design/singleton.hpp"
#include <unordered_map>
#include <vector>

namespace vtb {

class Error : public Singleton<Error> {
public:
  friend class Singleton<Error>;

  static void register_error_callback(uint8_t id, Callback callback);
  static void handle_error(uint8_t id, RcvData data);
  static void handle_error(uint8_t id);
  static void handle_error(RcvData data);
  static void handle_error();

private:
  std::unordered_map<uint8_t, std::vector<Callback>> callbacks_;
};

}

#endif