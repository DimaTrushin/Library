#ifndef SUPRESSOR_H
#define SUPRESSOR_H

namespace NSLibrary {

class CSupressor {
public:
  bool isLocked() const {
    return locked_;
  }
  void lock() {
    locked_ = true;
  }
  void unlock() {
    locked_ = false;
  }

private:
  bool locked_ = false;
};
} // namespace NSLibrary
#endif // SUPRESSOR_H
