/**
 * @file
 * @brief
 */

#pragma once

#include "Attributes.h"
#include "Concepts.h"
#include "CopyAndMoveTraits.h"
#include "Empty.h"
#include "HelperMacros.h"
#include "Iterator.h"
#include "Range.h"
#include "ReverseIterator.h"
#include "View.h"

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
        constexpr auto Begin(this Self &&self) noexcept(noexcept(MakeReverseIterator(Ranges::End(m_view))))
            requires CommonRangeConcept<View>
        {
            return MakeReverseIterator(Ranges::End(Forward<Self>(self).m_view));
        }

        template<typename Self>
        constexpr auto End(this Self &&self) noexcept(noexcept(MakeReverseIterator(Ranges::End(m_view))))
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

} // namespace SSSEngine::Ranges
