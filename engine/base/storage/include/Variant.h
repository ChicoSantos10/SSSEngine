/*  SSS Engine
    Copyright (C) 2025  Francisco Santos

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

/**
 * @file
 * @brief
 */

#pragma once

#include "Address.h"
#include "Algorithm.h"
#include "Attributes.h"
#include "Concepts.h"
#include "ConversionTraits.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "InitializerList.h"
#include "Invoke.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "Swap.h"
#include "Traits.h"
#include "Types.h"
#include "Utility.h"
#include "ValueConstant.h"
#include "EnableSpecialMembers.h"

namespace SSSEngine
{
    // NOLINTBEGIN(*-explicit-constructor)

    template<typename T>
    concept ValidVariantMemberConcept = !IsLValueReference<T> && ObjectConcept<T> && !IsArray<T> && !IsVoid<T>;

    template<typename... Types>
        requires(sizeof...(Types) > 1) && (ValidVariantMemberConcept<Types> && ...) && AreAllUnique<Types...>
    class Variant;

    SSSENGINE_GLOBAL
    constexpr SizeType InvalidTag = -1;

    struct VariantCookie
    {
    };

    struct VariantIndexCookie
    {
        using Type = VariantIndexCookie;
    };

    template<typename T>
    struct DeduceVisitResult
    {
        using Type = T;
    };

    template<typename Variant>
    struct VariantSizeChecker;

    template<typename Variant>
    struct VariantSizeChecker<const Variant> : VariantSizeChecker<Variant>
    {
    };

    template<typename Variant>
    struct VariantSizeChecker<volatile Variant> : VariantSizeChecker<Variant>
    {
    };

    template<typename Variant>
    struct VariantSizeChecker<const volatile Variant> : VariantSizeChecker<Variant>
    {
    };

    template<typename... Variants>
    struct VariantSizeChecker<VariantSizeChecker<Variants...>> : IntegralConstant<sizeof...(Variants)>
    {
    };

    template<typename Variant>
    SSSENGINE_GLOBAL
    constexpr SizeType VariantSize = VariantSizeChecker<Variant>::Value;

    template<typename... Variants>
    SSSENGINE_GLOBAL
    constexpr SizeType VariantSize<VariantSizeChecker<Variants...>> = sizeof...(Variants);

    template<typename... Variants>
    SSSENGINE_GLOBAL
    constexpr SizeType VariantSize<const VariantSizeChecker<Variants...>> = sizeof...(Variants);

    template<SizeType N, typename Variant>
    struct VariantAlternative;

    template<SizeType N, typename... Types>
        requires(N < sizeof...(Types))
    struct VariantAlternative<N, Variant<Types...>>
    {
        using Type = NthType<N, Types...>;
    };

    template<SizeType N, typename Variant>
    using VariantAlternativeType = VariantAlternative<N, Variant>::Type;

    template<SizeType N, typename Variant>
    struct VariantAlternative<N, const Variant>
    {
        using Type = const VariantAlternativeType<N, Variant>;
    };

    template<SizeType N, typename Variant>
    struct VariantAlternative<N, volatile Variant>
    {
        using Type = volatile VariantAlternativeType<N, Variant>;
    };

    template<SizeType N, typename Variant>
    struct VariantAlternative<N, const volatile Variant>
    {
        using Type = const volatile VariantAlternativeType<N, Variant>;
    };

    template<typename T, SizeType... Dimensions>
    struct MultiArray;

    template<typename T>
    struct MultiArray<T>
    {
        template<typename>
        struct UntagResult : FalseType
        {
            using ElementType = T;
        };

        SSSENGINE_SUPRESS_DIAG_IGNORE_QUALIFIERS

        template<typename... Args>
        struct UntagResult<const void (*)(Args...)> : FalseType
        {
            using ElementType = T;
        };

        SSSENGINE_POP_SUPPRESS_DIAGNOSTIC

        template<typename... Args>
        struct UntagResult<VariantCookie (*)(Args...)> : FalseType
        {
            using ElementType = void (*)(Args...);
        };

        template<typename... Args>
        struct UntagResult<VariantIndexCookie (*)(Args...)> : FalseType
        {
            using ElementType = void (*)(Args...);
        };

        template<typename Result, typename... Args>
        struct UntagResult<DeduceVisitResult<Result> (*)(Args...)> : FalseType
        {
            using ElementType = void (*)(Args...);
        };

        using Result = UntagResult<T>;

        typename Result::ElementType data;

        constexpr const typename Result::ElementType &Access() const
        {
            return data;
        }
    };

    template<typename Return, typename Visitor, typename... Variants, SizeType First, SizeType... Rest>
    struct MultiArray<Return (*)(Visitor, Variants...), First, Rest...>
    {
        static constexpr SizeType Index = sizeof...(Variants) - sizeof...(Rest) - 1;

        using Variant = NthType<Index, Variants...>;

        using T = Return (*)(Visitor, Variants...);

        template<typename... Args>
        constexpr decltype(auto) Access(SizeType firstIndex, Args... restIndices) const
        {
            return data[firstIndex + 1].Access(restIndices...);
        }

        MultiArray<T, Rest...> data[First + 1];
    };

