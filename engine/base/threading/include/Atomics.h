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
 * @brief Atomics
 */

#pragma once

#include "Attributes.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "QualifierTraits.h"
#include "Address.h"
#include "Threading.h"
#include "Types.h"
#include "ValueTraits.h"
#include "Concepts.h"
#include "MemoryUtility.h"

namespace SSSEngine::Threading
{
    /**
     * @enum MemoryOrder
     * @brief Represents the memory order for atomic operations
     *
     * For more information use the C++ reference documentation:
     * https://en.cppreference.com/w/cpp/atomic/memory_order.html
     *
     */
    enum class MemoryOrder : u8
    {
        /** No synchronization or ordering constraints. Only guarantees atomicity */
        Relaxed = 0,

        /**
         * No reads or writes in the current thread can be reordered before this load. All writes in
         * other threads that release the same atomic are visible in the current thread
         */
        Acquire = 2,

        /*
         * No reads or writes in the current thread can reordered after this store. All writes in the current thread are
         * visible in other threads that acquire the same atomic variable and writes that carry a dependency into the
         * atomic variable become visible in other threads that consume the same atomic
         */
        Release,

        /*
         * Combines both acquire and release
         */
        AcquireRelease,

        /*
         * Ensures that all operations will occur sequentially. Safest but potentially slower
         */
        SequentiallyConsistentOrdering
    };

    /**
     * @brief Checks that a certain memory order is valid store order
     *
     * @param order The memory order to check
     * @return True if it's valid, false otherwise
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE bool constexpr IsValidStoreMemoryOrder(MemoryOrder order)
    {
        using enum MemoryOrder;
        return order != Acquire && order != AcquireRelease;
    }

    /**
     * @brief Checks that a certain memory order is valid load order
     *
     * @param order The memory order to check
     * @return True if it's valid, false otherwise
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE bool constexpr IsValidLoadMemoryOrder(MemoryOrder order)
    {
        using enum MemoryOrder;
        return order != Release && order != AcquireRelease;
    }

    /**
     * @brief Checks that a certain memory order is valid failure order for compare exchange operations
     *
     * Failure operations must be less or equally restrictive as the success order
     * @see FailureOrder to check what failure order maps to what success order. Note that FailureOrder uses that same
     * restriction when possible, but is not necessarily the only possible value for a certain success order
     *
     * @param order The memory order to check
     * @return True if it's valid, false otherwise
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE bool constexpr IsValidFailureMemoryOrder(MemoryOrder success, MemoryOrder failure)
    {
        using enum MemoryOrder;
        return failure != Release && failure != AcquireRelease && int(success) >= int(failure);
    }

    /**
     * @brief Derives a failure memory order based on another memory order
     *
     * Failure memory order must be below compared to success order
     *
     * @param order The order to which we derive the failure order
     * @return A derived memory order
     */
    constexpr MemoryOrder FailureOrder(MemoryOrder success) noexcept
    {
        using enum MemoryOrder;
        switch(success)
        {
            case AcquireRelease:
                return Acquire;
            case MemoryOrder::Release:
                return Relaxed;
            case MemoryOrder::Relaxed:
            case MemoryOrder::Acquire:
            case MemoryOrder::SequentiallyConsistentOrdering:
                return success;
            default:
                SSSENGINE_UNREACHABLE;
        }
    }

    /**
     * @brief Checks if it's possible to clear padding on Type T
     *
     * @tparam T The Type to check
     * @return True if the type has padding and it's possible to clear it, false otherwise
     */
    template<typename T>
    SSSENGINE_GLOBAL consteval bool CanClearPadding()
    {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
    #if __has_builtin(__builtin_clear_padding)
        return !HasUniqueObjectRepresentation<T> && !IsSameType<T, float> && !IsSameType<T, double>;
    #else
        return false;
    #endif
#endif
    }

    /**
     * @brief Clears padding if possible
     *
     * @important It may not be possible to clear padding even if the Type has padding. This is because we are relying
     * on compiler features that may not be implemented. If not possible to clear, this just returns the memory address
     * of value
     *
     * @tparam T Type to clear the padding of
     * @param value A reference to the variable to clear it's padding
     * @return The memory address of the type with the padding its cleared if possible
     */
    template<typename T>
    SSSENGINE_FORCE_INLINE constexpr T *ClearPadding(T &value) noexcept
    {
        auto *ptr = AddressOf(value);
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
    #if __has_builtin(__builtin_clear_padding)
        __builtin_clear_padding(ptr);
    #endif
#endif
        return ptr;
    }

