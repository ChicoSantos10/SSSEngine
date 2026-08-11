/**
 * @file
 * @brief
 */

#pragma once

#include "Buffer.h"
#include "Concepts.h"
#include "MemorySize.h"

namespace SSSEngine::Memory
{
    template<typename T>
    concept AllocatorConcept = requires(T allocator, Math::Bytes size, SizeType align, Buffer buffer) {
        { allocator.Allocate(size, align) } -> ConvertibleToConcept<void *>;
        { allocator.Free(buffer) } -> SameAsConcept<void>;
    };
} // namespace SSSEngine::Memory
