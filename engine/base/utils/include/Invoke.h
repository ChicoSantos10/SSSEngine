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

#include "Concepts.h"
#include "ConversionTraits.h"
#include "HelperMacros.h"
#include "QualifierTraits.h"
#include "Traits.h"
#include "Utility.h"
#include "ValueConstant.h"

namespace SSSEngine
{
    namespace Impl
    {
        // TODO: If we ever implement reference_wrapper, we should also unwrap it here
        template<typename T, typename U = RemoveCVReferenceType<T>>
        struct ReferenceUnwrapper
        {
            using Type = T;
        };

        struct InvokeMemberObjectReference
        {
        };

        struct InvokeMemberObjectDereference
        {
        };

        struct InvokeMemberFunctionReference
        {
        };

        struct InvokeMemberFunctionDereference
        {
        };

        struct InvokeOther
        {
        };

        template<typename T, typename Tag>
        struct ResultOfSuccess : SuccessType<T>
        {
            using InvokeType = Tag;
        };

        struct ResultOfMemberObjectReferenceImpl
        {
            template<typename T, typename U>
            static ResultOfSuccess<decltype((DeclVal<U>()).*DeclVal<T>()), InvokeMemberObjectReference> Test(int);

            template<typename, typename>
            static FailureType Test(...);
        };

        template<typename MemberPtr, typename Arg>
        struct ResultOfMemberObjectReference : private ResultOfMemberObjectReferenceImpl
        {
            using Type = decltype(Test<MemberPtr, Arg>(0));
        };

        struct ResultOfMemberObjectDereferenceImpl
        {
            template<typename T, typename U>
            static ResultOfSuccess<decltype((*DeclVal<U>()).*DeclVal<T>()), InvokeMemberObjectDereference> Test(int);

            template<typename, typename>
            static FailureType Test(...);
        };

        template<typename MemberPtr, typename Arg>
        struct ResultOfMemberObjectDereference : private ResultOfMemberObjectDereferenceImpl
        {
            using Type = decltype(Test<MemberPtr, Arg>(0));
        };

        template<typename MemberPtr, typename Arg>
        struct ResultOfMemberObject;

        template<typename Result, typename Class, typename Arg>
        struct ResultOfMemberObject<Result Class::*, Arg>
        {
            using ArgValueType = RemoveCVReferenceType<Arg>;
            using MemberPtr = Result Class::*;
            using Type = ConditionalType<IsSameType<ArgValueType, Class> || IsBaseOf<Class, ArgValueType>,
                                         ResultOfMemberObjectReference<MemberPtr, Arg>,
                                         ResultOfMemberObjectDereference<MemberPtr, Arg>>::Type;
        };

        template<bool IsMemberObjPtr, bool IsMemberFnPtr, typename Fn, typename... Args>
        struct ResultOfImpl
        {
            using Type = FailureType;
        };

        template<typename MemberPtr, typename Arg>
        struct ResultOfImpl<true, false, MemberPtr, Arg> :
            ResultOfMemberObject<DecayType<MemberPtr>, typename ReferenceUnwrapper<Arg>::Type>
        {
        };

        struct ResultOfMemberFunctionReferenceImpl
        {
            template<typename T, typename U, typename... Args>
            static ResultOfSuccess<decltype((DeclVal<U>().*DeclVal<T>())(DeclVal<Args>()...)), InvokeMemberFunctionReference>
            Test(int);

            template<typename, typename>
            static FailureType Test(...);
        };

        template<typename MemberPtr, typename Arg, typename... Args>
        struct ResultOfMemberFunctionReference : private ResultOfMemberFunctionReferenceImpl
        {
            using Type = decltype(Test<MemberPtr, Arg, Args...>(0));
        };

        struct ResultOfMemberFunctionDereferenceImpl
        {
            template<typename T, typename U, typename... Args>
            static ResultOfSuccess<decltype(((*DeclVal<U>()).*DeclVal<T>())(DeclVal<Args>()...)), InvokeMemberFunctionDereference>
            Test(int);

            template<typename, typename, typename...>
            static FailureType Test(...);
        };

        template<typename MemberPtr, typename Arg, typename... Args>
        struct ResultOfMemberFunctionDereference : private ResultOfMemberFunctionDereferenceImpl
        {
            using Type = decltype(Test<MemberPtr, Arg, Args...>(0));
        };

        template<typename MemberFn, typename Arg, typename... Args>
        struct ResultOfMemberFunction;

