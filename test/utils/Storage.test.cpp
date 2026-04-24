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

#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Test.h"

#include "Storage.h"

namespace SSSTest
{
    using namespace SSSEngine;
    using Storage = Storage<int>;

    SSSTEST_TEST(TConstructor)
    {
        Storage s(10);

        SSSTEST_EXPECT_EQ(s, 10);
    }

    SSSTEST_TEST(CopyConstructor)
    {
        Storage s(10);
        Storage t(s);

        SSSTEST_EXPECT_EQ(t, 10);
    }

    SSSTEST_TEST(MoveConstructor)
    {
        Storage s(Storage(10));

        SSSTEST_EXPECT_EQ(s, 10);
    }

    SSSTEST_TEST(CopyTConstructor)
    {
        int value = 10;
        Storage s(value);

        SSSTEST_EXPECT_EQ(s, 10);
    }

    SSSTEST_TEST(MoveTConstructor)
    {
        Storage s(10);

        SSSTEST_EXPECT_EQ(s, 10);
    }

    SSSTEST_TEST(CopyAssignment)
    {
        Storage s(10);
        Storage t;
        t = s;

        SSSTEST_EXPECT_EQ(t, 10);
    }

    SSSTEST_TEST(CopyTAssignment)
    {
        int value = 5;
        Storage s;
        s = value;

        SSSTEST_EXPECT_EQ(s, 5);
    }

    SSSTEST_TEST(MoveAssignment)
    {
        Storage t;
        t = Storage(10);

        SSSTEST_EXPECT_EQ(t, 10);
    }

    SSSTEST_TEST(MoveTAssignment)
    {
        Storage s;
        s = 10;

        SSSTEST_EXPECT_EQ(s, 10);
    }

    SSSTEST_TEST(StorageTest)
    {
        Storage s(10);
        Storage i = 15;

        SSSTEST_EXPECT_EQ(s + i, 25);
        SSSTEST_EXPECT_EQ(s - i, -5);
        SSSTEST_EXPECT_EQ(s * i, 150);
        SSSTEST_EXPECT_EQ(i / s, 1);

        s = 25;
        i = 2;

        SSSTEST_EXPECT_EQ(s + i, 27);
        SSSTEST_EXPECT_EQ(s - i, 23);
        SSSTEST_EXPECT_EQ(s * i, 50);
        SSSTEST_EXPECT_EQ(s / i, 12);
    }

    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, int>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, int &>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, int &&>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, Storage>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, Storage &>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, Storage &&>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowMoveConstructible<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowMoveAssignable<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowCopyConstructible<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowCopyAssignable<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowCopyAssignable<Storage>, "");
} // namespace SSSTest
