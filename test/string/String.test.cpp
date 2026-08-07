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

#include "String.h"
#include "ArrayTraits.h"
#include "MemoryUtility.h"
#include "Test.h"
#include "Types.h"
#include "Utility.h"

namespace SSSTest
{
    using namespace SSSEngine;
    using namespace SSSEngine::Text;

    SSSTEST_TEST(TestDefaultConstructor)
    {
        Utf8 test;
        SSSTEST_EXPECT(MemoryCompare(test.CString(), u8"", 1) == 0);
        SSSTEST_EXPECT_EQ(test.Count(), 0);
    }

    SSSTEST_TEST(TestCStringConstructor)
    {
        Utf8 test(u8"Hello");
        SSSTEST_EXPECT((MemoryCompare(test.CString(), u8"Hello", 6) == 0));
        SSSTEST_EXPECT_EQ(test.Count(), 5);

        constexpr const char8 String[] = u8"This is a big text that needs to be allocated on the heap";
        constexpr auto Count = CountOf<decltype(String)>;

        Utf8 test2(String);
        SSSTEST_EXPECT((MemoryCompare(test2.CString(), String, Count) == 0));
        SSSTEST_EXPECT_EQ(test2.Count(), Count - 1);
    }

    SSSTEST_TEST(TestCopyConstructor)
    {
        Utf8 test(u8"Hello");
        Utf8 testCopy(test); // NOLINT(performance-*)
        SSSTEST_EXPECT((MemoryCompare(test.CString(), testCopy.CString(), test.Count()) == 0));
        SSSTEST_EXPECT_EQ(test.Count(), testCopy.Count())

        Utf8 test2(u8"This is a big text that needs to be allocated on the heap");
        Utf8 testCopy2(test2);
        SSSTEST_EXPECT((MemoryCompare(test2.CString(), testCopy2.CString(), test2.Count()) == 0));
        SSSTEST_EXPECT_EQ(test2.Count(), testCopy2.Count())
        SSSTEST_EXPECT_EQ(test2.Count(), 57);

        char8 *mutString = test2.CString();
        mutString[0] = u8'H';
        SSSTEST_EXPECT((MemoryCompare(test2.CString(), testCopy2.CString(), test2.Count()) != 0));
    }

    SSSTEST_TEST(TestMoveConstructor)
    {
        Utf8 test(u8"Hello");
        Utf8 testCopy(Move(test));
        SSSTEST_EXPECT(MemoryCompare(&test, 0, sizeof(test)) == 0);
        SSSTEST_EXPECT(MemoryCompare(testCopy.CString(), u8"Hello", 6) == 0);
        SSSTEST_EXPECT_EQ(testCopy.Count(), 5);

        constexpr const char8 String[] = u8"This is a big text that needs to be allocated on the heap";
        constexpr auto Count = CountOf<decltype(String)>;
        Utf8 test2(String);
        Utf8 testCopy2(Move(test2));
        SSSTEST_EXPECT(MemoryCompare(&test2, 0, sizeof(test)) == 0);
        SSSTEST_EXPECT(MemoryCompare(testCopy2.CString(), String, Count) == 0);
        SSSTEST_EXPECT_EQ(testCopy2.Count(), Count - 1);
    }

    SSSTEST_TEST(TestCopyAssignment)
    {
        Utf8 test(u8"Hello");
        Utf8 testCopy;
        testCopy = test; // NOLINT(performance-*)
        SSSTEST_EXPECT((MemoryCompare(test.CString(), testCopy.CString(), test.Count()) == 0));
        SSSTEST_EXPECT_EQ(test.Count(), testCopy.Count())

        Utf8 test2(u8"This is a big text that needs to be allocated on the heap");
        testCopy = test2;
        SSSTEST_EXPECT((MemoryCompare(test2.CString(), testCopy.CString(), test2.Count()) == 0));
        SSSTEST_EXPECT_EQ(test2.Count(), testCopy.Count())
    }

    SSSTEST_TEST(TestMoveAssignment)
    {
        Utf8 test(u8"Hello");
        Utf8 testCopy;
        testCopy = Move(test);
        SSSTEST_EXPECT(MemoryCompare(&test, 0, sizeof(test)) == 0);
        SSSTEST_EXPECT(MemoryCompare(testCopy.CString(), u8"Hello", 6) == 0);
        SSSTEST_EXPECT_EQ(testCopy.Count(), 5);

        constexpr const char8 String[] = u8"This is a big text that needs to be allocated on the heap";
        constexpr auto Count = CountOf<decltype(String)>;
        Utf8 test2(String);
        Utf8 testCopy2;
        testCopy2 = Move(test2);
        SSSTEST_EXPECT(MemoryCompare(&test2, 0, sizeof(test)) == 0);
        SSSTEST_EXPECT(MemoryCompare(testCopy2.CString(), String, Count) == 0);
        SSSTEST_EXPECT_EQ(testCopy2.Count(), Count - 1);
    }

    SSSTEST_TEST(SelfAssignment)
    {
        Utf8 test(u8"Hello");
        test = test;

        SSSTEST_EXPECT_EQ(MemoryCompare(test.CString(), u8"Hello", 6), 0);

        test = Move(test);

        SSSTEST_EXPECT_EQ(MemoryCompare(test.CString(), u8"Hello", 6), 0);
    }
} // namespace SSSTest