    template<typename ArrayType, typename IndexSequence>
    struct VTableGeneratorImpl;

    template<typename Result, typename Visitor, SizeType... Dimensions, typename... Variants, SizeType... Indices>
    struct VTableGeneratorImpl<MultiArray<Result (*)(Visitor, Variants...), Dimensions...>, IndexSequence<Indices...>>
    {
        using Next = RemoveReferenceType<NthType<sizeof...(Indices), Variants...>>;
        using ArrayType = MultiArray<Result (*)(Visitor, Variants...), Dimensions...>;

        static constexpr ArrayType Apply()
        {
            ArrayType vTable{};
            ApplyAllAlts(vTable, MakeIndexSequence<VariantSize<Next>>());
            return vTable;
        }

        template<SizeType... VariantIndices>
        static constexpr void ApplyAllAlts(ArrayType &vTable, IndexSequence<VariantIndices...>)
        {
            (ApplySingleAlt<VariantIndices>(vTable.data[VariantIndices + 1], &(vTable.data[0])), ...);
        }

        template<SizeType Index, typename T>
        static constexpr void ApplySingleAlt(T &element, T *cookie = nullptr)
        {
            element = VTableGeneratorImpl<T, IndexSequence<Indices..., Index>>::Apply();
            *cookie = VTableGeneratorImpl<T, IndexSequence<Indices..., InvalidTag>>::Apply();
        }
    };

    template<typename Result, typename Visitor, typename... Variants, SizeType... Indices>
    struct VTableGeneratorImpl<MultiArray<Result (*)(Visitor, Variants...)>, IndexSequence<Indices...>>
    {
        using ArrayType = MultiArray<Result (*)(Visitor, Variants...)>;

        template<SizeType Index, typename Variant>
        static constexpr decltype(auto) ElementByIndexOrCookie(Variant &&variant) noexcept
        {
            if constexpr(Index != InvalidTag)
            {
                return Get<Index>(Forward<Variant>(variant));
            }
            else
            {
                return VariantCookie{};
            }
        }

        static constexpr decltype(auto) VisitInvoke(Visitor &&visitor, Variants... variants)
        {
            if constexpr(IsSameType<Result, VariantIndexCookie>)
            {
                Invoke(Forward<Visitor>(visitor),
                       ElementByIndexOrCookie<Indices>(Forward<Variants>(variants))...,
                       IntegralConstant<Indices>()...);
            }
            else if constexpr(IsSameType<Result, VariantCookie>)
            {
                Invoke(Forward<Visitor>(visitor), ElementByIndexOrCookie<Indices>(Forward<Variants>(variants))...);
            }
            else if constexpr(ArrayType::Result::Value)
            {
                return Invoke(Forward<Visitor>(visitor), ElementByIndexOrCookie<Indices>(Forward<Variants>(variants))...);
            }
            else
            {
                return Invoke<Result>(Forward<Visitor>(visitor),
                                      ElementByIndexOrCookie<Indices>(Forward<Variants>(variants))...);
            }
        }

        static constexpr auto Apply()
        {
            if constexpr(ArrayType::Result::Value)
            {
                constexpr bool ReturnTypeMismatch =
                    !IsSameType<typename Result::Type, decltype(VisitInvoke(DeclVal<Visitor>(), DeclVal<Variants>()...))>;

                if constexpr(ReturnTypeMismatch)
                {
                    struct NoMatch
                    {
                    };

                    return NoMatch{};
                }
                else
                {
                    return ArrayType{&VisitInvoke};
                }
            }
            else
            {
                return ArrayType{&VisitInvoke};
            }
        }
    };

    template<typename Result, typename Visitor, typename... Variants>
    struct VTableGenerator
    {
        using ArrayType = MultiArray<Result (*)(Visitor, Variants...), VariantSize<RemoveReferenceType<Variants>>...>;

        static constexpr ArrayType VTable = VTableGeneratorImpl<ArrayType, IndexSequence<>>::Apply();
    };

    template<SizeType N>
    struct IndexType
    {
        explicit IndexType() = default;
    };

    template<SizeType N>
    SSSENGINE_GLOBAL
    constexpr IndexType<N>
        Index{};

    template<bool, typename...>
    union VariadicUnion
    {
        VariadicUnion() = default;

        template<SizeType N, typename... Args>
        VariadicUnion(IndexType<N>, Args &&...args) = delete;
    };

    template<bool TriviallyDestructible, typename First, typename... Rest>
    union VariadicUnion<TriviallyDestructible, First, Rest...>
    {
        constexpr VariadicUnion() : rest() {}

        template<typename... Args>
        constexpr VariadicUnion(IndexType<0>, Args &&...args) : first(Index<0>,
                                                                      Forward<Args>(args)...) // NOLINT
        {
        }

        template<SizeType N, typename... Args>
        constexpr VariadicUnion(IndexType<N>, Args &&...args) : rest(Index<N - 1>,
                                                                     Forward<Args>(args)...) // NOLINT
        {
        }

        VariadicUnion(const VariadicUnion &) = default;
        VariadicUnion(VariadicUnion &&) = default;
        VariadicUnion &operator=(const VariadicUnion &) = default;
        VariadicUnion &operator=(VariadicUnion &&) = default;
        ~VariadicUnion() = default;

