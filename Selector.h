#ifndef SELECTOR_H
#define SELECTOR_H

#include "TypeList/ElementList.h"
#include "TypeList/TypeList.h"

//--------------------------------------------------------------------------------
// Selector
//--------------------------------------------------------------------------------
//
// Selector selects a function pointer depending on a level parameter
// How to use:
// Selector::SetSignature<void(int)>::SetLevels<9,3>::choose(&f1, &f2, &fd, 5);
// 1) We select signature of a function void(int) in the example
// 2) We set levels of function (these are the SIMD levels)
//    9 and 3 in the example
// 3) We choose a required function using function choose
//    f1 has level 9
//    f2 has level 3
//    fd is a default function
//    5 is a given level
//    Function chooses the highest possible level of function less or equal to
//    the given level of instructions
//    In the example we will get f2
// Notes:
// 1) f1, f2, fd must be functions of the given signature
// 2) instruction levels must be sorted in decreasing order
// 3) instruction level is unsigned int
//--------------------------------------------------------------------------------

namespace NSLibrary {

class Selector {
public:
  template<class TSignature>
  class SetSignature {
    using CSignature = TSignature;
    using FunctionPtr = CSignature*;

    template<class TElementList>
    class SetLevelsImpl {
      using LevelList = TElementList;
      static_assert(EL::IsDecreasing<LevelList>::value,
                    "Levels must go in decreasing order!");

      template<class... TFunArgs>
      struct SelectorImpl;

      template<class TFunArg, class... TFunArgs>
      struct SelectorImpl<TFunArg, TFunArgs...> {
        static TFunArg choose(TFunArg Fun1, TFunArgs... Funs, int level) {
          if (level >= LevelList::Head)
            return Fun1;
          return SetLevelsT<typename LevelList::Tail>::choose(Funs..., level);
        }
      };

      template<class TFunArg>
      struct SelectorImpl<TFunArg> {
        static TFunArg choose(TFunArg DefaultFun, int) {
          return DefaultFun;
        }
      };

      template<class TSeq>
      struct SelectorHelperImpl;

      template<class TSeq>
      using SelectorHelper = typename SelectorHelperImpl<TSeq>::Result;

      template<class... TTypes>
      struct SelectorHelperImpl<TypeSeq<TTypes...>> {
        using Result = SelectorImpl<TTypes...>;
      };

    public:
      using Selector = SelectorHelper<
          TL::MakeTypeSeqN<FunctionPtr, EL::Length<LevelList>::value + 1>>;
    };

  public:
    template<class TElementList>
    using SetLevelsT = typename SetLevelsImpl<TElementList>::Selector;
    template<int... TInts>
    using SetLevels = SetLevelsT<EL::MakeElementList<TInts...>>;
  };
};
} // namespace NSLibrary

#endif // SELECTOR_H
