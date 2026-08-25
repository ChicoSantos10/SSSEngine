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
 * @brief
 */

#pragma once

#include "Address.h"
#include "Attributes.h"
#include "BasicIterator.h"
#include "Concepts.h"
#include "Debug.h"
#include "Encoding.h"
#include "Iterator.h"
#include "Memory.h"
#include "Range.h"
#include "String.h"
#include "StringView.h"
#include "Types.h"
#include "Utility.h"
#include "Container.h"
#include "Span.h"

namespace SSSEngine::Containers
{
    template<typename T>
    concept SinkConcept = true;

    // template<typename T>
    // concept SinkConcept = requires { T::Iterator; } && requires(T &sink, SizeType n, T::Iterator it) {
    //     { sink.Reserve(n) } -> SameAsConcept<bool>;
    //     { sink.Current() } -> Ranges::IteratorConcept;
    //     { Move(sink).Get() };
    //     { sink.AdvanceTo(it) };
    //     { sink.Advance(n) };
    // };

    template<typename T>
    struct StackBuffer
    {
        static constexpr SizeType StackSize = Memory::CacheLineConstructive * 5;
        static constexpr SizeType Elements = StackSize / sizeof(T);

        using Iterator = Ranges::BasicIterator<T>;
        using DifferenceType = Ranges::IteratorDifferenceType<Iterator>;

        Array<T, Elements> storage;
        Iterator current;

        DifferenceType Used(StackBuffer buffer) noexcept
        {
            return buffer.current - buffer.storage.Begin();
        }

        DifferenceType Unused(StackBuffer buffer) noexcept
        {
            return buffer.storage.End() - buffer.current;
        }
    };

    template<ContainerConcept Container>
    class RangeSink : StackBuffer<typename Container::ValueType>
    {
        using Base = StackBuffer<typename Container::ValueType>;
        using Base::Used;
        using Base::Unused;

      public:
        RangeSink() = default;

        auto Reserve(SizeType amount)
        {
            if(amount > Unused()) SSSENGINE_UNLIKELY
            {
                Flush();
            }

            auto current = this->current;
            this->current += amount;

            return current;
        }

        void Flush()
        {
            auto amount = Used();
            m_range.Reserve(amount);

            m_range.Append(Subrange(this->storage.Begin(), this->current));

            this->current = this->storage.Begin();
        }

        Container Get() &&
        {
            return Move(m_range);
        }

      private:
        Container m_range;
    };

    template<SinkConcept Sink>
    class SinkIterator
    {
        using Encoding = Sink::Encoding;
        using View = Text::StringView<Encoding>;

      public:
        SinkIterator() noexcept = default;
        SinkIterator(const SinkIterator &other) noexcept = default;
        constexpr SinkIterator &operator=(const SinkIterator &) = default;

        constexpr explicit SinkIterator(Sink &sink) noexcept : m_sink(AddressOf(sink)) {}

        constexpr SinkIterator &operator=(View string)
        {
            m_sink->Write(string);
            return *this;
        }

        constexpr SinkIterator &operator*()
        {
            return *this;
        }

        constexpr SinkIterator &operator++() noexcept
        {
            return *this;
        }

        constexpr SinkIterator &operator++(int) noexcept
        {
            return *this;
        }

      private:
        Sink *m_sink;
    };

    template<Text::EncodingConcept E>
    class StringSink
    {
        using CharType = E::CodeUnitType;
        using String = Text::String<E>;
        using View = Text::StringView<E>;

      public:
        using Encoding = E;

        void Write(View string)
        {
            m_string.Append(string);
        }

        String Get() &&
        {
            return Move(m_string);
        }

        SinkIterator<StringSink> Out() noexcept
        {
            return SinkIterator<StringSink>(*this);
        }

      private:
        String m_string;
    };

} // namespace SSSEngine::Containers
