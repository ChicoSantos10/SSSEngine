/**
 * @file
 * @brief
 */

#pragma once

#include "Address.h"
#include "Attributes.h"
#include "Concepts.h"
#include "Debug.h"
#include "Iterator.h"
#include "ObjectConcepts.h"
#include "QualifierTraits.h"
#include "Range.h"

namespace SSSEngine::Ranges
{
    /**
     * @class ViewTag
     * @brief A tag to represent a type is a view
     *
     * If a class inherits from this, it will be considered a view.
     *
     */
    struct ViewTag
    {
    };

    template<typename Derived>
        requires IsClass<Derived> && SameAsConcept<Derived, RemoveCVType<Derived>>
    class ViewInterface;

    namespace __impl // NOLINT(bugprone-reserved-identifier, readability-identifier-naming)
    {
        template<typename T, typename U>
            requires(!SameAsConcept<T, ViewInterface<U>>)
        void IsDerivedFromViewInterfaceHelper(const T &, const ViewInterface<U> &);

    } // namespace __impl

    template<typename T>
    concept ViewInterfaceConcept = requires(T t) { __impl::IsDerivedFromViewInterfaceHelper(t, t); };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool EnableView = DerivedFromConcept<T, ViewTag> || ViewInterfaceConcept<T>;

    template<typename T>
    concept ViewConcept = RangeConcept<T> && MovableConcept<T> && EnableView<T>;

    /**
     * @class ViewInterface
     * @brief Serves the purpose of implementing common functions for each View using CRTP
     *
     */
    template<typename Derived>
        requires IsClass<Derived> && SameAsConcept<Derived, RemoveCVType<Derived>>
    class ViewInterface
    {
        template<typename Self>
        constexpr decltype(auto) GetDerived(this Self &&self) noexcept
        {
            SSSENGINE_STATIC_ASSERT(DerivedFromConcept<Derived, ViewInterface<Derived>>);
            SSSENGINE_STATIC_ASSERT(ViewConcept<Derived>);

            using Type = MatchCV<Self, Derived>::Type;
            return static_cast<Type &>(Forward<Self>(self));
        }

        /**
         * @brief Purely to evaluate an expression for correct propagation of noexcept
         *
         * @return Never evaluated
         */
        static constexpr bool EvaluateExpression(bool) noexcept;

        template<typename T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        static constexpr bool IsEmpty(T &t) noexcept(noexcept(EvaluateExpression(Ranges::Begin(t) == Ranges::End(t))))
        {
            return Ranges::Begin(t) == Ranges::End(t);
        }

        template<typename T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        static constexpr auto Count(T &t) noexcept(noexcept(Ranges::Begin(t) - Ranges::End(t)))
        {
            return Ranges::End(t) - Ranges::Begin(t);
        }

      public:
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() noexcept(noexcept(IsEmpty(GetDerived())))
            requires MultiPassRangeConcept<Derived> && (!SizedRangeConcept<Derived>)
        {
            return IsEmpty(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() noexcept(noexcept(Ranges::Count(GetDerived()) == 0))
            requires SizedRangeConcept<Derived>
        {
            return Ranges::Count(GetDerived()) == 0;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() const noexcept(noexcept(IsEmpty(GetDerived())))
            requires MultiPassRangeConcept<const Derived> && (!SizedRangeConcept<const Derived>)
        {
            return IsEmpty(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() const noexcept(noexcept(Ranges::Count(GetDerived()) == 0))
            requires SizedRangeConcept<const Derived>
        {
            return Ranges::Count(GetDerived()) == 0;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr explicit operator bool() noexcept(noexcept(Ranges::IsEmpty(GetDerived())))
            requires requires { Ranges::IsEmpty(GetDerived()); }
        {
            return !Ranges::IsEmpty(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr explicit operator bool() const noexcept(noexcept(Ranges::IsEmpty(GetDerived())))
            requires requires { Ranges::IsEmpty(GetDerived()); }
        {
            return !Ranges::IsEmpty(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Data() noexcept(noexcept(Ranges::Begin(GetDerived())))
            requires ContiguousMemoryIteratorConcept<IteratorType<Derived>>
        {
            return ToAddress(Ranges::Begin(GetDerived()));
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Data() const noexcept(noexcept(Ranges::Begin(GetDerived())))
            requires RangeConcept<const Derived> && ContiguousMemoryIteratorConcept<IteratorType<const Derived>>
        {
            return ToAddress(Ranges::Begin(GetDerived()));
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Count() noexcept(noexcept(Count(GetDerived())))
            requires MultiPassRangeConcept<Derived> && SizedSentinelForConcept<SentinelType<Derived>, IteratorType<Derived>>
        {
            return Count(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Count() const noexcept(noexcept(Count(GetDerived())))
            requires MultiPassRangeConcept<const Derived> &&
                     SizedSentinelForConcept<SentinelType<const Derived>, IteratorType<const Derived>>
        {
            return Count(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Front() noexcept(noexcept(*Ranges::Begin(GetDerived())))
            requires MultiPassRangeConcept<Derived>
        {
            SSSENGINE_ASSERT(!IsEmpty());
            return *Ranges::Begin(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Front() const noexcept(noexcept(*Ranges::Begin(GetDerived())))
            requires MultiPassRangeConcept<const Derived>
        {
            SSSENGINE_ASSERT(!IsEmpty());
            return *Ranges::Begin(GetDerived());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Back() noexcept(noexcept(*Ranges::End(GetDerived())))
            requires MultiPassRangeConcept<Derived>
        {
            SSSENGINE_ASSERT(!IsEmpty());
            return *Previous(Ranges::End(GetDerived()));
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Back() const noexcept(noexcept(*Ranges::End(GetDerived())))
            requires MultiPassRangeConcept<const Derived>
        {
            SSSENGINE_ASSERT(!IsEmpty());
            return *Previous(Ranges::End(GetDerived()));
        }

        template<RandomAccessRangeConcept Range = Derived>
        constexpr decltype(auto) operator[](RangeDifferenceType<Range> offset)
        {
            return Ranges::Begin(GetDerived())[offset];
        }

        template<RandomAccessRangeConcept Range = const Derived>
        constexpr decltype(auto) operator[](RangeDifferenceType<Range> offset) const
        {
            return Ranges::Begin(GetDerived())[offset];
        }

        constexpr auto ConstBegin()
            requires InputRangeConcept<Derived>
        {
            return Ranges::ConstBegin(GetDerived());
        }

        constexpr auto ConstBegin() const
            requires InputRangeConcept<const Derived>
        {
            return Ranges::ConstBegin(GetDerived());
        }

        constexpr auto ConstEnd()
            requires InputRangeConcept<Derived>
        {
            return Ranges::ConstEnd(GetDerived());
        }

        constexpr auto ConstEnd() const
            requires InputRangeConcept<const Derived>
        {
            return Ranges::ConstEnd(GetDerived());
        }
    };
} // namespace SSSEngine::Ranges
