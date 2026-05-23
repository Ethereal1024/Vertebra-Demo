#ifndef VTB_LOON
#define VTB_LOON

namespace vtb {

class TaskLauncher;

class Loon {
 public:
  Loon();
  virtual ~Loon() = default;
  virtual void awake() = 0;
};

}  // namespace vtb

#endif
