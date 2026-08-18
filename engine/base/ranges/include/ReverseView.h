/**
 * @file
 * @brief
 */

#pragma once

#include "Array.h"
#include "Attributes.h"
#include "Concepts.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Empty.h"
#include "HelperMacros.h"
#include "Iterator.h"
#include "QualifierTraits.h"
#include "Range.h"
#include "RangeAdaptor.h"
#include "ReverseIterator.h"
#include "Subrange.h"
#include "View.h"
#include "AllView.h"

namespace SSSEngine::Ranges
{
    template<ViewConcept View>
        requires BidirectionalRangeConcept<View>
    class ReverseView : public ViewInterface<ReverseView<View>>
    {
        consteval bool IsNoExceptBegin()
        {
            auto v = DeclVal<View>();
            auto it = DeclVal<IteratorType<View>>();
            bool next = noexcept(Ranges::Next(Begin(v), End(v))) && noexcept(MakeReverseIterator(Move(it)));

            if constexpr(NeedsCachedBegin)
            {
                return noexcept(MakeReverseIterator(it)) && next;
            }
            else
            {
                return next;
            }
        };

      public:
        using ReverseIterator = ReverseIterator<IteratorType<View>>;

        ReverseView()
            requires IsDefaultConstructible<View>
        = default;

        constexpr explicit ReverseView(View view) : m_view(Move(view)) {}

        constexpr View GetView() const &
            requires CopyConstructibleConcept<View>
        {
            return m_view;
        }

        constexpr View GetView() &&
        {
            return Move(m_view);
        }

        constexpr ReverseIterator Begin() noexcept(IsNoExceptBegin())
        {
            if constexpr(NeedsCachedBegin)
            {
                if(m_cached.HasValue())
                {
                    return MakeReverseIterator(m_cached.Get(m_view));
                }
            }

            auto it = Next(Begin(m_view), End(m_view));

            if constexpr(NeedsCachedBegin)
            {
                m_cached.Set(m_view, it);
            }

            return MakeReverseIterator(Move(it));
        }

        template<typename Self>
        constexpr auto
        Begin(this Self &&self) noexcept(noexcept(MakeReverseIterator(Ranges::End(Forward<Self>(self).m_view))))
            requires CommonRangeConcept<View>
        {
            return MakeReverseIterator(Ranges::End(Forward<Self>(self).m_view));
        }

        template<typename Self>
        constexpr auto
        End(this Self &&self) noexcept(noexcept(MakeReverseIterator(Ranges::Begin(Forward<Self>(self).m_view))))
            requires CommonRangeConcept<View>
        {
            return MakeReverseIterator(Ranges::Begin(Forward<Self>(self).m_view));
        }

      private:
        static constexpr bool NeedsCachedBegin =
            !CommonRangeConcept<View> &&
            !(RandomAccessRangeConcept<View> && SizedSentinelForConcept<SentinelType<View>, IteratorType<View>>);

        View m_view;
        SSSENGINE_OVERLAP
        MaybeEmptyType<NeedsCachedBegin, CachedIterator<View>> m_cached;

        // NOLINTBEGIN(readability-identifier-naming)

        friend auto begin(const ReverseView &view) noexcept(noexcept(view.Begin()))
        {
            return view.Begin();
        }

        friend auto end(const ReverseView &view) noexcept(noexcept(view.End()))
        {
            return view.End();
        }

        // NOLINTEND(readability-identifier-naming)
    };

    template<typename View>
    SSSENGINE_GLOBAL
    constexpr bool EnableBorrowRange<ReverseView<View>> = EnableBorrowRange<View>;

    template<typename Range>
    ReverseView(Range &&) -> ReverseView<Ranges::AllType<Range>>;

    // NOLINTBEGIN(bugprone-reserved-identifier, readability-identifier-naming)
    namespace __impl
    {
        template<typename>
        SSSENGINE_GLOBAL
        constexpr bool __IsReversibleSubrange = false;

        template<typename It, SubrangeKind Kind>
        SSSENGINE_GLOBAL
        constexpr bool __IsReversibleSubrange<Subrange<ReverseIterator<It>, ReverseIterator<It>, Kind>> = true;

        template<typename>
        SSSENGINE_GLOBAL
        constexpr bool __IsReverseView = false;

        template<typename View>
        SSSENGINE_GLOBAL
        constexpr bool __IsReverseView<ReverseView<View>> = true;

        template<typename Range>
        concept __CanReverseViewConcept = requires { ReverseView{DeclVal<Range>()}; };

        struct _Reverse : RangeAdaptorClosure<_Reverse>
        {
            template<ViewableRangeConcept Range>
                requires __impl::__IsReverseView<RemoveCVReferenceType<Range>> ||
                         __impl::__IsReversibleSubrange<RemoveCVReferenceType<Range>> ||
                         __impl::__CanReverseViewConcept<Range>
        SSSENGINE_PURE
            constexpr auto operator()(Range &&range) const
            {
                using Type = RemoveCVReferenceType<Range>;
                if constexpr(__impl::__IsReverseView<Type>)
                {
                    return Forward<Range>(range).GetView();
                }
                else if constexpr(__impl::__IsReversibleSubrange<Type>)
                {
                    using It = decltype(Ranges::Begin(range).Underlying());
                    if constexpr(SizedRangeConcept<Type>)
                        return Subrange<It, It, SubrangeKind::Sized>{
                            range.End().Underlying(), range.Begin().Underlying(), range.Count()};
                    else
                        return Subrange<It, It, SubrangeKind::Unsized>{range.End().Underlying(), range.Begin().Underlying()};
                }
                else
                    return ReverseView{Forward<Range>(range)};
            }

            static constexpr bool SimpleCallOp = true;
        };

    } // namespace __impl

    // NOLINTEND(bugprone-reserved-identifier, readability-identifier-naming)

    SSSENGINE_GLOBAL
    __impl::_Reverse Reverse;

} // namespace SSSEngine::Ranges
