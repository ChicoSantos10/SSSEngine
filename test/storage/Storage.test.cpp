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

    struct TestNonTrivial
    {
        // NOLINTBEGIN(modernize-use-equals-default)
        explicit TestNonTrivial(int value) :
            value(value) {

            };

        TestNonTrivial(const TestNonTrivial &test) :
            value(test.value) {

            };

        TestNonTrivial(TestNonTrivial &&test) noexcept : value(Move(test.value)) {};

        TestNonTrivial &operator=(const TestNonTrivial &test) noexcept
        {
            value = test.value;
            return *this;
        };

        TestNonTrivial &operator=(TestNonTrivial &&test) noexcept
        {
            value = Move(test.value);
            return *this;
        };

        ~TestNonTrivial()
        {
            value = 0;
        };

        int value{0};

        // NOLINTEND(modernize-use-equals-default)
    };

    using NonTrivialStorage = Storage<TestNonTrivial>;
    using Storage = Storage<int>;

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

    SSSTEST_TEST(CopyTAssignment)
    {
        int value = 5;
        Storage s;
        s = value;

        SSSTEST_EXPECT_EQ(s, 5);
    }

    SSSTEST_TEST(MoveTAssignment)
    {
        Storage s;
        s = 10;

        SSSTEST_EXPECT_EQ(s, 10);
    }

    SSSTEST_TEST(CopyConstructor)
    {
        NonTrivialStorage s(TestNonTrivial(10));
        NonTrivialStorage t(s);

        SSSTEST_EXPECT_EQ(t.Get().value, 10);
    }

    SSSTEST_TEST(MoveConstructor)
    {
        NonTrivialStorage s(NonTrivialStorage(TestNonTrivial(10)));

        SSSTEST_EXPECT_EQ(s.Get().value, 10);
    }

    SSSTEST_TEST(CopyAssignment)
    {
        NonTrivialStorage s(NonTrivialStorage(TestNonTrivial(10)));
        NonTrivialStorage t;
        t = s;

        SSSTEST_EXPECT_EQ(t.Get().value, 10);
    }

    SSSTEST_TEST(MoveAssignment)
    {
        NonTrivialStorage t;
        t = NonTrivialStorage(NonTrivialStorage(TestNonTrivial(10)));

        SSSTEST_EXPECT_EQ(t.Get().value, 10);
    }

    SSSTEST_TEST(Get)
    {
        Storage s(10);
        int v = s.Get();

        SSSTEST_EXPECT_EQ(s, 10);
        SSSTEST_EXPECT_EQ(v, 10);
    }

    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, int>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, int &>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, int &&>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowConstructible<Storage, Storage>);
    SSSENGINE_STATIC_ASSERT(IsNoThrowCopyConstructible<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowMoveConstructible<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowMoveConstructible<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowMoveAssignable<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowCopyConstructible<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowCopyAssignable<Storage>, "");
    SSSENGINE_STATIC_ASSERT(IsNoThrowCopyAssignable<Storage>, "");
} // namespace SSSTest
