/**
 * @file
 * @brief
 */

#pragma once

#include "Iterator.h"

namespace SSSEngine::Ranges
{
    enum class SubrangeKind : bool
    {
        Unsized,
        Sized
    };

    template<IteratorConcept It,
             SentinelForConcept<It> Sentinel = It,
             SubrangeKind Kind = SizedSentinelForConcept<Sentinel, It> ? SubrangeKind::Sized : SubrangeKind::Unsized>
        requires(Kind == SubrangeKind::Sized || !SizedSentinelForConcept<Sentinel, It>)
    class Subrange
    {
      private:
        It m_iterator;
        Sentinel m_sentinel;
    };
} // namespace SSSEngine::Ranges
