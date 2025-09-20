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
#include "HelperMacros.h"
#include "Limits.h"
#include "Ratio.h"

namespace SSSEngine::Math
{
    /**
     * @brief Represents a time duration
     */
    template<typename T, NumberConcept V, Math::RatioConcept P>
        requires(P::Numerator > 0)
    struct Quantity;

    template<typename T>
    struct IsQuantity : std::false_type
    {
    };

    template<typename T, NumberConcept V, Math::RatioConcept P>
    struct IsQuantity<Quantity<T, V, P>> : std::true_type
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsQuantityValue = IsQuantity<T>::value;

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
    concept SameQuantityConcept = SameType<TagType<T>, TagType<V>>;

    template<QuantityConcept Q1, SameQuantityConcept<Q1> Q2>
    struct QuantityCommonType
    {
        using CommonValueType = CommonType<typename Q1::ValueType, typename Q2::ValueType>;
        using CommonRatioType = CommonType<typename Q1::Ratio, typename Q2::Ratio>;

        using type = Quantity<typename Q1::Tag, CommonValueType, CommonRatioType>;
    };

    template<typename T, NumberConcept V, Math::RatioConcept P>
        requires(P::Numerator > 0)
    struct Quantity

    {
        using ValueType = V;
        using Ratio = P;
        using QuantityType = Quantity<T, V, P>;
        using Tag = T;

        ValueType value;

        template<SameQuantityConcept<QuantityType> To>
        explicit constexpr operator To() const
        {
            if constexpr(SameType<QuantityType, To>)
            {
                return To(value);
            }

            using Divide = Math::RatioDivide<typename QuantityType::Ratio, typename To::Ratio>;
            // TODO: Create common type trait
            using CommonType = std::common_type_t<typename To::ValueType, ValueType, maxint>;

            auto convertedValue = static_cast<CommonType>(value) * static_cast<CommonType>(Divide::Numerator) /
                                  static_cast<CommonType>(Divide::Denominator);

            return To(static_cast<To::ValueType>(convertedValue));
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
            return Quantity{Limits::Max<ValueType>};
        }

        static consteval Quantity Min() noexcept
        {
            return {Limits::Min<ValueType>};
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

namespace std // NOLINT(readability-identifier-naming)
{
    template<SSSEngine::Math::QuantityConcept Q1, SSSEngine::Math::SameQuantityConcept<Q1> Q2>
    struct common_type<Q1, Q2> : SSSEngine::Math::QuantityCommonType<Q1, Q2> // NOLINT(cert-dcl58-cpp)
    {
    };

    template<SSSEngine::Math::QuantityConcept Q, SSSEngine::NumberConcept Num>
    struct common_type<Q, Num> // NOLINT(cert-dcl58-cpp)
    {
        using CT = common_type_t<typename Q::ValueType, Num>;
        using type = SSSEngine::Math::Quantity<typename Q::Tag, CT, typename Q::Ratio>;
    };
} // namespace std