        constexpr ~VariadicUnion()
            requires(!TriviallyDestructible)
        {
        }

        First first;
        VariadicUnion<TriviallyDestructible, Rest...> rest;
    };

    template<typename...>
    struct IsVariadicUnion : FalseType
    {
    };

    template<bool TriviallyDestructible, typename... Args>
    struct IsVariadicUnion<VariadicUnion<TriviallyDestructible, Args...>> : TrueType
    {
    };

    template<typename T>
    concept VariadicUnionConcept = IsVariadicUnion<T>::Value;

    template<SizeType N, VariadicUnionConcept Union>
    consteval decltype(auto) GetElementAt(Union &&u)
    {
        if constexpr(N == 0)
        {
            return Forward<Union>(u).first;
        }
        else if constexpr(N == 1)
        {
            return Forward<Union>(u).rest.first;
        }
        else if constexpr(N == 2)
        {
            return Forward<Union>(u).rest.rest.first;
        }
        else if constexpr(N >= 3)
        {
            return GetElementAt<N - 3>(Forward<Union>(u).rest.rest.rest);
        }
    }

    template<SizeType N, typename Variant>
    constexpr decltype(auto) GetImpl(Variant &&variant) noexcept
    {
        return GetElementAt<N>(Forward<Variant>(variant).types);
    }

    template<SizeType N, VariadicUnionConcept Union>
    consteval decltype(auto) ConstructElementsUntil(Union &&u)
    {
        if constexpr(N == 0)
        {
            return &u.first;
        }
        else if constexpr(N == 1)
        {
            BraceConstructAt<decltype(u.rest)>(AdressOf(u.rest));
            return &u.rest.first;
        }
        else if constexpr(N == 2)
        {
            BraceConstructAt<decltype(u.rest)>(AdressOf(u.rest));
            BraceConstructAt<decltype(u.rest.rest)>(AdressOf(u.rest.rest));
            return &u.rest.rest.first;
        }
        else if constexpr(N >= 3)
        {
            BraceConstructAt<decltype(u.rest)>(AdressOf(u.rest));
            BraceConstructAt<decltype(u.rest.rest)>(AdressOf(u.rest.rest));
            BraceConstructAt<decltype(u.rest.rest.rest)>(AdressOf(u.rest.rest.rest));
            return ConstructElementsUntil<N - 3>(u.rest.rest.rest);
        }
    }

    /**
     * @brief Defines the common traits between variadic types.
     *
     * @tparam Types The types to check
     */
    template<typename... Types>
    struct VariadicUnionTypesCommonTraits
    {
        using FirstType = NthType<0, Types...>;

        static constexpr bool DefaultConstructible = IsDefaultConstructible<FirstType>;
        static constexpr bool CopyConstructible = (IsCopyConstructible<Types> && ...);
        static constexpr bool MoveConstructible = (IsMoveConstructible<Types> && ...);
        static constexpr bool CopyAssignable = CopyConstructible && (IsCopyAssignable<Types> && ...);
        static constexpr bool MoveAssignable = MoveConstructible && (IsMoveAssignable<Types> && ...);

        static constexpr bool TrivialDestructor = (IsTriviallyDestructible<Types> && ...);
        static constexpr bool TrivialCopyConstructor = (IsTriviallyCopyConstructible<Types> && ...);
        static constexpr bool TrivialMoveConstructor = (IsTriviallyMoveConstructible<Types> && ...);
        static constexpr bool TrivialCopyAssignment =
            TrivialDestructor && TrivialCopyConstructor && (IsTriviallyCopyAssignable<Types> && ...);
        static constexpr bool TrivialMoveAssignment =
            TrivialDestructor && TrivialMoveAssignment && (IsTriviallyMoveAssignable<Types> && ...);

        static constexpr bool NoThrowDefaultConstructor = IsNoThrowDefaultConstructible<FirstType>;
        static constexpr bool NoThrowCopyConstructor = (IsNoThrowCopyConstructible<Types> && ...);
        static constexpr bool NoThrowMoveConstructor = (IsNoThrowMoveConstructible<Types> && ...);
        static constexpr bool NoThrowCopyAssignment = NoThrowCopyConstructor && (IsNoThrowCopyAssignable<Types> && ...);
        static constexpr bool NoThrowMoveAssignment = NoThrowMoveConstructor && (IsNoThrowMoveAssignable<Types> && ...);
        static constexpr bool NoThrowSwappable = (IsNoThrowSwappable<Types> && ...);
    };

    template<typename Result, typename Visitor, typename... Variants>
    constexpr decltype(auto) VisitImpl(Visitor &&visitor, Variants &&...args);

    template<typename... Types, typename T>
    constexpr decltype(auto) VariantCast(T &&rhs)
    {
        if constexpr(IsLValueReference<T>)
        {
            if constexpr(IsConst<RemoveReferenceType<T>>)
            {
                return static_cast<const Variant<Types...> &>(rhs);
            }
            else
            {
                return static_cast<Variant<Types...> &>(rhs);
            }
        }
        else
        {
            return static_cast<Variant<Types...> &&>(rhs);
        }
    }

