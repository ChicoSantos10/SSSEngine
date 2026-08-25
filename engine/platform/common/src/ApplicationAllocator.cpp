#include "ApplicationAllocator.h"
#include "Buffer.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Math.h"
#include "Memory.h"
#include "MemorySize.h"
#include "Types.h"
#include "Byte.h"

namespace SSSEngine::Memory
{
    SSSENGINE_INTERNAL Buffer Memory;
    SSSENGINE_INTERNAL u32 CurrentPageOffset = 0;

    void Reserve(u32 pages)
    {
        SSSENGINE_ASSERT(Memory.address == nullptr);
        SSSENGINE_ASSERT(Memory.capacity == 0_B);
        Memory = ReserveMemory(pages * GetSystemPageSize());
    }

    Buffer Request(Math::Bytes bytes)
    {
        if(Memory.address == nullptr)
        {
            Reserve(Math::NextMultiplePowerOf2(bytes.value, GetSystemPageSize().value) / GetSystemPageSize().value);
        }

        void *startAddress = static_cast<Byte *>(Memory.address) + CurrentPageOffset * GetSystemPageSize().value;
        ++CurrentPageOffset;

        Buffer buffer{.address = startAddress, .capacity = GetSystemPageSize()};
        CommitMemory(buffer);

        return buffer;
    }

    void Release(Buffer buffer)
    {
        // FIXME: This does not take into consideration system pages! We should only free or release pages and not
        // individual blocks of memory!
        ReleaseMemory(buffer);
    }
} // namespace SSSEngine::Memory
