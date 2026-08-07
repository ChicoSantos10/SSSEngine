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

#include "AlignedStorage.h"
#include "Test.h"

#if 0

namespace SSSTest
{
    using namespace SSSEngine;
    using AlignedStorage = AlignedStorage<4, 4>;

    SSSTEST_TEST(DefaultConstruct)
    {
        AlignedStorage a;
        a.Construct<int>();

        SSSTEST_EXPECT_EQ(a.Get<int>(), 0);
    }

    SSSTEST_TEST(CopyConstruct)
    {
        AlignedStorage a;
        int b = 10;
        a.Construct<int>(b);

        SSSTEST_EXPECT_EQ(a.Get<int>(), 10);
    }

    SSSTEST_TEST(MoveConstruct)
    {
        AlignedStorage a;
        a.Construct<int>(10);

        SSSTEST_EXPECT_EQ(a.Get<int>(), 10);
    }
} // namespace SSSTest

#endif