        template<typename Result, typename Class, typename Arg, typename... Args>
        struct ResultOfMemberFunction<Result Class::*, Arg, Args...>
        {
            using ArgValuetype = RemoveReferenceType<Arg>;
            using MemberPtr = Result Class::*;
            using Type = ConditionalType<IsBaseOf<Class, ArgValuetype>,
                                         ResultOfMemberFunctionReference<MemberPtr, Arg, Args...>,
                                         ResultOfMemberFunctionDereference<MemberPtr, Arg, Args...>>::Type;
        };

        template<typename MemberPtr, typename Arg, typename... Args>
        struct ResultOfImpl<false, true, MemberPtr, Arg, Args...> :
            public ResultOfMemberFunction<DecayType<MemberPtr>, typename ReferenceUnwrapper<Arg>::Type, Args...>
        {
        };

        struct ResultOfOtherImpl
        {
            template<typename Fn, typename... Args>
            static auto Test(int) -> ResultOfSuccess<decltype(DeclVal<Fn>()(DeclVal<Args>()...)), InvokeOther>;

            template<typename...>
            static FailureType Test(...);
        };

        template<typename Fn, typename... Args>
        struct ResultOfImpl<false, false, Fn, Args...> : private ResultOfOtherImpl
        {
            using Type = decltype(Test<Fn, Args...>(0));
        };

    } // namespace Impl

    template<typename Fn, typename... Args>
    struct InvokeResult :
        public Impl::ResultOfImpl<IsMemberObjectPointer<RemoveReferenceType<Fn>>, IsMemberFunctionPointer<RemoveReferenceType<Fn>>, Fn, Args...>::Type
    {
    };

    template<typename Fn, typename... Args>
    using InvokeResultType = typename InvokeResult<Fn, Args...>::Type;

    namespace Impl
    {
        template<typename Result, typename Ret, bool = IsVoid<Ret>, typename = void>
        struct IsInvocableImpl : FalseType
        {
        };

        template<typename Result, typename Ret>
        struct IsInvocableImpl<Result, Ret, true, VoidType<typename Result::Type>> : TrueType
        {
        };

        template<typename Result, typename Ret>
        struct IsInvocableImpl<Result, Ret, false, VoidType<typename Result::Type>>
        {
          private:
            using ResultType = Result::Type;
            static ResultType Get() noexcept;

            template<typename T>
            static void Convert(IdentityType<T>) noexcept;

            template<typename T, bool NoThrow = noexcept(Convert<T>(Get())), typename = decltype(Convert<T>(Get())), bool Dangle = false>
            static BoolConstant<NoThrow && !Dangle> Test(int);

            template<typename T, bool = false>
            static FalseType Test(...);

          public:
            using Type = decltype(Test<Ret, true>(1));
            using NoThrowConvertible = decltype(Test<Ret>(1));
        };

    } // namespace Impl

    template<typename Fn, typename... Args>
    struct IsInvocableChecker : Impl::IsInvocableImpl<InvokeResult<Fn, Args...>, void>::Type
    {
    };

    template<typename ReturnType, typename Fn, typename... Args>
    struct IsInvocableWithReturnTypeChecker : Impl::IsInvocableImpl<InvokeResult<Fn, Args...>, ReturnType>::Type
    {
    };

    template<typename Fn, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool IsInvocable = IsInvocableChecker<Fn, Args...>::Value;

    template<typename ReturnType, typename Fn, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool IsInvocableWithReturnType = IsInvocableWithReturnTypeChecker<ReturnType, Fn, Args...>::Value;

    namespace Impl
    {
        template<typename Fn, typename... Args>
        consteval bool IsNoThrowCallHelper(InvokeOther)
        {
            return noexcept(DeclVal<Fn>()(DeclVal<Args>()...));
        }

        template<typename Fn, typename T, typename... Args>
        consteval bool IsNoThrowCallHelper(InvokeMemberFunctionReference)
        {
            using Unwrapped = ReferenceUnwrapper<T>::Type;
            return noexcept((DeclVal<Unwrapped>().*DeclVal<Fn>())(DeclVal<Args>()...));
        }

        template<typename Fn, typename T, typename... Args>
        consteval bool IsNoThrowCallHelper(InvokeMemberFunctionDereference)
        {
            return noexcept(((*DeclVal<T>()).*DeclVal<Fn>())(DeclVal<Args>()...));
        }

        template<typename Fn, typename T>
        consteval bool IsNoThrowCallHelper(InvokeMemberObjectReference)
        {
            using Unwrapped = ReferenceUnwrapper<T>::Type;
            return noexcept(DeclVal<Unwrapped>().*DeclVal<Fn>());
        }

        template<typename Fn, typename T>
        consteval bool IsNoThrowCallHelper(InvokeMemberObjectDereference)
        {
            return noexcept((*DeclVal<T>()).*DeclVal<Fn>());
        }