    template<SizeType MaxAlignment, SizeType Count>
    struct TypeSelector
    {
      private:
        static consteval auto SelectType()
        {
            if constexpr(MaxAlignment == 1 && Count < u8(-1))
            {
                return u8{};
            }
            else if constexpr(MaxAlignment <= 2 && Count < u16(-1))
            {
                return u16{};
            }
            else if constexpr(MaxAlignment <= 4 && Count < u32(-1))
            {
                return u32{};
            }
            else
            {
                return u64{};
            }
        }

      public:
        using Type = decltype(SelectType());
    };

    template<typename... Types>
    struct VariantStorage // NOLINT(cppcoreguidelines-special-member-functions)
    {
        using TagType = TypeSelector<Iterators::Max({alignof(Types)...}), sizeof...(Types)>::Type;
        using Traits = VariadicUnionTypesCommonTraits<Types...>;

        constexpr VariantStorage() : index(static_cast<TagType>(InvalidTag)) {}

        template<SizeType N, typename... Args>
        constexpr VariantStorage(IndexType<N>, Args &&...args) : types(Index<N>, Forward<Args>(args)...), index{N}
        {
        }

        constexpr void Destroy()
        {
            if constexpr(!Traits::TrivialDestructor)
            {
                if(!Valid()) SSSENGINE_UNLIKELY
                {
                    return;
                }

                VisitImpl<void>([](auto &&value) { DestroyAt(AddressOf(value)); }, VariantCast<Types...>(*this));
            }

            index = static_cast<TagType>(InvalidTag);
        }

        constexpr ~VariantStorage()
            requires(!Traits::TrivialDestructor)
        {
            Destroy();
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool Valid() const noexcept
        {
            return index != TagType(InvalidTag);
        }

        VariadicUnion<Traits::TrivialDestructor, Types...> types;
        TagType index;
    };

    template<typename Result, typename Visitor, typename... Variants>
    constexpr decltype(auto) VisitImpl(Visitor &&visitor, Variants &&...args)
    {
        if constexpr(sizeof...(Variants) == 0)
        {
            if constexpr(IsSameType<Result, void>)
            {
                return SSSENGINE_DISCARD Invoke(Forward<Visitor>(visitor));
            }
            else
            {
                return Invoke(Forward<Visitor>(visitor));
            }
        }

        constexpr SizeType Max = 11;

        using FirstType = NthType<0, Variants...>;
        constexpr auto N = VariantSize<RemoveReferenceType<FirstType>>;

        if constexpr(sizeof...(Variants) > 1 || N > Max)
        {
            constexpr auto &VTable = VTableGenerator<Result, Visitor &&, Variants &&...>::VTable;
            auto function = VTable.Access(args.Index()...);
            return (*function)(Forward<Visitor>(visitor), Forward<Variants>(args)...);
        }
        else
        {
            FirstType &first = [](FirstType &f, ...) -> FirstType & { return f; }(args...);

            using Array = MultiArray<Result (*)(Visitor &&, FirstType &&)>;

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SSSENGINE_VISIT(N)                                                                                             \
    case N:                                                                                                            \
    {                                                                                                                  \
        return VTableGeneratorImpl<Array, IndexSequence<(N)>>::VisitInvoke(Forward<Visitor>(visitor),                  \
                                                                           Forward<FirstType>(first));                 \
    }
            switch(first.Index())
            {
                SSSENGINE_VISIT(0)
                SSSENGINE_VISIT(1)
                SSSENGINE_VISIT(2)
                SSSENGINE_VISIT(3)
                SSSENGINE_VISIT(4)
                SSSENGINE_VISIT(5)
                SSSENGINE_VISIT(6)
                SSSENGINE_VISIT(7)
                SSSENGINE_VISIT(8)
                SSSENGINE_VISIT(9)
                SSSENGINE_VISIT(10)
                case InvalidTag:
                {
                    if constexpr(IsSameType<Result, VariantIndexCookie> || IsSameType<Result, VariantCookie>)
                    {
                        VisitInvoke(Forward<Visitor>(visitor), Forward<FirstType>(first));
                    }
                    else
                    {
                        SSSENGINE_UNREACHABLE;
                    }
                }
                default:
                    SSSENGINE_UNREACHABLE;
            }
#undef SSSENGINE_VIST
        }
    }

    template<typename... Types>
    constexpr Variant<Types...> &As(Variant<Types...> &tagUnion) noexcept
    {
        return tagUnion;
    }

    template<typename... Types>
    constexpr const Variant<Types...> &As(const Variant<Types...> &tagUnion) noexcept
    {
        return tagUnion; // NOLINT(bugprone-return-const-ref-from-parameter)
    }

    template<typename... Types>
    constexpr Variant<Types...> &&As(Variant<Types...> &&tagUnion) noexcept
    {
        return Move(tagUnion);
    }

    template<typename... Types>
    constexpr const Variant<Types...> &&As(const Variant<Types...> &&tagUnion) noexcept
    {
        return Move(tagUnion);
    }

    template<SizeType N, typename Variant, typename AsType = decltype(As(DeclVal<Variant>())), typename T = VariantAlternative<N, RemoveReferenceType<AsType>>>
    using GetT = ConditionalType<IsLValueReference<Variant>, T &, T &&>;

