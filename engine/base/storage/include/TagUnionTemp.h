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

#include "Algorithm.h"
#include "ArrayTraits.h"
#include "Attributes.h"
#include "ConversionTraits.h"
#include "Debug.h"
#include "QualifierTraits.h"
#include "Traits.h"
#include "Types.h"
#include "VariadicUnion.h"

namespace SSSEngine
{

    template<typename T>
    concept ValidTagUnionMemberConcept = !IsLValueReference<T> && !IsArray<T> && !IsVoid<T>;

    template<typename T, typename... Args>
    concept TagUnionMemberTypeConcept = ValidTagUnionMemberConcept<T> && IsAnyType<T, Args...>;

    template<typename... Types>
        requires(sizeof...(Types) > 1) && (ValidTagUnionMemberConcept<Types> && ...) && AreAllUnique<Types...>
    class TagUnion
    {
      private:
      public:
        using Traits = VariadicUnionTypesCommonTraits<Types...>;

        template<typename T>
        static constexpr TagType IndexOf = FindPackedArgIndex<TagType, T, Types...>;

        // NOLINTBEGIN(*-explicit-constructor)

        // NOLINTEND(*-explicit-constructor)

      private:
        template<typename Self, SizeType N>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr decltype(auto) Get(this Self &&self) noexcept
        {
            return GetElementAt<N>(Forward<Self>(self).m_storage);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool ValidTag() const noexcept
        {
            return m_tag != UninitializedTag;
        }

      private:
        static constexpr TagType UninitializedTag = -1;

        VariadicUnion<Traits::TrivialDestructor, Types...> m_storage;
        TagType m_tag = UninitializedTag;
    };

    struct Bit
    {
        int a, b, c, d;
    };

    using TUnion = TagUnion<u8, u16, u32, float, Bit>;

    using Tag = TUnion::TagType;

    SSSENGINE_STATIC_ASSERT(alignof(TUnion) == 4, "");
    SSSENGINE_STATIC_ASSERT(sizeof(TUnion) == 20, "");

} // namespace SSSEngine
