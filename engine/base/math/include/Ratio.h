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

#include "Math.h"
#include "HelperMacros.h"
#include "Types.h"
#include "ValueConstant.h"

namespace SSSEngine::Math
{

    /**
     * @brief Represents any finite rational number through a numerator and denominator at compile time. The ratio is
     * simplified when instantiated
     */
    template<maxint Num, maxint Den = 1>
        requires(Den != 0 && Num != Math::Limits::Min<maxint> && Den != Math::Limits::Max<maxint>)
    struct Ratio
    {
        static constexpr maxint Gcd = GreatestCommonDivisor(Num, Den);
        static constexpr maxint Numerator = Num * SignOf(Den) / Gcd;
        static constexpr maxint Denominator = Absolute(Den) / Gcd;

        using Type = Ratio<Numerator, Denominator>;
    };

    template<typename T>
    struct IsRatio : FalseType
    {
    };

    template<maxint N, maxint D>
    struct IsRatio<Ratio<N, D>> : TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsRatioValue = IsRatio<T>::Value;

    template<typename T>
    concept RatioConcept = IsRatioValue<T>;

    using Nano = Ratio<1, static_cast<maxint>(1e9)>;
    using Micro = Ratio<1, static_cast<maxint>(1e6)>;
    using Milli = Ratio<1, static_cast<maxint>(1e3)>;
    using Centi = Ratio<1, static_cast<maxint>(1e2)>;
    using Deci = Ratio<1, 10>;
    using Deca = Ratio<10>;
    using Hecto = Ratio<100>;
    using Kilo = Ratio<1000>;
    using Mega = Ratio<static_cast<maxint>(1e6)>;
    using Giga = Ratio<static_cast<maxint>(1e9)>;
    using Tera = Ratio<static_cast<maxint>(1e12)>;

    template<RatioConcept R1, RatioConcept R2>
    struct RatioCompute
    {
        // static constexpr auto Lcm = LeastCommonMultiple(R1::Denominator, R2::Denominator);

        // INVESTIGATE: Is it better with this aliases or create a struct for each operation?
        // Should we care about overflows?

        using Add =
            Ratio<R1::Numerator * R2::Denominator + R2::Numerator * R1::Denominator, R1::Denominator * R2::Denominator>;

        using Subtract =
            Ratio<R1::Numerator * R2::Denominator - R2::Numerator * R1::Denominator, R1::Denominator * R2::Denominator>;

        using Multiply = Ratio<R1::Numerator * R2::Numerator, R1::Denominator * R2::Denominator>;

        using Divide = RatioCompute<R1, Ratio<R2::Denominator, R2::Numerator>>::Multiply;
    };

    template<RatioConcept R1, RatioConcept R2>
    using RatioAdd = RatioCompute<R1, R2>::Add;

    template<RatioConcept R1, RatioConcept R2>
    using RatioSubtract = RatioCompute<R1, R2>::Subtract;

    template<RatioConcept R1, RatioConcept R2>
    using RatioMultiply = RatioCompute<R1, R2>::Multiply;

    template<RatioConcept R1, RatioConcept R2>
    using RatioDivide = RatioCompute<R1, R2>::Divide;

} // namespace SSSEngine::Math

template<SSSEngine::Math::RatioConcept R1, SSSEngine::Math::RatioConcept R2>
struct SSSEngine::CommonTypeChecker<R1, R2>
{
    static constexpr auto Gcd = SSSEngine::Math::GreatestCommonDivisor(R1::Numerator, R2::Numerator);
    static constexpr auto Lcm = SSSEngine::Math::LeastCommonMultiple(R1::Denominator, R2::Denominator);

    using Type = SSSEngine::Math::Ratio<Gcd, Lcm>;
};
