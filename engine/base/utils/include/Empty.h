/**
 * @file
 * @brief
 */

#pragma once

#include "Traits.h"

namespace SSSEngine
{
    struct Empty
    {
    };

    /**
     * @brief A type that is T if Cond is true otherwise it's an empty struct
     *
     */
    template<bool Cond, typename T>
    using MaybeEmptyType = ConditionalType<Cond, T, Empty>;

} // namespace SSSEngine
