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
 * @brief Traits for dealing if conversions between Types
 */

#pragma once

#include "HelperMacros.h"
#include "QualifierTraits.h"
#include "Traits.h"
#include "Types.h"
#include "ValueConstant.h"

namespace SSSEngine
{
    template<typename From, typename To>
    struct ConvertibleChecker : BoolConstant<__is_convertible(From, To)>
    {
    };

    template<typename From, typename To>
    SSSENGINE_GLOBAL
    constexpr bool IsConvertible = ConvertibleChecker<From, To>::Value;

    template<typename... T>
    struct CommonTypeChecker;

    struct CommonTypeImpl
    {
        template<typename T, typename V>
        using DeducedType = decltype(true ? DeclVal<T>() : DeclVal<V>());

        template<typename T, typename V>
        static SuccessType<DecayType<DeducedType<T, V>>> Test(int);

        template<typename T, typename V>
        static SuccessType<RemoveCVReferenceType<DeducedType<T &, V &>>> Test2(int);

        template<typename T, typename V>
        static FailureType Test2(...);

        template<typename T, typename V>
        static decltype(Test2<T, V>(0)) Test(...);
    };

    template<>
    struct CommonTypeChecker<>
    {
    };

    template<typename T>
    struct CommonTypeChecker<T> : public CommonTypeChecker<T, T>
    {
    };

    template<typename T0, typename T1, typename D0 = DecayType<T0>, typename D1 = DecayType<T1>>
    struct CommonTypeHelper
    {
        using Type = CommonTypeChecker<D0, D1>;
    };

    template<typename T0, typename T1>
    struct CommonTypeHelper<T0, T1, T0, T1> : private CommonTypeImpl
    {
        using Type = decltype(Test<T0, T1>(0));
    };

    template<typename T0, typename T1>
    struct CommonTypeChecker<T0, T1> : public CommonTypeHelper<T0, T1>::Type
    {
    };

    template<typename...>
    struct CommonTypePack
    {
    };

    template<typename, typename, typename = void>
    struct CommonTypeFold;

    template<typename T0, typename T1, typename... Rest>
    struct CommonTypeChecker<T0, T1, Rest...> : public CommonTypeFold<CommonTypeChecker<T0, T1>, CommonTypePack<Rest...>>
    {
    };

    template<typename T0, typename... Args>
    struct CommonTypeFold<T0, CommonTypePack<Args...>, VoidType<typename T0::Type>> :
        public CommonTypeChecker<typename T0::Type, Args...>
    {
    };

    template<typename T0, typename T1>
    struct CommonTypeFold<T0, T1, void>
    {
    };

    template<typename... Args>
    using CommonType = typename CommonTypeChecker<Args...>::Type;

    template<typename From, typename To>
    SSSENGINE_GLOBAL
    constexpr bool IsSameType = __is_same(From, To);

    template<typename T, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool IsAnyType = (IsSameType<T, Args> || ...);

    template<typename T, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool AllSameType = (IsSameType<T, Args> && ...);

    template<typename...>
    struct AllUniqueChecker;

    template<>
    struct AllUniqueChecker<>
    {
        static constexpr bool Value = true;
    };

    template<typename T, typename... Args>
    struct AllUniqueChecker<T, Args...>
    {
        static constexpr bool Value = AllUniqueChecker<Args...>::Value && !IsAnyType<T, Args...>;
    };

    template<typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool AreAllUnique = AllUniqueChecker<Args...>::Value;

    template<typename Base, typename Derived>
    SSSENGINE_GLOBAL
    constexpr bool IsBaseOf = __is_base_of(Base, Derived);

    /**
     * @brief Finds a Type from a variadic list of arguments and returns its index
     *
     * @tparam ReturnType The type of the return count
     * @tparam Find The Type to find
     * @tparam Args The Variadic list of arguments to search in
     */
    template<typename ReturnType, typename Find, typename... Args>
    SSSENGINE_GLOBAL
    constexpr ReturnType FindPackedArgIndex = []()
    {
        ReturnType count{0};
        auto _ = ((IsSameType<Find, Args> || (++count, false)) || ...);
        return count;
    }();

    template<SizeType N, typename... Types>
    struct NthTypeFinder
    {
    };

    template<typename First, typename... Rest>
    struct NthTypeFinder<0, First, Rest...>
    {
        using Type = First;
    };

    template<typename First, typename Second, typename... Rest>
    struct NthTypeFinder<1, First, Second, Rest...>
    {
        using Type = Second;
    };

    template<typename First, typename Second, typename Third, typename... Rest>
    struct NthTypeFinder<2, First, Second, Third, Rest...>
    {
        using Type = Third;
    };

    template<SizeType N, typename First, typename Second, typename Third, typename... Rest>
        requires(N >= 3)
    struct NthTypeFinder<N, First, Second, Third, Rest...> : NthTypeFinder<N - 3, Rest...>
    {
    };

    template<SizeType N, typename... Types>
    using NthType = NthTypeFinder<N, Types...>::Type;

    template<typename... T>
    struct CommonReference;

