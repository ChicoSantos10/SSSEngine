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

// NOLINTBEGIN
#include "Attributes.h"
#include "Limits.h"
#include "Types.h"

namespace std
{
    namespace SSSENGINE_HIDDEN Impl
    {
        using OrderingType = i8;

        enum class Ordering : OrderingType
        {
            equivalent = 0,
            less = -1,
            greater = 1,
            unordered = SSSEngine::Math::Limits::Min<OrderingType>,
        };

        template<typename T>
        SSSENGINE_FORCE_INLINE
        constexpr Ordering Order(T type) noexcept
        {
            return Ordering(type.value);
        }

        template<typename T>
        SSSENGINE_FORCE_INLINE
        constexpr T Make(Ordering ordering) noexcept
        {
            return T(ordering);
        }

        struct LiteralZero
        {
            consteval LiteralZero(LiteralZero *) noexcept {}
        };
    } // namespace SSSENGINE_HIDDEN Impl

    class partial_ordering
    {
      public:
        static const partial_ordering less;
        static const partial_ordering equivalent;
        static const partial_ordering greater;
        static const partial_ordering unordered;

        SSSENGINE_PURE
        friend constexpr bool operator==(partial_ordering ordering, Impl::LiteralZero)
        {
            return ordering.value == 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator==(partial_ordering, partial_ordering) = default;

        SSSENGINE_PURE
        friend constexpr bool operator<(partial_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value == -1;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>(partial_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value == 1;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<=(partial_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.Reverse() >= 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>=(partial_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value >= 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<(Impl::LiteralZero, partial_ordering ordering) noexcept
        {
            return ordering.value == 1;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>(Impl::LiteralZero, partial_ordering ordering) noexcept
        {
            return ordering.value == -1;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<=(Impl::LiteralZero, partial_ordering ordering) noexcept
        {
            return 0 <= ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>=(Impl::LiteralZero, partial_ordering ordering) noexcept
        {
            return 0 <= ordering.Reverse();
        }

        SSSENGINE_PURE
        friend constexpr partial_ordering operator<=>(partial_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering;
        }

        SSSENGINE_PURE
        friend constexpr partial_ordering operator<=>(Impl::LiteralZero, partial_ordering ordering) noexcept
        {
            return partial_ordering(Impl::Ordering(ordering.Reverse()));
        }

      private:
        Impl::OrderingType value;

        SSSENGINE_FORCE_INLINE
        constexpr Impl::OrderingType Reverse() const
        {
            return static_cast<Impl::OrderingType>(-value);
        }

        constexpr explicit partial_ordering(Impl::Ordering ordering) noexcept : value(Impl::OrderingType(ordering)) {}

        friend constexpr Impl::Ordering Impl::Order<partial_ordering>(partial_ordering) noexcept;
        friend constexpr partial_ordering Impl::Make<partial_ordering>(Impl::Ordering) noexcept;
    };

    inline constexpr partial_ordering partial_ordering::less(Impl::Ordering::less);
    inline constexpr partial_ordering partial_ordering::equivalent(Impl::Ordering::equivalent);
    inline constexpr partial_ordering partial_ordering::greater(Impl::Ordering::greater);
    inline constexpr partial_ordering partial_ordering::unordered(Impl::Ordering::unordered);

    class weak_ordering
    {
      public:
        static const weak_ordering less;
        static const weak_ordering equivalent;
        static const weak_ordering greater;

        SSSENGINE_PURE
        constexpr operator partial_ordering() const noexcept
        {
            return Impl::Make<partial_ordering>(Impl::Ordering(value));
        }

        SSSENGINE_PURE
        friend constexpr bool operator==(weak_ordering ordering, Impl::LiteralZero)
        {
            return ordering.value == 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator==(weak_ordering, weak_ordering) = default;

        SSSENGINE_PURE
        friend constexpr bool operator<(weak_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value < 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>(weak_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value > 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<=(weak_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value <= 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>=(weak_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value >= 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<(Impl::LiteralZero, weak_ordering ordering) noexcept
        {
            return 0 < ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>(Impl::LiteralZero, weak_ordering ordering) noexcept
        {
            return 0 > ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<=(Impl::LiteralZero, weak_ordering ordering) noexcept
        {
            return 0 <= ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>=(Impl::LiteralZero, weak_ordering ordering) noexcept
        {
            return 0 >= ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr weak_ordering operator<=>(weak_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering;
        }

        SSSENGINE_PURE
        friend constexpr weak_ordering operator<=>(Impl::LiteralZero, weak_ordering ordering) noexcept
        {
            return weak_ordering(Impl::Ordering(-ordering.value));
        }

      private:
        Impl::OrderingType value;

        constexpr explicit weak_ordering(Impl::Ordering ordering) noexcept : value(Impl::OrderingType(ordering)) {}

        friend constexpr Impl::Ordering Impl::Order<weak_ordering>(weak_ordering) noexcept;
        friend constexpr weak_ordering Impl::Make<weak_ordering>(Impl::Ordering) noexcept;
    };

    inline constexpr weak_ordering weak_ordering::less(Impl::Ordering::less);
    inline constexpr weak_ordering weak_ordering::equivalent(Impl::Ordering::equivalent);
    inline constexpr weak_ordering weak_ordering::greater(Impl::Ordering::greater);

    class strong_ordering
    {
      public:
        static const strong_ordering less;
        static const strong_ordering equivalent;
        static const strong_ordering equal;
        static const strong_ordering greater;

        SSSENGINE_PURE
        constexpr operator partial_ordering() const noexcept
        {
            return Impl::Make<partial_ordering>(Impl::Ordering(value));
        }

        SSSENGINE_PURE
        constexpr operator weak_ordering() const noexcept
        {
            return Impl::Make<weak_ordering>(Impl::Ordering(value));
        }

        SSSENGINE_PURE
        friend constexpr bool operator==(strong_ordering ordering, Impl::LiteralZero)
        {
            return ordering.value == 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator==(strong_ordering, strong_ordering) = default;

        SSSENGINE_PURE
        friend constexpr bool operator<(strong_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value < 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>(strong_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value > 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<=(strong_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value <= 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>=(strong_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering.value >= 0;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<(Impl::LiteralZero, strong_ordering ordering) noexcept
        {
            return 0 < ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>(Impl::LiteralZero, strong_ordering ordering) noexcept
        {
            return 0 > ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr bool operator<=(Impl::LiteralZero, strong_ordering ordering) noexcept
        {
            return 0 <= ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr bool operator>=(Impl::LiteralZero, strong_ordering ordering) noexcept
        {
            return 0 >= ordering.value;
        }

        SSSENGINE_PURE
        friend constexpr strong_ordering operator<=>(strong_ordering ordering, Impl::LiteralZero) noexcept
        {
            return ordering;
        }

        SSSENGINE_PURE
        friend constexpr strong_ordering operator<=>(Impl::LiteralZero, strong_ordering ordering) noexcept
        {
            return strong_ordering(Impl::Ordering(-ordering.value));
        }

      private:
        Impl::OrderingType value;

        constexpr explicit strong_ordering(Impl::Ordering ordering) noexcept : value(Impl::OrderingType(ordering)) {}

        friend constexpr Impl::Ordering Impl::Order<strong_ordering>(strong_ordering) noexcept;
        friend constexpr strong_ordering Impl::Make<strong_ordering>(Impl::Ordering) noexcept;
    };

    inline constexpr strong_ordering strong_ordering::less(Impl::Ordering::less);
    inline constexpr strong_ordering strong_ordering::equal(Impl::Ordering::equivalent);
    inline constexpr strong_ordering strong_ordering::equivalent(Impl::Ordering::equivalent);
    inline constexpr strong_ordering strong_ordering::greater(Impl::Ordering::greater);

} // namespace std

// NOLINTEND

namespace SSSEngine
{
    using StrongOrdering = std::strong_ordering;
    using WeakOrdering = std::weak_ordering;
    using PartialOrdering = std::partial_ordering;

    struct Test
    {
        int a{10};
    };

    auto operator<=>(const Test &a, const int &b)
    {
        return a.a <=> b;
    }

} // namespace SSSEngine