        template<typename Result, typename Fn, typename... Args>
        struct IsNoThrowCallableHelper : BoolConstant<IsNoThrowCallHelper<Fn, Args...>(typename Result::InvokeType{})>
        {
        };

        template<typename Fn, typename... Args>
        struct IsNoThrowCallableChecker : IsNoThrowCallableHelper<InvokeResult<Fn, Args...>, Fn, Args...>
        {
        };

        template<typename Fn, typename... Args>
        struct IsNoThrowInvocableChecker : And<IsInvocableChecker<Fn, Args...>, IsNoThrowCallableChecker<Fn, Args...>>
        {
        };

        template<typename Result, typename ReturnType>
        using IsNoThrowInvocableImpl = typename IsInvocableImpl<Result, ReturnType>::NoThrowConvertible;

        template<typename ReturnType, typename Fn, typename... Args>
        struct IsNoThrowInvocableWithReturnType :
            And<IsNoThrowInvocableImpl<InvokeResult<Fn, Args...>, ReturnType>, IsNoThrowCallableChecker<Fn, Args...>>::Type
        {
        };

        template<typename T, typename U = ReferenceUnwrapper<T>::Type>
        constexpr U &&ForwardWrappedReference(RemoveReferenceType<T> &ref)
        {
            return static_cast<U &&>(ref);
        }

        template<typename Result, typename Fn, typename... Args>
        constexpr Result InvokeImpl(InvokeOther, Fn &&fn, Args... args)
        {
            return Forward<Fn>(fn)(Forward<Args>(args)...);
        }

        template<typename Result, typename Fn, typename T, typename... Args>
        constexpr Result InvokeImpl(InvokeMemberFunctionReference, Fn &&fn, T &&t, Args &&...args)
        {
            return (ForwardWrappedReference<T>(t).*fn)(Forward<Args>(args)...);
        }

        template<typename Result, typename Fn, typename T, typename... Args>
        constexpr Result InvokeImpl(InvokeMemberFunctionDereference, Fn &&fn, T &&t, Args &&...args)
        {
            return ((*Forward<T>(t)).*fn)(Forward<Args>(args)...);
        }

        template<typename Result, typename Fn, typename T>
        constexpr Result InvokeImpl(InvokeMemberObjectReference, Fn &&fn, T &&t)
        {
            return ForwardWrappedReference<T>(t).*fn;
        }

        template<typename Result, typename Fn, typename T>
        constexpr Result InvokeImpl(InvokeMemberObjectDereference, Fn &&fn, T &&t)
        {
            return (*Forward<T>(t)).*fn;
        }
    } // namespace Impl

    template<typename Callable, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowInvocable = Impl::IsNoThrowInvocableChecker<Callable, Args...>::Value;

    template<typename ReturnType, typename Callable, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowInvocableWithReturnType =
        Impl::IsNoThrowInvocableWithReturnType<ReturnType, Callable, Args...>::Value;

    template<typename Callable, typename... Args>
    constexpr InvokeResultType<Callable, Args...>
    Invoke(Callable &&fn, Args &&...args) noexcept(IsNoThrowInvocable<Callable, Args...>)
    {
        using Result = InvokeResult<Callable, Args...>;
        using ResultType = Result::Type;
        using ResultTag = Result::InvokeType;
        return Impl::InvokeImpl<ResultType>(ResultTag{}, Forward<Callable>(fn), Forward<Args>(args)...);
    }

    template<typename ReturnType, typename Callable, typename... Args>
        requires IsInvocableWithReturnType<ReturnType, Callable, Args...>
    constexpr ReturnType
    Invoke(Callable &&fn, Args &&...args) noexcept(IsNoThrowInvocableWithReturnType<ReturnType, Callable, Args...>)
    {
        using Result = InvokeResult<Callable, Args...>;
        using ResultType = Result::Type;
        using ResultTag = Result::InvokeType;
        if constexpr(IsSameType<ReturnType, void>)
        {
            Impl::InvokeImpl<ResultType>(ResultTag{}, Forward<Callable>(fn), Forward<Args>(args)...);
        }
        else
        {
            return Impl::InvokeImpl<ResultType>(ResultTag{}, Forward<Callable>(fn), Forward<Args>(args)...);
        }
    }

    template<typename Invocable, typename... Args>
    concept InvocableConcept = IsInvocable<Invocable, Args...>;

    template<typename Invocable, typename... Args>
    concept PredicateConcept = InvocableConcept<Invocable, Args...> && requires(Invocable &&inv, Args &&...args) {
        { Invoke(Forward<Invocable>(inv), Forward<Args>(args)...) } -> BooleanTestableConcept;
    };

} // namespace SSSEngine
