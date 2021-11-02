#ifndef ELEMENTLIST_H
#define ELEMENTLIST_H

#include "NullType.h"

namespace NSLibrary {
//--------------------------------------------------------------------------------
// ElementSeq
//--------------------------------------------------------------------------------

template<int... TElements>
struct ElementSeq {};

//--------------------------------------------------------------------------------
// ElementList
//--------------------------------------------------------------------------------

template<int THead, class TTail>
struct ElementList {
  static constexpr int Head = THead;
  using Tail = TTail;
};

using EmptyElementList = NullType;
//--------------------------------------------------------------------------------

namespace EL {
//--------------------------------------------------------------------------------
// MakeElementList
//--------------------------------------------------------------------------------

namespace NSElementListDetail {
template<int... TElements>
struct MakeElementListImpl;
}

template<int... TElements>
using MakeElementList =
    typename NSElementListDetail::MakeElementListImpl<TElements...>::Result;

namespace NSElementListDetail {
template<int TFirst, int... TElements>
struct MakeElementListImpl<TFirst, TElements...> {
  using Result = ElementList<TFirst, MakeElementList<TElements...>>;
};

template<int TElement>
struct MakeElementListImpl<TElement> {
  using Result = ElementList<TElement, NullType>;
};

template<>
struct MakeElementListImpl<> {
  using Result = NullType;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// Seq2List
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TElementSeq>
struct Seq2ListImpl;
}

template<class TElementSeq>
using Seq2List =
    typename NSElementListDetail::Seq2ListImpl<TElementSeq>::Result;

namespace NSElementListDetail {
template<int... TElements>
struct Seq2ListImpl<ElementSeq<TElements...>> {
  using Result = MakeElementList<TElements...>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// MergeSeq
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TSeq1, class TSeq2>
struct MergeSeqImpl;
}

template<class TSeq1, class TSeq2>
using MergeSeq =
    typename NSElementListDetail::MergeSeqImpl<TSeq1, TSeq2>::Result;

namespace NSElementListDetail {
template<int... TElements1, int... TElements2>
struct MergeSeqImpl<ElementSeq<TElements1...>, ElementSeq<TElements2...>> {
  using Result = ElementSeq<TElements1..., TElements2...>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// List2Seq
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList>
struct List2SeqImpl;
}

template<class TList>
using List2Seq = typename NSElementListDetail::List2SeqImpl<TList>::Result;

namespace NSElementListDetail {
template<>
struct List2SeqImpl<NullType> {
  using Result = ElementSeq<>;
};

template<int THead, class TTail>
struct List2SeqImpl<ElementList<THead, TTail>> {
  using Result = MergeSeq<ElementSeq<THead>, List2Seq<TTail>>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// IsDecreasing
//--------------------------------------------------------------------------------

template<class TList>
struct IsDecreasing;

template<>
struct IsDecreasing<EmptyElementList> {
  static constexpr bool value = true;
};

template<int THead>
struct IsDecreasing<ElementList<THead, NullType>> {
  static constexpr bool value = true;
};

template<int THead, int THead2, class TTail2>
struct IsDecreasing<ElementList<THead, ElementList<THead2, TTail2>>> {
  static constexpr bool value =
      THead > THead2&& IsDecreasing<ElementList<THead2, TTail2>>::value;
};
//--------------------------------------------------------------------------------
// Length
//--------------------------------------------------------------------------------

template<class TList>
struct Length;

template<>
struct Length<NullType> {
  static constexpr int value = 0;
};

template<int THead, class TTail>
struct Length<ElementList<THead, TTail>> {
  static constexpr int value = 1 + Length<TTail>::value;
};
//--------------------------------------------------------------------------------
// ElementAt
//--------------------------------------------------------------------------------

template<class TList, unsigned int TIndex>
struct ElementAt;

template<int THead, class TTail>
struct ElementAt<ElementList<THead, TTail>, 0> {
  static constexpr int value = THead;
};

template<int THead, class TTail, unsigned int TIndex>
struct ElementAt<ElementList<THead, TTail>, TIndex> {
  static constexpr int value = ElementAt<TTail, TIndex - 1>::value;
};
//--------------------------------------------------------------------------------
// IndexOf
//--------------------------------------------------------------------------------

template<class TList, int TElement>
struct IndexOf;

template<int TElement>
struct IndexOf<NullType, TElement> {
  static constexpr int value = -1;
};

template<int TElement, class TTail>
struct IndexOf<ElementList<TElement, TTail>, TElement> {
  static constexpr int value = 0;
};

template<int THead, class TTail, int TElement>
struct IndexOf<ElementList<THead, TTail>, TElement> {
private:
  static constexpr int temp = IndexOf<TTail, TElement>::value;

public:
  static constexpr int value = (temp == -1 ? -1 : 1 + temp);
};
//--------------------------------------------------------------------------------
// Append
//--------------------------------------------------------------------------------
// It appends an element to an ElementList
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList, int TElement>
struct AppendImpl;
}

template<class TList, int TElement>
using Append =
    typename NSElementListDetail::AppendImpl<TList, TElement>::Result;

namespace NSElementListDetail {
template<int TElement>
struct AppendImpl<NullType, TElement> {
  using Result = MakeElementList<TElement>;
};

template<int THead, class TTail, int TElement>
struct AppendImpl<ElementList<THead, TTail>, TElement> {
  using Result = ElementList<THead, Append<TTail, TElement>>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// Merge
//--------------------------------------------------------------------------------
// It merges two ElementLists
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList1, class TList2>
struct MergeImpl;
}

template<class TList1, class TList2>
using Merge = typename NSElementListDetail::MergeImpl<TList1, TList2>::Result;

namespace NSElementListDetail {
template<>
struct MergeImpl<NullType, NullType> {
  using Result = NullType;
};

template<int THead, class TTail>
struct MergeImpl<NullType, ElementList<THead, TTail>> {
  using Result = ElementList<THead, TTail>;
};

template<int THead, class TTail, class TList2>
struct MergeImpl<ElementList<THead, TTail>, TList2> {
  using Result = ElementList<THead, Merge<TTail, TList2>>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// Erase
//--------------------------------------------------------------------------------
// Erase first entry of an element if any
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList, int TElement>
struct EraseImpl;
}

template<class TList, int TElement>
using Erase = typename NSElementListDetail::EraseImpl<TList, TElement>::Result;

namespace NSElementListDetail {
template<int TElement>
struct EraseImpl<NullType, TElement> {
  using Result = NullType;
};

template<int TElement, class TTail>
struct EraseImpl<ElementList<TElement, TTail>, TElement> {
  using Result = TTail;
};

template<int THead, class TTail, int TElement>
struct EraseImpl<ElementList<THead, TTail>, TElement> {
  using Result = ElementList<THead, Erase<TTail, TElement>>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// EraseAll
//--------------------------------------------------------------------------------
// Erase all entries of an element
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList, int TElement>
struct EraseAllImpl;
}

template<class TList, int TElement>
using EraseAll =
    typename NSElementListDetail::EraseAllImpl<TList, TElement>::Result;

namespace NSElementListDetail {
template<int TElement>
struct EraseAllImpl<NullType, TElement> {
  using Result = NullType;
};

template<int TElement, class TTail>
struct EraseAllImpl<ElementList<TElement, TTail>, TElement> {
  using Result = EraseAll<TTail, TElement>;
};

template<int THead, class TTail, int TElement>
struct EraseAllImpl<ElementList<THead, TTail>, TElement> {
  using Result = ElementList<THead, EraseAll<TTail, TElement>>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// NoDuplicates
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList>
struct NoDuplicatesImpl;
}

template<class TList>
using NoDuplicates =
    typename NSElementListDetail::NoDuplicatesImpl<TList>::Result;

namespace NSElementListDetail {
template<>
struct NoDuplicatesImpl<NullType> {
  using Result = NullType;
};

template<int THead, class TTail>
struct NoDuplicatesImpl<ElementList<THead, TTail>> {
private:
  using GoodTail = NoDuplicates<TTail>;
  using GoodTailWithoutHead = Erase<GoodTail, THead>;

public:
  using Result = ElementList<THead, GoodTailWithoutHead>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// Replace
//--------------------------------------------------------------------------------
// Replace first entry of TToReplace with TToReplaceWith
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList, int TToReplace, int TToReplaceWith>
struct ReplaceImpl;
}

template<class TList, int TToReplace, int TToReplaceWith>
using Replace =
    typename NSElementListDetail::ReplaceImpl<TList, TToReplace,
                                              TToReplaceWith>::Result;

namespace NSElementListDetail {
template<int TToReplace, int TToReplaceWith>
struct ReplaceImpl<NullType, TToReplace, TToReplaceWith> {
  using Result = NullType;
};

template<int TToReplace, class TTail, int TToReplaceWith>
struct ReplaceImpl<ElementList<TToReplace, TTail>, TToReplace, TToReplaceWith> {
  using Result = ElementList<TToReplaceWith, TTail>;
};

template<int THead, class TTail, int TToReplace, int TToReplaceWith>
struct ReplaceImpl<ElementList<THead, TTail>, TToReplace, TToReplaceWith> {
  using Result = ElementList<THead, Replace<TTail, TToReplace, TToReplaceWith>>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
// ReplaceAll
//--------------------------------------------------------------------------------
// Replace all entries of TToReplace with TToReplaceWith
//--------------------------------------------------------------------------------
namespace NSElementListDetail {
template<class TList, int TToReplace, int TToReplaceWith>
struct ReplaceAllImpl;
}

template<class TList, int TToReplace, int TToReplaceWith>
using ReplaceAll =
    typename NSElementListDetail::ReplaceAllImpl<TList, TToReplace,
                                                 TToReplaceWith>::Result;

namespace NSElementListDetail {
template<int TToReplace, int TToReplaceWith>
struct ReplaceAllImpl<NullType, TToReplace, TToReplaceWith> {
  using Result = NullType;
};

template<int TToReplace, class TTail, int TToReplaceWith>
struct ReplaceAllImpl<ElementList<TToReplace, TTail>, TToReplace,
                      TToReplaceWith> {
  using Result = ElementList<TToReplaceWith,
                             ReplaceAll<TTail, TToReplace, TToReplaceWith>>;
};

template<int THead, class TTail, int TToReplace, int TToReplaceWith>
struct ReplaceAllImpl<ElementList<THead, TTail>, TToReplace, TToReplaceWith> {
  using Result =
      ElementList<THead, ReplaceAll<TTail, TToReplace, TToReplaceWith>>;
};
} // namespace NSElementListDetail
//--------------------------------------------------------------------------------
} // namespace EL
} // namespace NSLibrary
#endif // ELEMENTLIST_H