    template<typename... Args>
    using CommonReferenceType = typename CommonReference<Args...>::Type;

    template<>
    struct CommonReference<>
    {
    };

    template<typename T0>
    struct CommonReference<T0>
    {
        using Type = T0;
    };

    template<typename T1, typename T2, int Bullet = 1>
    struct CommonReferenceImpl : CommonReferenceImpl<T1, T2, Bullet + 1>
    {
    };

    template<typename T1, typename T2>
    struct CommonReference<T1, T2> : CommonReferenceImpl<T1, T2>
    {
    };

    template<typename Xp, typename Yp>
    using CondRes = decltype(false ? DeclVal<Xp (&)()>()() : DeclVal<Yp (&)()>()());

    template<typename Ap, typename Bp, typename = void>
    struct CommonRefImpl
    {
    };

    template<typename Ap, typename Bp>
    using CommonRef = typename CommonRefImpl<Ap, Bp>::Type;

    template<typename Xp, typename Yp>
    using CondResCVRef = CondRes<MatchCVQualifiersType<Xp, Yp> &, MatchCVQualifiersType<Yp, Xp> &>;

    template<typename Xp, typename Yp>
    struct CommonRefImpl<Xp &, Yp &, VoidType<CondResCVRef<Xp, Yp>>> :
        EnableChecker<IsReference<CondResCVRef<Xp, Yp>>, CondResCVRef<Xp, Yp>>
    {
    };

    template<typename Xp, typename Yp>
    using CommonRefC = RemoveReferenceType<CommonRef<Xp &, Yp &>> &&;

    template<typename Xp, typename Yp>
    struct CommonRefImpl<Xp &&, Yp &&, Require<ConvertibleChecker<Xp &&, CommonRefC<Xp, Yp>>, ConvertibleChecker<Yp &&, CommonRefC<Xp, Yp>>>>
    {
        using Type = CommonRefC<Xp, Yp>;
    };

    template<typename Xp, typename Yp>
    using CommonRefD = CommonRef<const Xp &, Yp &>;

    template<typename Xp, typename Yp>
    struct CommonRefImpl<Xp &&, Yp &, Require<ConvertibleChecker<Xp &&, CommonRefD<Xp, Yp>>>>
    {
        using Type = CommonRefD<Xp, Yp>;
    };

    template<typename Xp, typename Yp>
    struct CommonRefImpl<Xp &, Yp &&> : CommonRefImpl<Yp &&, Xp &>
    {
    };

    template<typename Tp, typename Up, template<typename> class TQual, template<typename> class UQual>
    struct BasicCommonReference
    {
    };

    template<typename Tp>
    struct XRef
    {
        template<typename Up>
        using Type = MatchCVQualifiers<Tp, Up>;
    };

    template<typename Tp>
    struct XRef<Tp &>
    {
        template<typename Up>
        using Type = MatchCVQualifiers<Tp, Up> &;
    };

    template<typename Tp>
    struct XRef<Tp &&>
    {
        template<typename Up>
        using Type = MatchCVQualifiers<Tp, Up> &&;
    };

    template<typename Tp1, typename Tp2>
    using BasicCommonReferenceType =
        typename BasicCommonReference<RemoveCVReferenceType<Tp1>, RemoveCVReferenceType<Tp2>, XRef<Tp1>::template Type, XRef<Tp2>::template Type>::Type;

    template<typename T1, typename T2>
        requires IsReference<T1> && IsReference<T2> && requires { typename CommonRef<T1, T2>; } &&
                 IsConvertible<AddPointerType<T1>, AddPointerType<CommonRef<T1, T2>>> &&
                 IsConvertible<AddPointerType<T2>, AddPointerType<CommonRef<T1, T2>>>
    struct CommonReferenceImpl<T1, T2, 1>
    {
        using Type = CommonRef<T1, T2>;
    };

    template<typename T1, typename T2>
        requires requires { typename BasicCommonReferenceType<T1, T2>; }
    struct CommonReferenceImpl<T1, T2, 2>
    {
        using Type = BasicCommonReferenceType<T1, T2>;
    };

    template<typename T1, typename T2>
        requires requires { typename CondRes<T1, T2>; }
    struct CommonReferenceImpl<T1, T2, 3>
    {
        using Type = CondRes<T1, T2>;
    };

    template<typename T1, typename T2>
        requires requires { typename CommonType<T1, T2>; }
    struct CommonReferenceImpl<T1, T2, 4>
    {
        using Type = CommonType<T1, T2>;
    };

    template<typename T1, typename T2>
    struct CommonReferenceImpl<T1, T2, 5>
    {
    };

    template<typename T1, typename T2, typename... Rest>
    struct CommonReference<T1, T2, Rest...> : CommonTypeFold<CommonReference<T1, T2>, CommonTypePack<Rest...>>
    {
    };

    template<typename T1, typename T2, typename... Rest>
    struct CommonTypeFold<CommonReference<T1, T2>, CommonTypePack<Rest...>, VoidType<CommonReferenceType<T1, T2>>> :
        public CommonReference<CommonReferenceType<T1, T2>, Rest...>
    {
    };

} // namespace SSSEngine
