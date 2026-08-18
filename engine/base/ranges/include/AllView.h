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
