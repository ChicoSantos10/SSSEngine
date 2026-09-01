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
#include "Attributes.h"
#include "Concepts.h"
#include "HelperMacros.h"
#include "ObjectTraits.h"
#include "Range.h"
#include "Traits.h"
#include "View.h"

namespace SSSEngine::Ranges
{
    template<RangeConcept Range>
        requires IsObject<Range>
    struct RefView : public ViewInterface<RefView<Range>>
    {
      private:
        static void Fun(Range &);
        static void Fun(Range &&) = delete; // NOLINT(modernize-use-equals-delete)

      public:
        using Iterator = IteratorType<Range>;
        using Sentinel = SentinelType<Range>;

        template<DifferentFromConcept<RefView> T>
            requires ConvertibleToConcept<T, Range &> && requires { Fun(DeclVal<T>()); }
        constexpr RefView(T &&t) noexcept(noexcept(static_cast<Range &>(DeclVal<T>()))) : // NOLINT(google-explicit-constructor)
            m_range(AddressOf(static_cast<Range &>(Forward<T>(t))))
        {
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Range &Underlying() const noexcept
        {
            return *m_range;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Begin() const noexcept(noexcept(Ranges::Begin(Underlying())))
        {
            return Ranges::Begin(Underlying());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator End() const noexcept(noexcept(Ranges::End(Underlying())))
        {
            return Ranges::End(Underlying());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator IsEmpty() const noexcept(noexcept(Ranges::IsEmpty(Underlying())))
            requires requires { Ranges::IsEmpty(Underlying()); }
        {
            return Ranges::IsEmpty(Underlying());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Count() const noexcept(noexcept(Ranges::Count(Underlying())))
            requires SizedRangeConcept<Range>
        {
            return Ranges::Count(Underlying());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Data() const noexcept(noexcept(Ranges::Data(Underlying())))
            requires ContiguousRangeConcept<Range>
        {
            return Ranges::Data(Underlying());
        }

      private:
        Range *m_range;

        // NOLINTBEGIN(readability-identifier-naming)

        friend constexpr auto begin(const RefView &view)
        {
            return view.Begin();
        }

        friend constexpr auto end(const RefView &view)
        {
            return view.End();
        }

        // NOLINTEND(readability-identifier-naming)
    };

    template<typename Range>
    RefView(Range &) -> RefView<Range>;

    template<typename Range>
    SSSENGINE_GLOBAL
    constexpr bool EnableBorrowRange<RefView<Range>> = true;

    template<typename Range>
    concept CanRefViewConcept = requires { RefView{DeclVal<Range>()}; };

} // namespace SSSEngine::Ranges