    template<typename Visitor, typename... Variants>
    using VisitResultType = InvokeResultType<Visitor, GetT<0, Variants>...>;

    template<typename Visitor, typename Variant, SizeType... Indices>
    constexpr bool CheckVisitorResults(IndexSequence<Indices...>)
    {
        return AllSameType<InvokeResultType<Visitor, GetT<Indices, Variant>>...>;
    }

    template<typename Visitor, typename... Variants>
    SSSENGINE_GLOBAL
    constexpr VisitResultType<Visitor, Variants...> Visit(Visitor &&visitor, Variants... variants)
    {
        // INVESTIGATE: What to do when variants are not valid
        SSSENGINE_ASSERT(!(As(variants).Valid() || ...));

        using ReturnType = VisitResultType<Visitor, Variants...>;
        using Tag = DeduceVisitResult<ReturnType>;

        if constexpr(sizeof...(Variants) == 1)
        {
            using T = decltype(As(DeclVal<Variants>()...));

            constexpr bool ReturnTypesMatch =
                CheckVisitorResults<Visitor, T>(MakeIndexSequence<VariantSize<RemoveReferenceType<T>>>());

            if constexpr(!ReturnTypesMatch)
            {
                SSSENGINE_STATIC_ASSERT(ReturnTypesMatch, "Visitor must have the same return type for all alternatives");
                return;
            }
            else
            {
                return VisitImpl<Tag>(Forward<Visitor>(visitor), static_cast<T>(variants)...);
            }
        }
        else
        {
            return VisitImpl<Tag>(Forward<Visitor>(visitor), As(Forward<Variants>(variants))...);
        }
    }

    template<typename ReturnType, typename Visitor, typename... Variants>
    SSSENGINE_GLOBAL
    constexpr ReturnType Visit(Visitor &&visitor, Variants &&...variants)
    {
        SSSENGINE_ASSERT((As(variants).Valid() || ...));

        return VisitImpl<ReturnType>(Forward<Visitor>(visitor), As(Forward<Variants>(variants))...);
    }

    namespace SSSENGINE_HIDDEN VariantImpl
    {
        // NOLINTBEGIN(cppcoreguidelines-special-member-functions)

        template<typename... Types>
        using VariantStorageType = VariantStorage<Types...>;

        template<typename... Types>
        using Traits = VariadicUnionTypesCommonTraits<Types...>;

        template<typename Visitor, typename... Variants>
        SSSENGINE_FORCE_INLINE
        constexpr void RawIndexVisit(Visitor &&visitor, Variants &&...variants)
        {
            VisitImpl<VariantIndexCookie>(Forward<Visitor>(visitor), Forward<Variants>(variants)...);
        }

        template<SizeType N, typename... Types, typename... Args>
        SSSENGINE_GLOBAL
        constexpr void Emplace(VariantStorage<Types...> &storage, Args &&...args)
        {
            storage.Reset();

            auto *address = ConstructElementsUntil<N>(storage.types);
            BraceConstructAt(address, Index<0>, Forward<Args>(args)...);

            storage.index = N;
        }

        template<bool, typename... Types>
        struct CopyCtor : VariantStorageType<Types...>
        {
            using Base = VariantStorageType<Types...>;
            using Base::Base;

            constexpr CopyCtor(const CopyCtor &rhs) noexcept(Traits<Types...>::NoThrowCopyConstructor)
            {
                RawIndexVisit(
                    [this](auto &&type, auto index)
                    {
                        constexpr SizeType I = index;
                        if constexpr(I != InvalidTag)
                        {
                            BraceConstructAt(AddressOf(this->types), Index<I>, type);
                        }
                    },
                    VariantCast<Types...>(rhs));

                this->index = rhs.index;
            }

            constexpr CopyCtor(CopyCtor &&) = default;
            constexpr CopyCtor &operator=(const CopyCtor &) = default;
            constexpr CopyCtor &operator=(CopyCtor &&) = default;
        };

        template<typename... Types>
        struct CopyCtor<true, Types...> : VariantStorageType<Types...>
        {
            using Base = VariantStorageType<Types...>;
            using Base::Base;
        };

        template<typename... Types>
        using CopyCtorType = CopyCtor<Traits<Types...>::TrivialCopyConstructor, Types...>;

        template<bool, typename... Types>
        struct MoveCtor : CopyCtorType<Types...>
        {
            using Base = CopyCtorType<Types...>;
            using Base::Base;

            constexpr MoveCtor(MoveCtor &&rhs) noexcept(Traits<Types...>::NoThrowMoveConstructor)
            {
                RawIndexVisit(
                    [this](auto &&type, auto index)
                    {
                        constexpr SizeType I = index;
                        if constexpr(I != InvalidTag)
                        {
                            BraceConstructAt(AddressOf(this->types), Index<I>, Forward<decltype(type)>(type));
                        }
                    },
                    VariantCast<Types...>(Move(rhs)));

                this->index = rhs.index;
            }

            constexpr MoveCtor(const MoveCtor &) = default;
            constexpr MoveCtor &operator=(const MoveCtor &) = default;
            constexpr MoveCtor &operator=(MoveCtor &&) = default;
        };

        template<typename... Types>
        struct MoveCtor<true, Types...> : CopyCtorType<Types...>
        {
            using Base = CopyCtorType<Types...>;
            using Base::Base;
        };

