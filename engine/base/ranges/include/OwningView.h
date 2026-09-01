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

#include "Attributes.h"
#include "Concepts.h"
#include "CopyAndMoveTraits.h"
#include "InitializerList.h"
#include "QualifierTraits.h"
#include "Range.h"
#include "View.h"

namespace SSSEngine::Ranges
{
    template<typename Range>
        requires MovableConcept<Range> && (!IsInitializerList<RemoveCVReferenceType<Range>>)
    class OwningView : public ViewInterface<OwningView<Range>>
    {
      public:
        using Iterator = IteratorType<Range>;
        using Sentinel = SentinelType<Range>;

        OwningView()
            requires DefaultInitializableConcept<Range>
        = default;

        OwningView(const OwningView &) = delete;
        OwningView(OwningView &&) = default;

        constexpr OwningView(Range &&range) noexcept(IsNoThrowMoveConstructible<Range>) : // NOLINT(google-explicit-constructor)
            m_range(Move(range))
        {
        }

        constexpr OwningView &operator=(OwningView &&) = default;
        OwningView &operator=(const OwningView &) = delete;

        ~OwningView() = default;

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr decltype(auto) Underlying(this Self &&self) noexcept
        {
            return Forward<Self>(self).m_range;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Begin() noexcept(noexcept(Ranges::Begin(m_range)))
        {
            return Ranges::Begin(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Begin() const noexcept(noexcept(Ranges::Begin(m_range)))
            requires RangeConcept<const Range>
        {
            return Ranges::Begin(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Sentinel End() noexcept(noexcept(Ranges::End(m_range)))
        {
            return Ranges::End(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto End() const noexcept(noexcept(Ranges::End(m_range)))
            requires RangeConcept<const Range>
        {
            return Ranges::End(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() noexcept(noexcept(Ranges::IsEmpty(m_range)))
            requires requires { Ranges::IsEmpty(Underlying()); }
        {
            return Ranges::IsEmpty(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() const noexcept(noexcept(Ranges::IsEmpty(m_range)))
            requires requires { Ranges::IsEmpty(Underlying()); }
        {
            return Ranges::IsEmpty(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Sentinel Count() noexcept(noexcept(Ranges::Count(m_range)))
            requires SizedRangeConcept<Range>
        {
            return Ranges::Count(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Count() const noexcept(noexcept(Ranges::Count(m_range)))
            requires SizedRangeConcept<const Range>
        {
            return Ranges::Count(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Sentinel Data() noexcept(noexcept(Ranges::Data(m_range)))
            requires ContiguousRangeConcept<Range>
        {
            return Ranges::Data(m_range);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Data() const noexcept(noexcept(Ranges::Data(m_range)))
            requires ContiguousRangeConcept<const Range>
        {
            return Ranges::Data(m_range);
        }

      private:
        Range m_range{};

        // NOLINTBEGIN(readability-identifier-naming)

        friend constexpr auto begin(const OwningView &view)
        {
            return view.Begin();
        }

        friend constexpr auto end(const OwningView &view)
        {
            return view.End();
        }

        // NOLINTEND(readability-identifier-naming)
    };

    template<typename Range>
    inline constexpr bool EnableBorrowRange<OwningView<Range>> = EnableBorrowRange<Range>;

    template<typename Range>
    concept CanOwningViewConcept = requires { OwningView{DeclVal<Range>()}; };

} // namespace SSSEngine::Ranges
