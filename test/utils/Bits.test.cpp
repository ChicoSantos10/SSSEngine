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

#pragma once

#include "Test.h"
#include "Bits.h"

namespace SSSTest
{
    using namespace SSSEngine;

    enum class BitTest : u8
    {
        A = 0x01,
        B = 0x02,
        C = 0x04
    };

    SSSTEST_TEST(TestIntegralJoin)
    {
        SSSTEST_EXPECT_EQ(Join(1, 2), 3);
        SSSTEST_EXPECT_EQ(Join(1, 2, 4, 8), 15);
        SSSTEST_EXPECT_EQ(Join(3, 2), 3);
        SSSTEST_EXPECT_EQ(Join(1, 1, 1), 1);
    }

    SSSTEST_TEST(TestEnumJoin)
    {
        using enum BitTest;

        SSSTEST_EXPECT_EQ(Join(A, B), static_cast<BitTest>(3));
        SSSTEST_EXPECT_EQ(Join(A, B, C), static_cast<BitTest>(7));
        SSSTEST_EXPECT_EQ(Join(C, static_cast<BitTest>(5)), static_cast<BitTest>(5));
        SSSTEST_EXPECT_EQ(Join(A, A, A), A);
    }

    SSSTEST_TEST(TestIntegralHasBitSet)
    {
        SSSTEST_EXPECT(HasBitSet(3, 1));
        SSSTEST_EXPECT(HasBitSet(3, 2));
        SSSTEST_EXPECT(!HasBitSet(3, 4));
        SSSTEST_EXPECT(HasBitSet(15, 1));
        SSSTEST_EXPECT(HasBitSet(15, 2));
        SSSTEST_EXPECT(HasBitSet(15, 4));
        SSSTEST_EXPECT(HasBitSet(15, 8));
    }

    SSSTEST_TEST(TestEnumHasBitSet)
    {
        using enum BitTest;
        const BitTest abc = Join(A, B, C);
        SSSTEST_EXPECT(HasBitSet(abc, A));
        SSSTEST_EXPECT(HasBitSet(abc, B));
        SSSTEST_EXPECT(HasBitSet(abc, C));
    }

    SSSTEST_TEST(TestIntegralWithoutBits)
    {
        SSSTEST_EXPECT_EQ(WithoutBits(3, 1), 2);
        SSSTEST_EXPECT_EQ(WithoutBits(15, 8, 2), 5);
        SSSTEST_EXPECT_EQ(WithoutBits(4, 3, 2, 1), 4);
    }

    SSSTEST_TEST(TestEnumWithoutBits)
    {
        using enum BitTest;
        const BitTest abc = Join(A, B, C);
        SSSTEST_EXPECT_EQ(WithoutBits(abc, B), Join(A, C));
        SSSTEST_EXPECT_EQ(WithoutBits(abc, A, C), B);
        SSSTEST_EXPECT_EQ(WithoutBits(abc, C, C, C), Join(A, B));
    }
} // namespace SSSTest