    template<typename T>
    concept AtomicTypeConcept = IsTriviallyCopyable<T> && IsCopyConstructible<T> && IsMoveConstructible<T> &&
                                IsCopyAssignable<T> && IsMoveAssignable<T>;

    /**
     * @class Atomic
     * @brief An object where operations are atomic
     *
     * @tparam T The Type to perform operations on
     */
    template<AtomicTypeConcept T>
    class Atomic
    {
        static constexpr bool IsIntegralType = IsIntegral<T>;
        static constexpr bool IsPointerType = IsPointer<T>;
        using UnderlyingType = RemovePointerType<T>;

        public:
        using Type = T;

        constexpr Atomic() noexcept(IsNoThrowDefaultConstructible<T>)
            requires IsDefaultConstructible<T>
        : m_value()
        {
        }

        ~Atomic() noexcept = default;

        Atomic(const Atomic &) = delete;
        Atomic(Atomic &&) = delete;
        Atomic &operator=(const Atomic &) = delete;
        Atomic &operator=(const Atomic &) volatile = delete;
        Atomic &operator=(Atomic &&) = delete;

        constexpr explicit Atomic(T value) noexcept : m_value(value)
        {
            if constexpr(CanClearPadding<T>())
            {
                ClearPadding(m_value);
            }
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator++(this NotConstConcept auto &self) noexcept
            requires(IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_add_fetch(&self.m_value, 1, int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator++(this NotConstConcept auto &self) noexcept
            requires(IsPointerType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_add_fetch(&self.m_value, sizeof(UnderlyingType), int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator++(this NotConstConcept auto &self, int) noexcept
            requires(IsIntegralType || IsPointerType)
        {
            return self.FetchAdd(1, Order);
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator--(this NotConstConcept auto &self) noexcept
            requires(IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_sub_fetch(&self.m_value, 1, int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator--(this NotConstConcept auto &self) noexcept
            requires(IsPointerType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_sub_fetch(&self.m_value, sizeof(UnderlyingType), int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator--(this NotConstConcept auto &self, int) noexcept
            requires(IsIntegralType || IsPointerType)
        {
            return self.FetchSub(1, Order);
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator+=(this NotConstConcept auto &self, T value) noexcept
            requires(IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_add_fetch(&self.m_value, value, int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator+=(this NotConstConcept auto &self, ptrdiff value) noexcept
            requires(IsPointerType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_add_fetch(&self.m_value, value * sizeof(UnderlyingType), int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator-=(this NotConstConcept auto &self, T value) noexcept
            requires(IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_sub_fetch(&self.m_value, value, int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator-=(this NotConstConcept auto &self, ptrdiff value) noexcept
            requires(IsPointerType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_sub_fetch(&self.m_value, value * sizeof(UnderlyingType), int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator&=(this NotConstConcept auto &self, T value) noexcept
            requires(IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_and_fetch(&self.m_value, value, int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator|=(this NotConstConcept auto &self, T value) noexcept
            requires(IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_or_fetch(&self.m_value, value, int(Order));
#endif
        }

        template<MemoryOrder Order = MemoryOrder::Relaxed>
        T operator^=(this NotConstConcept auto &self, T value) noexcept
            requires(IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_xor_fetch(&self.m_value, value, int(Order));
#endif
        }

        /**
         * @brief Checks if Type is always lock free
         */
        static constexpr bool IsAlwaysLockFree = __atomic_always_lock_free(sizeof(T), nullptr);

        /**
         * @brief Checks if object if lock free
         *
         * @return True if is lock free, false otherwise
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE bool IsLockFree(this auto &self) noexcept
        {
            if constexpr(IsAlwaysLockFree)
            {
                return true;
            }
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_is_lock_free(sizeof(self.m_value),
                                         reinterpret_cast<void *>(-Alignment)); // NOLINT(performance-no-int-to-ptr)
#endif // SSSENGINE_MSVC
        }

        /**
         * @brief Stores a value into this atomic
         *
         * @param value The value to store
         * @param memoryOrder The memory order to use
         */
        SSSENGINE_FORCE_INLINE void Store(this auto &self, T value, MemoryOrder memoryOrder) noexcept
            requires(!IsIntegralType)
        {
            SSSENGINE_ASSERT(IsValidStoreMemoryOrder(memoryOrder));
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            __atomic_store(AddressOf(self.m_value), ClearPadding(value), int(memoryOrder));
#endif // SSSENGINE_MSVC
        }

        /**
         * @brief Stores a value into this atomic
         *
         * @param value The value to store
         * @param memoryOrder The memory order to use
         */
        SSSENGINE_FORCE_INLINE void Store(this auto &self, T value, MemoryOrder memoryOrder) noexcept
            requires(IsIntegralType)
        {
            SSSENGINE_ASSERT(IsValidStoreMemoryOrder(memoryOrder));
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            __atomic_store_n(&self.m_value, value, int(memoryOrder));
#endif // SSSENGINE_MSVC
        }

        /**
         * @brief Loads the value from this atomic
         *
         * @param memoryOrder The memory order to use
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE T Load(this auto &self, MemoryOrder memoryOrder) noexcept
        {
            SSSENGINE_ASSERT(IsValidLoadMemoryOrder(memoryOrder));
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            alignas(T) byte buffer[sizeof(T)];
            T *ptr = reinterpret_cast<T *>(buffer);
            __atomic_load(AddressOf(self.m_value), ptr, int(memoryOrder));
            return *ptr;
#endif // SSSENGINE_MSVC
        }

        /**
         * @brief Loads the value from this atomic
         *
         * @param memoryOrder The memory order to use
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE T Load(this auto &self, MemoryOrder memoryOrder) noexcept
            requires(IsIntegralType)
        {
            SSSENGINE_ASSERT(IsValidLoadMemoryOrder(memoryOrder));
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            return __atomic_load_n(&self.m_value, int(memoryOrder));
#endif // SSSENGINE_MSVC
        }

        /**
         * @brief Loads and adds the value in one atomic operation
         *
         * @param value The value to add
         * @param order The memory order to use
         * @return The original value before the add
         */
        SSSENGINE_FORCE_INLINE T FetchAdd(this NotConstConcept auto &self, T value, MemoryOrder order) noexcept
            requires(IsIntegralType)
        {
            return __atomic_fetch_add(&self.m_value, value, int(order));
        }

        /**
         * @brief Loads and adds the value in one atomic operation
         *
         * @param value The value to add
         * @param order The memory order to use
         * @return The original value before the add
         */
        SSSENGINE_FORCE_INLINE T FetchAdd(this NotConstConcept auto &self, ptrdiff value, MemoryOrder order) noexcept
            requires(IsPointerType)
        {
            return __atomic_fetch_add(&self.m_value, value * sizeof(UnderlyingType), int(order));
        }

        /**
         * @brief Loads and subtracts the value in one atomic operation
         *
         * @param value The value to subtract
         * @param order The memory order to use
         * @return The original value before the subtraction
         */
        SSSENGINE_FORCE_INLINE T FetchSub(this NotConstConcept auto &self, T value, MemoryOrder order) noexcept
            requires(IsIntegralType)
        {
            return __atomic_fetch_sub(&self.m_value, value, int(order));
        }

        /**
         * @brief Loads and subtracts the value in one atomic operation
         *
         * @param value The value to subtract
         * @param order The memory order to use
         * @return The original value before the subtract
         */
        SSSENGINE_FORCE_INLINE T FetchSub(this NotConstConcept auto &self, ptrdiff value, MemoryOrder order) noexcept
            requires(IsPointerType)
        {
            return __atomic_fetch_sub(&self.m_value, value * sizeof(UnderlyingType), int(order));
        }

        /**
         * @brief Loads and performs a bitwise AND with the value in one atomic operation
         *
         * @param value The value to perform the bitwise AND
         * @param order The memory order to use
         * @return The original value before the AND
         */
        SSSENGINE_FORCE_INLINE T FetchAnd(this NotConstConcept auto &self, T value, MemoryOrder order) noexcept
            requires(IsIntegralType)
        {
            return __atomic_fetch_and(&self.m_value, value, int(order));
        }

        /**
         * @brief Loads and performs a bitwise OR with the value in one atomic operation
         *
         * @param value The value to perform the bitwise OR
         * @param order The memory order to use
         * @return The original value before the OR
         */
        SSSENGINE_FORCE_INLINE T FetchOr(this NotConstConcept auto &self, T value, MemoryOrder order) noexcept
            requires(IsIntegralType)
        {
            return __atomic_fetch_or(&self.m_value, value, int(order));
        }

        /**
         * @brief Loads and performs a bitwise XOR with the value in one atomic operation
         *
         * @param value The value to perform the bitwise XOR
         * @param order The memory order to use
         * @return The original value before the XOR
         */
        SSSENGINE_FORCE_INLINE T FetchXor(this NotConstConcept auto &self, T value, MemoryOrder order) noexcept
            requires(IsIntegralType)
        {
            return __atomic_fetch_xor(&self.m_value, value, int(order));
        }

        /**
         * @brief Exchanges the current value with a new value and returns the previous value
         *
         * @param value The new value
         * @param memoryOrder The memory order to use
         * @return The original value before the change
         */
        SSSENGINE_FORCE_INLINE T Exchange(this NotConstConcept auto &self, T value, MemoryOrder memoryOrder) noexcept
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
            alignas(T) byte buffer[sizeof(T)];
            T *ptr = reinterpret_cast<T *>(buffer);
            __atomic_exchange(AddressOf(self.m_value), ClearPadding(value), ptr, int(memoryOrder));
            return *ptr;
#endif // SSSENGINE_MSVC
        }

        // INVESTIGATE: What is this AtomicRef
        /**
         * @brief Compares the value with the expected and updates to desired if they are the same. If they are
         * different updates the expected with current value
         *
         * @param expected The value to compare to
         * @param desired The value to update to if current value and expected are equal
         * @param isWeak Whether to use weak ordering
         * @param orderSuccess The memory order to use when the values are equal
         * @param orderFailure The memory order to use when the values are different
         * @return True if the values are equal, false otherwise
         */
        template<bool AtomicRef = false>
        SSSENGINE_FORCE_INLINE bool
        CompareExchange(this NotConstConcept auto &self, RemoveVolatileType<T> &expected, RemoveVolatileType<T> &desired,
                        bool isWeak, MemoryOrder orderSuccess, MemoryOrder orderFailure) noexcept
            requires(!IsIntegralType)
        {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC

            using WithoutVolatileType = RemoveVolatile<T>;
            T *const valueAddress = AddressOf(self.m_value);

            if constexpr(!CanClearPadding<WithoutVolatileType>())
            {
                return __atomic_compare_exchange(
                    valueAddress, AddressOf(expected), AddressOf(desired), isWeak, int(orderSuccess), int(orderFailure));
            }
            else if constexpr(!AtomicRef)
            {
                WithoutVolatileType *const ptrDesired = ClearPadding(desired);
                WithoutVolatileType expectedCopy = expected;
                WithoutVolatileType *const ptrExpected = ClearPadding(expectedCopy);

                if(__atomic_compare_exchange(
                       valueAddress, ptrExpected, ptrDesired, isWeak, int(orderSuccess), int(orderFailure)))
                {
                    return true;
                }

                MemoryCopy(AddressOf(expected), ptrExpected, sizeof(WithoutVolatileType));
                return false;
            }
            else
            {
                WithoutVolatileType *const ptrDesired = ClearPadding(desired);
                WithoutVolatileType copy = expected;
                WithoutVolatileType *const ptrExpected = ClearPadding(copy);

                while(true)
                {
                    WithoutVolatileType original = copy;

                    if(__atomic_compare_exchange(
                           valueAddress, ptrExpected, ptrDesired, isWeak, int(orderSuccess), int(orderFailure)))
                    {
                        return true;
                    }

                    WithoutVolatileType current = copy;

                    if(MemoryCompare(ClearPadding(original), ClearPadding(current), sizeof(WithoutVolatileType)))
                    {
                        MemoryCopy(AddressOf(expected), ptrExpected, sizeof(WithoutVolatileType));
                        return false;
                    }
                }
            }
#endif // SSSENGINE_MSVC
        }

        /**
         * @brief Compares the value with the expected and updates to desired if they are the same. If they are
         * different updates the expected with current value
         *
         * @param expected The value to compare to
         * @param desired The value to update to if current value and expected are equal
         * @param isWeak Whether to use weak ordering
         * @param orderSuccess The memory order to use when the values are equal
         * @param orderFailure The memory order to use when the values are different
         * @return True if the values are equal, false otherwise
         */
        SSSENGINE_FORCE_INLINE bool CompareExchange(this NotConstConcept auto &self, T &expected, T desired, bool isWeak,
                                                    MemoryOrder successOrder, MemoryOrder failureOrder) noexcept
            requires(IsIntegralType)
        {
            SSSENGINE_ASSERT(IsValidFailureMemoryOrder(successOrder, failureOrder));
            return __atomic_compare_exchange_n(
                &self.m_value, &expected, desired, isWeak, int(successOrder), int(failureOrder));
        }

        /**
         * @brief Compares the value with the expected and updates to desired if they are the same. If they are
         * different updates the expected with current value
         *
         * @param expected The value to compare to
         * @param desired The value to update to if current value and expected are equal
         * @param isWeak Whether to use weak ordering
         * @param orderSuccess The memory order to use when the values are equal
         * @return True if the values are equal, false otherwise
         */
        SSSENGINE_FORCE_INLINE
        bool CompareExchange(this NotConstConcept auto &self, T &expected, T desired, bool isWeak, MemoryOrder order) noexcept
        {
            return self.CompareExchange(expected, desired, isWeak, order, FailureOrder(order));
        }

        /**
         * @brief Compares the value with the expected and updates to desired if they are the same. If they are
         * different updates the expected with current value
         *
         * Spurious fails are allowed
         *
         * @param expected The value to compare to
         * @param desired The value to update to if current value and expected are equal
         * @param orderSuccess The memory order to use when the values are equal
         * @param orderFailure The memory order to use when the values are different
         * @return True if the values are equal, false otherwise
         */
        SSSENGINE_FORCE_INLINE bool CompareExchangeWeak(this NotConstConcept auto &self, T &expected, T desired,
                                                        MemoryOrder successOrder, MemoryOrder failureOrder) noexcept
        {
            return self.CompareExchange(expected, desired, true, successOrder, failureOrder);
        }

        /**
         * @brief Compares the value with the expected and updates to desired if they are the same. If they are
         * different updates the expected with current value
         *
         * Spurious fails are allowed
         *
         * @param expected The value to compare to
         * @param desired The value to update to if current value and expected are equal
         * @param orderSuccess The memory order to use when the values are equal
         * @return True if the values are equal, false otherwise
         */
        SSSENGINE_FORCE_INLINE
        bool CompareExchangeWeak(this NotConstConcept auto &self, T &expected, T desired, MemoryOrder order) noexcept
        {
            return self.CompareExchange(expected, desired, true, order, FailureOrder(order));
        }

        /**
         * @brief Compares the value with the expected and updates to desired if they are the same. If they are
         * different updates the expected with current value
         *
         * Comparison is guaranteed to return true when equal or false otherwise
         *
         * @param expected The value to compare to
         * @param desired The value to update to if current value and expected are equal
         * @param orderSuccess The memory order to use when the values are equal
         * @param orderFailure The memory order to use when the values are different
         * @return True if the values are equal, false otherwise
         */
        SSSENGINE_FORCE_INLINE bool CompareExchangeStrong(this NotConstConcept auto &self, T &expected, T desired,
                                                          MemoryOrder successOrder, MemoryOrder failureOrder) noexcept
        {
            return self.CompareExchange(expected, desired, false, successOrder, failureOrder);
        }

        /**
         * @brief Compares the value with the expected and updates to desired if they are the same. If they are
         * different updates the expected with current value
         *
         * Comparison is guaranteed to return true when equal or false otherwise
         *
         * @param expected The value to compare to
         * @param desired The value to update to if current value and expected are equal
         * @param orderSuccess The memory order to use when the values are equal
         * @return True if the values are equal, false otherwise
         */
        SSSENGINE_FORCE_INLINE bool
        CompareExchangeStrong(this NotConstConcept auto &self, T &expected, T desired, MemoryOrder order) noexcept
        {
            return self.CompareExchange(expected, desired, false, order, FailureOrder(order));
        }

        /**
         * @brief Waits while this atomic value is equal to old
         *
         * @param old The value to wait on
         */
        void Wait(this auto &self, T old) noexcept
            requires(IsSameType<T, u32>)
        {
            Threading::Wait(&self.value, old);
        };

        /**
         * @brief Notifies one thread that is waiting on the address of this atomic
         *
         */
        void NotifyOne(this auto &self) noexcept
            requires(IsSameType<T, u32>)
        {
            Threading::NotifyOne(&self.m_value);
        }

        /**
         * @brief Notifies all threads that are waiting on the address of this atomic
         *
         */
        void NotifyAll(this auto &self) noexcept
            requires(IsSameType<T, u32>)
        {
            Threading::NotifyAll(&self.m_value);
        }

        private:
        static constexpr int MinAlignment = (sizeof(T) & (sizeof(T) - 1)) || sizeof(T) > 16 ? 0 : sizeof(T);
        static constexpr int Alignment = MinAlignment > alignof(T) ? MinAlignment : alignof(T);

        alignas(Alignment) Type m_value;
    };
} // namespace SSSEngine::Threading