        template<typename... Types>
        using MoveCtorType = MoveCtor<Traits<Types...>::TrivialMoveConstructor, Types...>;

        template<bool, typename... Types>
        struct CopyAssignment : MoveCtorType<Types...>
        {
            using Base = MoveCtorType<Types...>;
            using Base::Base;

            constexpr CopyAssignment &operator=(const CopyAssignment &rhs) noexcept(Traits<Types...>::NoThrowCopyAssignment)
            {
                RawIndexVisit(
                    [this](auto &&type, auto index)
                    {
                        constexpr SizeType I = index;
                        if constexpr(I == InvalidTag)
                        {
                            this->Reset();
                        }
                        else if(this->index == I)
                        {
                            GetImpl<I>(*this) = type;
                        }
                        else
                        {
                            using T = NthType<I, Types...>;
                            if constexpr(IsNoThrowCopyConstructible<T> || !IsNoThrowMoveConstructible<T>)
                            {
                                Emplace<I>(*this, type);
                            }
                            else
                            {
                                using UnionType = Variant<Types...>;
                                UnionType &self = VariantCast<Types...>(*this);
                                self = UnionType(Index<I>, type);
                            }
                        }
                    },
                    VariantCast<Types...>(rhs));

                return *this;
            }

            CopyAssignment(const CopyAssignment &) = default;
            CopyAssignment(CopyAssignment &&) = default;
            CopyAssignment &operator=(CopyAssignment &&) = default;
        };

        template<typename... Types>
        struct CopyAssignment<true, Types...> : MoveCtorType<Types...>
        {
            using Base = MoveCtorType<Types...>;
            using Base::Base;
        };

        template<typename... Types>
        using CopyAssignmentType = CopyAssignment<Traits<Types...>::TrivialCopyAssignment, Types...>;

        template<bool, typename... Types>
        struct MoveAssignment : CopyAssignmentType<Types...>
        {
            using Base = CopyAssignmentType<Types...>;
            using Base::Base;

            constexpr MoveAssignment &operator=(MoveAssignment &&rhs) noexcept(Traits<Types...>::NoThrowMoveAssignment)
            {
                RawIndexVisit(
                    [this](auto &&type, auto index)
                    {
                        constexpr SizeType I = index;
                        if constexpr(I == InvalidTag)
                        {
                            this->Reset();
                        }
                        else if(this->index == I)
                        {
                            GetImpl<I>(*this) = Move(type);
                        }
                        else
                        {
                            using T = NthType<I, Types...>;
                            if constexpr(IsNoThrowMoveConstructible<T>)
                            {
                                Emplace<I>(*this, Move(type));
                            }
                            else
                            {
                                using UnionType = Variant<Types...>;
                                UnionType &self = VariantCast<Types...>(*this);
                                self.template Emplace<I>(Move(type));
                            }
                        }
                    },
                    VariantCast<Types...>(rhs));

                return *this;
            }

            MoveAssignment(const MoveAssignment &) = default;
            MoveAssignment(MoveAssignment &&) = default;
            MoveAssignment &operator=(const MoveAssignment &) = default;
        };

        template<typename... Types>
        struct MoveAssignment<true, Types...> : CopyAssignmentType<Types...>
        {
            using Base = CopyAssignmentType<Types...>;
            using Base::Base;
        };

        template<typename... Types>
        using MoveAssignmentType = MoveAssignment<Traits<Types...>::TrivialMoveAssignment, Types...>;

        template<typename... Types>
        struct VariantBase : MoveAssignmentType<Types...>
        {
            using Base = MoveAssignmentType<Types...>;

            VariantBase() = default;

            template<SizeType N, typename... Args>
            constexpr explicit VariantBase(IndexType<N> index, Args &&...args) : Base(index, Forward<Args>(args)...) {};

            VariantBase(const VariantBase &) = default;
            VariantBase(VariantBase &&) = default;
            VariantBase &operator=(const VariantBase &) = default;
            VariantBase &operator=(VariantBase &&) = default;
        };

        template<SizeType Index, typename T, typename U, typename = void>
        struct FunctionBuilder
        {
            void Function() = delete;
        };

        template<SizeType Index, typename T, typename U>
            requires NonNarrowingConvertibleToConcept<U, T>
        struct FunctionBuilder<Index, T, U>
        {
            static IntegralConstant<Index> Function(U);
        };

        template<typename T, typename Variant, typename = MakeIndexSequence<VariantSize<Variant>>>
        struct FunctionsBuilder;

        template<typename T, typename... Types, SizeType... Indices>
        struct FunctionsBuilder<T, Variant<Types...>, IndexSequence<Indices...>> : FunctionBuilder<Indices, T, Types>...
        {
            using FunctionBuilder<Indices, T, Types>::Function...;
        };

        template<typename T, typename Variant>
        using FunctionType = decltype(FunctionsBuilder<T, Variant>::Function(DeclVal<T>()));

        template<typename T, typename Variant, typename = void>
        SSSENGINE_GLOBAL
        constexpr SizeType AcceptedIndex = InvalidTag;

