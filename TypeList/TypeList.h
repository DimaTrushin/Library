#ifndef TYPELIST_H
#define TYPELIST_H

#include "NullType.h"

namespace NSLibrary {
//--------------------------------------------------------------------------------
// TypeSeq
//--------------------------------------------------------------------------------

template<class... TTypes>
struct TypeSeq {};

//--------------------------------------------------------------------------------
// TypeList
//--------------------------------------------------------------------------------
// Empty TypeList is always NullType
// Non empty TypeList always has a non NullType Head
//--------------------------------------------------------------------------------

template<class THead, class TTail>
struct TypeList {
  using Head = THead;
  using Tail = TTail;
};

using EmptyTypeList = NullType;
//--------------------------------------------------------------------------------

namespace TL {
//--------------------------------------------------------------------------------
// MakeTypeList
//--------------------------------------------------------------------------------

namespace NSTypeListDetail {
template<class... TTypes>
struct MakeTypeListImpl;
}

template<class... TTypes>
using MakeTypeList =
    typename NSTypeListDetail::MakeTypeListImpl<TTypes...>::Result;

namespace NSTypeListDetail {
template<class TType, class... TTypes>
struct MakeTypeListImpl<TType, TTypes...> {
  using Result = TypeList<TType, MakeTypeList<TTypes...>>;
};

template<class TType>
struct MakeTypeListImpl<TType> {
  using Result = TypeList<TType, NullType>;
};

template<>
struct MakeTypeListImpl<> {
  using Result = EmptyTypeList;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// MakeTypeListN
//--------------------------------------------------------------------------------
// Make a TypeList populated with TSize copies of the same type TType
//--------------------------------------------------------------------------------

namespace NSTypeListDetail {
template<class TType, unsigned int TSize>
struct MakeTypeListNImpl;
}

template<class TType, unsigned int TSize>
using MakeTypeListN =
    typename NSTypeListDetail::MakeTypeListNImpl<TType, TSize>::Result;

namespace NSTypeListDetail {
template<class TType, unsigned int TSize>
struct MakeTypeListNImpl {
  using Result = TypeList<TType, MakeTypeListN<TType, TSize - 1>>;
};

template<class TType>
struct MakeTypeListNImpl<TType, 0> {
  using Result = EmptyTypeList;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// Seq2List
//--------------------------------------------------------------------------------

namespace NSTypeListDetail {
template<class TTypeSeq>
struct Seq2ListImpl;
}

template<class TTypeSeq>
using Seq2List = typename NSTypeListDetail::Seq2ListImpl<TTypeSeq>::Result;

namespace NSTypeListDetail {
template<class... TTypes>
struct Seq2ListImpl<TypeSeq<TTypes...>> {
  using Result = MakeTypeList<TTypes...>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// MergeSeq
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TSeq1, class TSeq2>
struct MergeSeqImpl;
}

template<class TSeq1, class TSeq2>
using MergeSeq = typename NSTypeListDetail::MergeSeqImpl<TSeq1, TSeq2>::Result;

namespace NSTypeListDetail {
template<class... TTypes1, class... TTypes2>
struct MergeSeqImpl<TypeSeq<TTypes1...>, TypeSeq<TTypes2...>> {
  using Result = TypeSeq<TTypes1..., TTypes2...>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// List2Seq
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList>
struct List2SeqImpl;
}

template<class TList>
using List2Seq = typename NSTypeListDetail::List2SeqImpl<TList>::Result;

namespace NSTypeListDetail {
template<>
struct List2SeqImpl<NullType> {
  using Result = TypeSeq<>;
};

template<class THead, class TTail>
struct List2SeqImpl<TypeList<THead, TTail>> {
  using Result = MergeSeq<TypeSeq<THead>, List2Seq<TTail>>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// MakeTypeSeqN
//--------------------------------------------------------------------------------
// Make a TypeSeq populated with TSize copies of the same type TType
//--------------------------------------------------------------------------------

namespace NSTypeListDetail {
template<class TType, unsigned int TSize>
struct MakeTypeSeqNImpl {
  using Result = List2Seq<MakeTypeListN<TType, TSize>>;
};
} // namespace NSTypeListDetail

template<class TType, unsigned int TSize>
using MakeTypeSeqN =
    typename NSTypeListDetail::MakeTypeSeqNImpl<TType, TSize>::Result;
//--------------------------------------------------------------------------------
// Length
//--------------------------------------------------------------------------------

template<class TList>
struct Length;

template<>
struct Length<NullType> {
  static constexpr int value = 0;
};

template<class THead, class TTail>
struct Length<TypeList<THead, TTail>> {
  static constexpr int value = 1 + Length<TTail>::value;
};
//--------------------------------------------------------------------------------
// TypeAt
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList, unsigned int TIndex>
struct TypeAtImpl;
}

template<class TList, unsigned int TIndex>
using TypeAt = typename NSTypeListDetail::TypeAtImpl<TList, TIndex>::Result;

namespace NSTypeListDetail {
template<class THead, class TTail>
struct TypeAtImpl<TypeList<THead, TTail>, 0> {
  using Result = THead;
};

template<class THead, class TTail, unsigned int TIndex>
struct TypeAtImpl<TypeList<THead, TTail>, TIndex> {
  using Result = TypeAt<TTail, TIndex - 1>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// IndexOf
//--------------------------------------------------------------------------------

template<class TList, class TType>
struct IndexOf;

template<class TType>
struct IndexOf<NullType, TType> {
  static constexpr int value = -1;
};

template<class TType, class TTail>
struct IndexOf<TypeList<TType, TTail>, TType> {
  static constexpr int value = 0;
};

template<class THead, class TTail, class TType>
struct IndexOf<TypeList<THead, TTail>, TType> {
private:
  static constexpr int temp = IndexOf<TTail, TType>::value;

public:
  static constexpr int value = (temp == -1 ? -1 : 1 + temp);
};
//--------------------------------------------------------------------------------
// Append
//--------------------------------------------------------------------------------
// It appends a type to a TypeList or merges two TypeLists
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList, class TType>
struct AppendImpl;
}

template<class TList, class TType>
using Append = typename NSTypeListDetail::AppendImpl<TList, TType>::Result;

namespace NSTypeListDetail {
template<>
struct AppendImpl<NullType, NullType> {
  using Result = NullType;
};

template<class TType>
struct AppendImpl<NullType, TType> {
  using Result = MakeTypeList<TType>;
};

template<class THead, class TTail>
struct AppendImpl<NullType, TypeList<THead, TTail>> {
  using Result = TypeList<THead, TTail>;
};

template<class THead, class TTail, class TType>
struct AppendImpl<TypeList<THead, TTail>, TType> {
  using Result = TypeList<THead, Append<TTail, TType>>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// Erase
//--------------------------------------------------------------------------------
// Erase first entry of a type if any
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList, class TType>
struct EraseImpl;
}

template<class TList, class TType>
using Erase = typename NSTypeListDetail::EraseImpl<TList, TType>::Result;

namespace NSTypeListDetail {
template<class TType>
struct EraseImpl<NullType, TType> {
  using Result = NullType;
};

template<class TType, class TTail>
struct EraseImpl<TypeList<TType, TTail>, TType> {
  using Result = TTail;
};

template<class THead, class TTail, class TType>
struct EraseImpl<TypeList<THead, TTail>, TType> {
  using Result = TypeList<THead, Erase<TTail, TType>>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// EraseAll
//--------------------------------------------------------------------------------
// Erase all entries of a type
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList, class TType>
struct EraseAllImpl;
}

template<class TList, class TType>
using EraseAll = typename NSTypeListDetail::EraseAllImpl<TList, TType>::Result;

namespace NSTypeListDetail {
template<class TType>
struct EraseAllImpl<NullType, TType> {
  using Result = NullType;
};

template<class TType, class TTail>
struct EraseAllImpl<TypeList<TType, TTail>, TType> {
  using Result = EraseAll<TTail, TType>;
};

template<class THead, class TTail, class TType>
struct EraseAllImpl<TypeList<THead, TTail>, TType> {
  using Result = TypeList<THead, EraseAll<TTail, TType>>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// NoDuplicates
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList>
struct NoDuplicatesImpl;
}

template<class TList>
using NoDuplicates = typename NSTypeListDetail::NoDuplicatesImpl<TList>::Result;

namespace NSTypeListDetail {
template<>
struct NoDuplicatesImpl<NullType> {
  using Result = NullType;
};

template<class THead, class TTail>
struct NoDuplicatesImpl<TypeList<THead, TTail>> {
private:
  using GoodTail = NoDuplicates<TTail>;
  using GoodTailWithoutHead = Erase<GoodTail, THead>;

public:
  using Result = TypeList<THead, GoodTailWithoutHead>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// Replace
//--------------------------------------------------------------------------------
// Replace first entry of TToReplace with TToReplaceWith
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList, class TToReplace, class TToReplaceWith>
struct ReplaceImpl;
}

template<class TList, class TToReplace, class TToReplaceWith>
using Replace = typename NSTypeListDetail::ReplaceImpl<TList, TToReplace,
                                                       TToReplaceWith>::Result;

namespace NSTypeListDetail {
template<class TToReplace, class TToReplaceWith>
struct ReplaceImpl<NullType, TToReplace, TToReplaceWith> {
  using Result = NullType;
};

template<class TToReplace, class TTail, class TToReplaceWith>
struct ReplaceImpl<TypeList<TToReplace, TTail>, TToReplace, TToReplaceWith> {
  using Result = TypeList<TToReplaceWith, TTail>;
};

template<class THead, class TTail, class TToReplace, class TToReplaceWith>
struct ReplaceImpl<TypeList<THead, TTail>, TToReplace, TToReplaceWith> {
  using Result = TypeList<THead, Replace<TTail, TToReplace, TToReplaceWith>>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
// ReplaceAll
//--------------------------------------------------------------------------------
// Replace all entries of TToReplace with TToReplaceWith
//--------------------------------------------------------------------------------
namespace NSTypeListDetail {
template<class TList, class TToReplace, class TToReplaceWith>
struct ReplaceAllImpl;
}

template<class TList, class TToReplace, class TToReplaceWith>
using ReplaceAll =
    typename NSTypeListDetail::ReplaceAllImpl<TList, TToReplace,
                                              TToReplaceWith>::Result;

namespace NSTypeListDetail {
template<class TToReplace, class TToReplaceWith>
struct ReplaceAllImpl<NullType, TToReplace, TToReplaceWith> {
  using Result = NullType;
};

template<class TToReplace, class TTail, class TToReplaceWith>
struct ReplaceAllImpl<TypeList<TToReplace, TTail>, TToReplace, TToReplaceWith> {
  using Result =
      TypeList<TToReplaceWith, ReplaceAll<TTail, TToReplace, TToReplaceWith>>;
};

template<class THead, class TTail, class TToReplace, class TToReplaceWith>
struct ReplaceAllImpl<TypeList<THead, TTail>, TToReplace, TToReplaceWith> {
  using Result = TypeList<THead, ReplaceAll<TTail, TToReplace, TToReplaceWith>>;
};
} // namespace NSTypeListDetail
//--------------------------------------------------------------------------------
} // namespace TL
} // namespace NSLibrary
#endif // TYPELIST_H
