/**
 * @file
 * @brief
 */

#pragma once

#include "Bits.h"
#include "Concepts.h"
#include "SignTraits.h"

namespace SSSEngine
{
    template<typename T>
    struct IntTraits;

    template<IntegralConcept Int>
    struct IntTraits<Int>
    {
        using Type = Int;

        /**
         * @brief The maximum number that can be represented by the Integer type N
         */
        static constexpr Int Max = IsSigned<Int> ? ~(Int{1} << (Bits<Int> - 1)) : ~Int{0};

        /**
         * @brief The minimum number that can be represented by the Integer type N
         */
        static constexpr Int Min = IsSigned<Int> ? static_cast<Int>(Int{1} << (Bits<Int> - 1)) : Int{0};

        /**
         * @brief The amount of meaningful base 2 digits for type N
         */
        static constexpr u32 BinaryDigits = IsSigned<Int> ? Bits<Int> - 1 : Bits<Int>;

        /**
         * @brief The amount of meaningful base 10 digits for type N
         */
        static constexpr u32 DecimalDigits = BinaryDigits * 0.301029995664;
    };

} // namespace SSSEngine