        template<typename T, typename Variant>
        SSSENGINE_GLOBAL
        constexpr SizeType AcceptedIndex<T, Variant, VoidType<FunctionType<T, Variant>>> = FunctionType<T, Variant>::Value;

        // NOLINTEND(cppcoreguidelines-special-member-functions)
        // NOLINTEND(*-explicit-constructor)
    } // namespace SSSENGINE_HIDDEN VariantImpl

    template<typename... Types>
        requires(sizeof...(Types) > 1) && (ValidVariantMemberConcept<Types> && ...) && AreAllUnique<Types...>
    class Variant final :
        private VariantImpl::VariantBase<Types...>,
        private EnableCopyMove<VariantImpl::Traits<Types...>::CopyConstructible,
                               VariantImpl::Traits<Types...>::CopyAssignable,
                               VariantImpl::Traits<Types...>::MoveConstructible,
                               VariantImpl::Traits<Types...>::MoveAssignable,
                               Variant<Types...>>
    {
      private:
        using Base = VariantImpl::VariantBase<Types...>;

        template<typename T>
        static constexpr SizeType AcceptedIndex = VariantImpl::AcceptedIndex<T, Variant>;

        template<SizeType N>
        using TypeByIndex = NthType<N, Types...>;

        template<typename T>
        using UnionType = TypeByIndex<AcceptedIndex<T>>;

        template<typename T>
        static constexpr SizeType IndexOf = FindPackedArgIndex<SizeType, T, Types...>;

      public:
        using Traits = VariadicUnionTypesCommonTraits<Types...>;
        using TagType = VariantStorage<Types...>::TagType;

        Variant()
            requires(Traits::DefaultConstructible)
        = default;

        constexpr Variant(const Variant &) = default;
        constexpr Variant(Variant &&) = default;
        constexpr Variant &operator=(const Variant &) = default;
        constexpr Variant &operator=(Variant &&) = default;
        constexpr ~Variant() = default;

        template<SizeType I, typename... Args, typename UnionType = UnionType<TypeByIndex<I>>>
        constexpr explicit Variant(IndexType<I>, Args &&...args) : Base(Index<I>, Forward<Args>(args)...)
        {
        }

        template<typename T, typename UnionType = UnionType<T>>
            requires IsConstructible<UnionType, T>
        constexpr explicit Variant(T &&value) noexcept(IsNoThrowConstructible<UnionType, T>) :
            Variant(Index<AcceptedIndex<T>>, Forward<T>(value))
        {
        }

        template<typename T, typename... Args>
            requires IsConstructible<T, Args...>
        constexpr explicit Variant(TypeTag<T>, Args &&...args) : Variant(Index<IndexOf<T>>, Forward<Args>(args)...)
        {
        }

        template<typename T, typename U, typename... Args>
            requires IsConstructible<T, InitializerList<U> &, Args...>
        constexpr explicit Variant(TypeTag<T>, InitializerList<U> list, Args &&...args) :
            Variant(Index<IndexOf<T>>, list, Forward<Args>(args)...)
        {
        }

        template<SizeType N, typename T, typename... Args, typename UnionType = UnionType<TypeByIndex<N>>>
            requires IsConstructible<UnionType, InitializerList<T> &, Args...>
        constexpr explicit Variant(IndexType<N>, InitializerList<T> list, Args &&...args) :
            Base(Index<N>, list, Forward<Args>(args)...)
        {
        }

        template<typename T, typename UnionType = UnionType<T &&>>
            requires(IsConstructible<UnionType, T> && IsAssignable<UnionType, T>)
        Variant &operator=(T &&value) noexcept(IsNoThrowAssignable<UnionType &, T> && IsNoThrowConstructible<UnionType, T>)
        {
            constexpr SizeType Index = AcceptedIndex<T>;

            if(GetIndex() == Index)
            {
                GetImpl<Index>(*this) = Forward<T>(value);
            }
            else
            {
                if constexpr(IsNoThrowConstructible<UnionType, T> || !IsNoThrowMoveConstructible<UnionType>)
                {
                    this->Emplace<Index>(Forward<T>(value));
                }
                else
                {
                    this->Emplace<Index>(T(Forward<T>(value)));
                }
            }

            return *this;
        }

        template<typename T, typename... Args>
            requires IsConstructible<T, Args...>
        T &Emplace(Args &&...args)
        {
            constexpr SizeType Index = IndexOf<T>;
            Emplace<Index>(Forward<Args>(args)...);
        }

        template<typename T, typename U, typename... Args>
            requires IsConstructible<T, InitializerList<U> &, Args...>
        T &Emplace(InitializerList<U> list, Args &&...args)
        {
            constexpr SizeType Index = IndexOf<T>;
            Emplace<Index>(list, Forward<Args>(args)...);
        }

        template<SizeType N, typename... Args>
            requires IsConstructible<TypeByIndex<N>, Args...>
        TypeByIndex<N> &Emplace(Args &&...args)
        {
            using Type = TypeByIndex<N>;

            if constexpr(IsNoThrowConstructible<Type, Args...>) // NOLINTNEXTLINE
            {
                VariantImpl::Emplace<N>(*this, Forward<Args>(args)...);
            }
            else if constexpr(IsScalar<Type>)
            {
                const auto tmp(Forward<Args>(args)...);
                VariantImpl::Emplace<N>(*this, tmp);
            }
            else if constexpr(Traits::MoveAssignable)
            {
                Variant tmp(Index<N>, Forward<Args>(args)...);
                VariantImpl::Emplace<N>(*this, tmp);
            }
            else
            {
                VariantImpl::Emplace<N>(*this, Forward<Args>(args)...);
            }

            return GetImpl<N>(*this);
        }

        template<SizeType N, typename U, typename... Args>
            requires IsConstructible<TypeByIndex<N>, InitializerList<U> &, Args...>
        TypeByIndex<N> &Emplace(InitializerList<U> list, Args &&...args)
        {
            using Type = TypeByIndex<N>;

            if constexpr(IsNoThrowConstructible<Type, InitializerList<U> &, Args...>) // NOLINTNEXTLINE
            {
                VariantImpl::Emplace<N>(*this, list, Forward<Args>(args)...);
            }
            else if constexpr(Traits::MoveAssignable)
            {
                Variant tmp(Index<N>, list, Forward<Args>(args)...);
                VariantImpl::Emplace<N>(*this, tmp);
            }
            else
            {
                VariantImpl::Emplace<N>(*this, list, Forward<Args>(args)...);
            }

            return GetImpl<N>(*this);
        }

        template<SizeType N, typename... Args>
            requires(N >= sizeof...(Types))
        void Emplace(Args &&...) = delete;

        constexpr SizeType GetIndex()
        {
            if constexpr(sizeof...(Types) <= TagType(-1) / 2)
            {
                return MakeSigned<TagType>(this->index);
            }
            else
            {
                return SizeType(TagType(this->index + 1) - 1);
            }
        }

        constexpr void Swap(Variant rhs) noexcept(Traits::NoThrowSwappable)
            requires Traits::MoveConstructible
        {
            if(!rhs.Valid()) SSSENGINE_UNLIKELY
            {
                if(this->Valid()) SSSENGINE_LIKELY
                {
                    rhs.Swap(*this);
                }
                return;
            }

            VariantImpl::RawIndexVisit(
                [this, &rhs](auto &&rhsValue, auto rhsIndex)
                {
                    constexpr SizeType Index = rhsIndex;
                    if(this->GetIndex() == Index)
                    {
                        Swap(GetImpl<Index>(*this), rhsValue);
                    }
                    else
                    {
                        auto tmp(Move(rhsValue));

                        if constexpr(Traits::TrivialMoveAssignment)
                        {
                            rhs = Move(*this);
                        }
                        else
                        {
                            VariantImpl::RawIndexVisit(
                                [&rhs](auto &&value, auto index)
                                {
                                    constexpr SizeType ThisIndex = index;
                                    if constexpr(ThisIndex != InvalidTag)
                                    {
                                        VariantImpl::Emplace<ThisIndex>(rhs, Move(value));
                                    }
                                },
                                *this);
                        }
                        VariantImpl::Emplace<Index>(*this, Move(tmp));
                    }
                },
                rhs);
        }

        template<typename Self, typename Visitor>
        constexpr decltype(auto) Visit(this Self &&self, Visitor &&visitor)
        {
            SSSEngine::Visit(Forward<Visitor>(visitor), Forward<Self>(self));
        }

        template<typename ReturnType, typename Self, typename Visitor>
        constexpr ReturnType Visit(this Self &&self, Visitor &&visitor)
        {
            SSSEngine::Visit<ReturnType>(Forward<Visitor>(visitor), Forward<Self>(self));
        }

      private:
        template<SizeType N, typename Variant>
        friend constexpr decltype(auto) GetImpl(Variant &&) noexcept;
    };

