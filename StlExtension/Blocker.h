#ifndef BLOCKER_H
#define BLOCKER_H

#include <cassert>

namespace NSLibrary {

//---------------------------------------------------------------------------
// Enum must contain Active = 0,
// Each blocker should be a power of 2, e.g.
// enum class Enum : CType { Active = 0, Block1 = 1, Block2 = 2, Block3 = 4 };
//---------------------------------------------------------------------------

struct COneBlocker {
  using CType = unsigned char;
  enum class Enum : CType { Active = 0, Block = 1 };
};

template<class TBlockers>
class CBlock : public TBlockers {
  using CBase = TBlockers;

public:
  using CType = typename CBase::CType;
  using Enum = typename CBase::Enum;
  static_assert(static_cast<CType>(Enum::Active) == 0,
                "Enum must contain \'Active = 0\'");

  bool isActive() const {
    return Flags_ == static_cast<CType>(CBase::Enum::Active);
  }

  void activate(Enum Blocker) {
    assert(Blocker != CBase::Enum::Active);
    Flags_ &= static_cast<CType>(-1) ^ static_cast<CType>(Blocker);
  }
  void deactivate(Enum Blocker) {
    assert(Blocker != CBase::Enum::Active);
    Flags_ |= static_cast<CType>(Blocker);
  }

private:
  CType Flags_ = static_cast<CType>(CBase::Enum::Active);
};

} // namespace NSLibrary

#endif // BLOCKER_H
