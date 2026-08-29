/**
 * @file
 * @brief
 */

#pragma once

#include "Concepts.h"

namespace SSSEngine
{
    template<typename T>
    concept BigIntConcept = SameAsConcept<T, i128> || SameAsConcept<T, u128>;

} // namespace SSSEngine
