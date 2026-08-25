/**
 * @file
 * @brief
 */

#pragma once

#include "Allocator.h"
#include "Attributes.h"
#include "BasicIterator.h"
#include "Debug.h"
#include "MemorySize.h"
#include "Optional.h"
#include "QualifierTraits.h"
#include "ReverseIterator.h"
#include "Traits.h"
#include "Types.h"
#include "Utility.h"

namespace SSSEngine::Containers
{
    template<typename T>
    class DynamicArray
    {
      private:
        static constexpr SizeType Alignment = alignof(T);
        static constexpr SizeType StartCapacity = 12;
        static constexpr Math::Bytes DefaultStartSize = Math::Bytes(sizeof(T) * StartCapacity);

        template<typename U>
        using SelectReferenceType = ConditionalType<IsConst<U>, const T &, T &>;

      public:
        using ValueType = T;
        using ElementType = RemoveCVType<T>;
        using PointerType = ValueType *;
        using ConstPointerType = const ValueType *;
        using ReferenceType = ValueType &;
        using ConstReferenceType = const ValueType &;

        using Iterator = Ranges::BasicIterator<ValueType *>;
        using ConstIterator = Ranges::BasicIterator<const ValueType *>;
        using ReverseIterator = Ranges::ReverseIterator<Iterator>;
        using ConstReverseIterator = Ranges::ReverseIterator<ConstIterator>;

      private:
        template<typename U>
        using IteratorType = ConditionalType<IsConst<U>, ConstIterator, Iterator>;

      public:
        constexpr DynamicArray() :
            m_data(static_cast<T *>(Memory::CurrentAllocator().Allocate(DefaultStartSize, Alignment))),
            m_capacity(StartCapacity)
        {
        }

        constexpr explicit DynamicArray(SizeType startingCapacity) :
            m_data(Memory::CurrentAllocator().Allocate(Math::Bytes(sizeof(T) * startingCapacity), Alignment)),
            m_capacity(startingCapacity)
        {
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr decltype(auto) operator[](this Self &self, SizeType index) noexcept
        {
            SSSENGINE_ASSERT(Forward<Self>(self).ValidIndex(index));
            return Forward<Self>(self).m_data[index];
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr T operator[](this Self &&self, SizeType index) noexcept
        {
            return static_cast<Self &>(self)[index];
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Optional<SelectReferenceType<Self>> TryAt(this Self &self, SizeType index) noexcept
        {
            if(self.ValidIndex(index))
            {
                return {self.m_data[index]};
            }

            return {};
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Optional<T> TryAt(this Self &&self, SizeType index) noexcept
        {
            if(self.ValidIndex(index))
            {
                return {self.m_data[index]};
            }

            return {};
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr decltype(auto) Front(this Self &&self) noexcept
        {
            return Forward<Self>(self)[0];
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr decltype(auto) Back(this Self &&self) noexcept
        {
            return Forward<Self>(self)[Forward<Self>(self).m_count - 1];
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr IteratorType<Self> Begin(this Self &self) noexcept
        {
            return IteratorType<Self>(self.m_data);
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr IteratorType<Self> End(this Self &self) noexcept
        {
            return IteratorType<Self>(self.m_data + self.m_count);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator ConstBegin() const noexcept
        {
            return ConstIterator(m_data);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator ConstEnd() const noexcept
        {
            return ConstIterator(m_data + m_count);
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto ReverseBegin(this Self &self) noexcept
        {
            return Ranges::MakeReverseIterator(self.End());
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto ReverseEnd(this Self &self) noexcept
        {
            return Ranges::MakeReverseIterator(self.Begin());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ReverseBegin();
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstReverseIterator ConstReverseEnd() const noexcept
        {
            return ReverseEnd();
        }

        template<typename Self>
            SSSENGINE_FORCE_INLINE
        constexpr void PushBack(this Self &self, const T &value) noexcept
        {
            if(self.m_count < self.m_capacity)
            {
                ConstructAt(self.End(), value);
            }
            else
            {
                // TODO: Reallocate more space and add
            }

            ++self.m_count;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr PointerType Data() noexcept
        {
            return static_cast<PointerType>(m_data);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstPointerType Data() const noexcept
        {
            return static_cast<ConstPointerType>(m_data);
        }

        // TODO: Count, Capacity, Empty, Reserve, Shrink
        // Clear, PushFront, Emplace, Pop
        // GetRange / Span
        // Destructor

      private:
        T *m_data;
        SizeType m_count = 0;
        SizeType m_capacity = 0;

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool ValidIndex(SizeType index) const noexcept
        {
            return index >= 0 && index < m_count;
        }

        // NOLINTBEGIN(readability-identifier-naming)

        friend constexpr decltype(auto) begin(const DynamicArray<T> &array)
        {
            return array.Begin();
        }

        friend constexpr decltype(auto) end(const DynamicArray<T> &array)
        {
            return array.End();
        }

        // NOLINTEND(readability-identifier-naming)
    };

} // namespace SSSEngine::Containers

namespace SSSEngine
{
    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyRelocatable<Containers::DynamicArray<T>> = true;
}
