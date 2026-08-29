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
 * @brief Quantity struct
 */

#pragma once

#include "Concepts.h"
#include "ConversionTraits.h"
#include "HelperMacros.h"
#include "Limits.h"
#include "Ratio.h"
#include "ValueConstant.h"
#include "Ordering.h"

namespace SSSEngine::Math
{
    /**
     * @brief Represents a time duration
     */
    template<typename T, NumberConcept V, Math::RatioConcept P>
        requires(P::Numerator > 0)
    struct Quantity;

    template<typename T>
    struct IsQuantity : FalseType
    {
    };

    template<typename T, NumberConcept V, Math::RatioConcept P>
    struct IsQuantity<Quantity<T, V, P>> : TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsQuantityValue = IsQuantity<T>::Value;

    template<typename T>
    concept QuantityConcept = IsQuantityValue<T>;

    template<typename T>
    struct QuantityTag
    {
    };

    template<typename T, NumberConcept N, RatioConcept R>
    struct QuantityTag<Quantity<T, N, R>>
    {
        using TagType = T;
    };

    template<typename T>
    using TagType = typename QuantityTag<T>::TagType;

    template<typename T, typename V>
    concept SameQuantityConcept = IsSameType<TagType<T>, TagType<V>>;

    template<typename T, NumberConcept V, Math::RatioConcept P>
        requires(P::Numerator > 0)
    struct Quantity
    {
        using ValueType = V;
        using Ratio = P;
        using QuantityType = Quantity<T, V, P>;
        using Tag = T;

        ValueType value{0};

        constexpr Quantity() noexcept = default;

        explicit constexpr Quantity(ValueType v) : value(v) {}

        template<SameQuantityConcept<QuantityType> To>
        explicit constexpr Quantity(To from) : value(from.template As<QuantityType>())
        {
        }

        template<SameQuantityConcept<QuantityType> To>
        explicit constexpr operator To() const
        {
            return To(As<To>());
        }

        template<SameQuantityConcept<QuantityType> To>
        constexpr To::ValueType As() const noexcept
        {
            if constexpr(IsSameType<QuantityType, To>)
            {
                return value;
            }

            using Divide = Math::RatioDivide<typename QuantityType::Ratio, typename To::Ratio>;
            using CT = CommonType<typename To::ValueType, ValueType, maxint>;

            auto convertedValue =
                static_cast<CT>(value) * static_cast<CT>(Divide::Numerator) / static_cast<CT>(Divide::Denominator);

            return static_cast<To::ValueType>(convertedValue);
        }

        constexpr operator ValueType() // NOLINT(*-explicit-constructor)
        {
            return value;
        }

        constexpr Quantity operator+() const
        {
            return Quantity{value};
        }

        constexpr Quantity operator-() const
        {
            return Quantity{-value};
        }

        constexpr Quantity &operator++()
        {
            ++value;
            return *this;
        }

        constexpr Quantity operator++(int)
        {
            return Quantity(value++);
        }

        constexpr Quantity &operator--()
        {
            --value;
            return *this;
        }

        constexpr Quantity operator--(int)
        {
            return Quantity(value--);
        }

        constexpr Quantity &operator+=(const SameQuantityConcept<Quantity> auto &quant)
        {
            value += static_cast<Quantity>(quant).value;
            return *this;
        }

        constexpr Quantity &operator-=(const SameQuantityConcept<Quantity> auto &quant)
        {
            value -= static_cast<Quantity>(quant).value;
            return *this;
        }

        constexpr Quantity &operator*=(ValueType quant)
        {
            value *= quant;
            return *this;
        }

        constexpr Quantity &operator/=(ValueType quant)
        {
            value /= quant;
            return *this;
        }

        constexpr Quantity &operator%=(IntegralConcept auto div)
        {
            value %= div;
            return *this;
        }

        static consteval Quantity Max() noexcept
        {
            return Quantity{NumericTraitsType<ValueType>::Max};
        }

        static consteval Quantity Min() noexcept
        {
            return Quantity{NumericTraitsType<ValueType>::Min};
        }

        template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
        constexpr friend auto operator+(Lhs lhs, Rhs rhs);