    template<SizeType N, typename... Types>
        requires(N < sizeof...(Types))
    constexpr VariantAlternative<N, Variant<Types...>> &Get(Variant<Types...> &tagUnion)
    {
        SSSENGINE_ASSERT(tagUnion.GetIndex() != InvalidTag);

        return GetImpl<N>(tagUnion);
    }

    template<SizeType N, typename... Types>
        requires(N < sizeof...(Types))
    constexpr VariantAlternative<N, Variant<Types...>> &&Get(Variant<Types...> &&tagUnion)
    {
        SSSENGINE_ASSERT(tagUnion.GetIndex() != InvalidTag);

        GetImpl<N>(Move(tagUnion));
    }

    template<SizeType N, typename... Types>
        requires(N < sizeof...(Types))
    constexpr VariantAlternative<N, Variant<Types...>> &&Get(const Variant<Types...> &tagUnion)
    {
        SSSENGINE_ASSERT(tagUnion.GetIndex() != InvalidTag);

        GetImpl<N>(tagUnion);
    }

    template<SizeType N, typename... Types>
        requires(N < sizeof...(Types))
    constexpr VariantAlternative<N, Variant<Types...>> &&Get(const Variant<Types...> &&tagUnion)
    {
        SSSENGINE_ASSERT(tagUnion.GetIndex() != InvalidTag);

        GetImpl<N>(Move(tagUnion));
    }

    // TODO: Improve with more modern C++:
    //  - Remove the Enable Copy Move structs inheritance
    //  - ...
    // TODO: Hash function/object for Variant

} // namespace SSSEngine
