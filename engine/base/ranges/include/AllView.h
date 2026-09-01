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
#include "OwningView.h"
#include "QualifierTraits.h"
#include "RangeAdaptor.h"
#include "RefView.h"
#include "View.h"

namespace SSSEngine::Ranges
{
    // NOLINTBEGIN(bugprone-reserved-identifier, readability-identifier-naming)

    namespace __impl
    {
        struct _All : RangeAdaptorClosure<_All>
        {
            template<typename Range>
            static consteval bool IsNoExcept()
            {
                if constexpr(ViewConcept<DecayType<Range>>)
                    return IsNoThrowConstructible<DecayType<Range>, Range>;
                else if constexpr(CanRefViewConcept<Range>)
                    return true;
                else
                    return noexcept(OwningView{DeclVal<Range>()});
            }

            template<ViewableRangeConcept Range>
                requires ViewConcept<DecayType<Range>> || CanRefViewConcept<Range> || CanOwningViewConcept<Range>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(Range &&range) const noexcept(IsNoExcept<Range>())
            {
                if constexpr(ViewConcept<DecayType<Range>>)
                {
                    return Forward<Range>();
                }
                else if constexpr(CanRefViewConcept<Range>)
                {
                    return RefView{Forward<Range>(range)};
                }
                else
                {
                    return OwningView{Forward<Range>(range)};
                }
            }

            static constexpr bool HasSimpleCallOp = true;
        };
    } // namespace __impl

    // NOLINTEND(bugprone-reserved-identifier, readability-identifier-naming)

    SSSENGINE_GLOBAL
    __impl::_All All{};

    template<ViewableRangeConcept Range>
    using AllType = decltype(All(DeclVal<Range>()));

} // namespace SSSEngine::Ranges