        template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
        constexpr friend auto operator-(Lhs lhs, Rhs rhs);

        template<QuantityConcept Quant, NumberConcept Num>
        constexpr friend auto operator*(Quant lhs, Num rhs);

        template<NumberConcept Num, QuantityConcept Quant>
        constexpr friend auto operator*(Num lhs, Quant rhs);

        template<QuantityConcept Quant, NumberConcept Num>
        constexpr friend auto operator/(Quant lhs, Num rhs);

        template<QuantityConcept Quant, IntegralConcept Num>
        constexpr friend auto operator%(Quant lhs, Num rhs);

        template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
        constexpr friend auto operator<=>(Lhs lhs, Rhs rhs);

        template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
        constexpr friend auto operator==(Lhs lhs, Rhs rhs);
    };

    template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
    constexpr auto operator+(Lhs lhs, Rhs rhs)
    {
        using ReturnType = CommonType<Lhs, Rhs>;

        auto newValue = static_cast<ReturnType>(lhs).value + static_cast<ReturnType>(rhs).value;
        return ReturnType{newValue};
    }

    template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
    constexpr auto operator-(Lhs lhs, Rhs rhs)
    {
        using ReturnType = CommonType<Lhs, Rhs>;

        auto newValue = static_cast<ReturnType>(lhs).value - static_cast<ReturnType>(rhs).value;
        return ReturnType{newValue};
    }

    template<QuantityConcept Quant, NumberConcept Num>
    constexpr auto operator*(Quant lhs, Num rhs)
    {
        using ReturnType = CommonType<Quant, Num>;

        auto newValue = static_cast<ReturnType>(lhs).value * rhs;
        return ReturnType{newValue};
    }

    template<NumberConcept Num, QuantityConcept Quant>
    constexpr auto operator*(Num lhs, Quant rhs)
    {
        return rhs * lhs;
    }

    template<QuantityConcept Quant, NumberConcept Num>
    constexpr auto operator/(Quant lhs, Num rhs)
    {
        using ReturnType = CommonType<Quant, Num>;

        auto value = static_cast<ReturnType>(lhs).value / rhs;
        return ReturnType{value};
    }

    template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
    constexpr auto operator/(Lhs lhs, Rhs rhs)
    {
        using ReturnType = CommonType<Lhs, Rhs>;
        return static_cast<ReturnType>(rhs).value / static_cast<ReturnType>(lhs).value;
    }

    template<QuantityConcept Quant, IntegralConcept Num>
    constexpr auto operator%(Quant lhs, Num rhs)
    {
        using ReturnType = CommonType<Quant, Num>;

        auto value = static_cast<ReturnType>(lhs).value % rhs;
        return ReturnType{value};
    }

    template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
    constexpr auto operator<=>(Lhs lhs, Rhs rhs)
    {
        using Common = CommonType<Lhs, Rhs>;

        return static_cast<Common>(lhs).value <=> static_cast<Common>(rhs).value;
    }

    template<QuantityConcept Lhs, SameQuantityConcept<Lhs> Rhs>
    constexpr auto operator==(Lhs lhs, Rhs rhs)
    {
        using Common = CommonType<Lhs, Rhs>;

        return static_cast<Common>(lhs).value == static_cast<Common>(rhs).value;
    }
} // namespace SSSEngine::Math

template<SSSEngine::Math::QuantityConcept Q1, SSSEngine::Math::SameQuantityConcept<Q1> Q2>
struct SSSEngine::CommonTypeChecker<Q1, Q2>
{
    using CommonValueType = CommonType<typename Q1::ValueType, typename Q2::ValueType>;
    using CommonRatioType = CommonType<typename Q1::Ratio, typename Q2::Ratio>;

    using Type = Math::Quantity<typename Q1::Tag, CommonValueType, CommonRatioType>;
};

template<SSSEngine::Math::QuantityConcept Q, SSSEngine::NumberConcept Num>
struct SSSEngine::CommonTypeChecker<Q, Num>
{
    using CT = CommonType<typename Q::ValueType, Num>;
    using Type = Math::Quantity<typename Q::Tag, CT, typename Q::Ratio>;
};
