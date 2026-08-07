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
 * @brief Utilities to conditionally enable or disable special member functions
 */

#pragma once

namespace SSSEngine
{
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions)

    struct EnableDefaultConstructorTag
    {
        explicit constexpr EnableDefaultConstructorTag() = default;
    };

    template<bool Switch, typename Tag = void>
    struct EnableDefaultConstructor
    {
        constexpr EnableDefaultConstructor() noexcept = default;
        constexpr EnableDefaultConstructor(const EnableDefaultConstructor &) noexcept = default;
        constexpr EnableDefaultConstructor(EnableDefaultConstructor &&) noexcept = default;
        constexpr EnableDefaultConstructor &operator=(const EnableDefaultConstructor &) noexcept = default;
        constexpr EnableDefaultConstructor &operator=(EnableDefaultConstructor &&) noexcept = default;

        constexpr explicit EnableDefaultConstructor(EnableDefaultConstructorTag) {}
    };

    template<bool Switch, typename Tag = void>
    struct EnableDestructor
    {
    };

    template<bool CopyCtor, bool CopyAssignment, bool MoveCtor, bool MoveAssignment, typename Tag = void>
    struct EnableCopyMove
    {
    };

    template<bool Default, bool Destructor, bool Copy, bool CopyAssignment, bool Move, bool MoveAssignment, typename Tag = void>
    struct EnableSpecialMembers :
        private EnableDefaultConstructor<Default, Tag>,
        private EnableDestructor<Destructor, Tag>,
        private EnableCopyMove<Copy, CopyAssignment, Move, MoveAssignment, Tag>
    {
    };

    template<typename Tag>
    struct EnableDefaultConstructor<false, Tag>
    {
        constexpr EnableDefaultConstructor() noexcept = delete;
        constexpr EnableDefaultConstructor(const EnableDefaultConstructor &) noexcept = default;
        constexpr EnableDefaultConstructor(EnableDefaultConstructor &&) noexcept = default;
        constexpr EnableDefaultConstructor &operator=(const EnableDefaultConstructor &) noexcept = default;
        constexpr EnableDefaultConstructor &operator=(EnableDefaultConstructor &&) noexcept = default;

        constexpr explicit EnableDefaultConstructor(EnableDefaultConstructorTag) {}
    };

    template<typename Tag>
    struct EnableDestructor<false, Tag>
    {
        ~EnableDestructor() noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<false, false, false, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<false, false, false, true, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = default;
    };

    template<typename Tag>
    struct EnableCopyMove<false, false, true, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = default;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<false, false, true, true, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = default;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = default;
    };

    template<typename Tag>
    struct EnableCopyMove<false, true, false, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<false, true, false, true, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = default;
    };

    template<typename Tag>
    struct EnableCopyMove<false, true, true, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = default;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<false, true, true, true, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = default;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = default;
    };

    template<typename Tag>
    struct EnableCopyMove<true, false, false, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<true, false, false, true, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = default;
    };

    template<typename Tag>
    struct EnableCopyMove<true, false, true, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = default;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<true, false, true, true, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = default;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = delete;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = default;
    };

    template<typename Tag>
    struct EnableCopyMove<true, true, false, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    template<typename Tag>
    struct EnableCopyMove<true, true, false, true, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = delete;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = default;
    };

    template<typename Tag>
    struct EnableCopyMove<true, true, true, false, Tag>
    {
        constexpr EnableCopyMove() noexcept = default;
        constexpr EnableCopyMove(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove(EnableCopyMove &&) noexcept = default;
        constexpr EnableCopyMove &operator=(const EnableCopyMove &) noexcept = default;
        constexpr EnableCopyMove &operator=(EnableCopyMove &&) noexcept = delete;
    };

    // NOLINTEND(cppcoreguidelines-special-member-functions)
} // namespace SSSEngine
