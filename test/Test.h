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

#include "Formatter.h"
#include "HelperMacros.h"
#include "Logger.h"
#include "StandardFileStream.h"
#include "StringView.h"
#include "Types.h"

namespace SSSTest
{
    // TODO: Meaningful messages

    struct TestData
    {
        using String = SSSEngine::Text::Utf8View;
        using function = void();

        static inline TestData *head = nullptr;

        int line;
        String file;
        function *test;
        TestData *next; // NOLINT(modernize-use-default-member-init)

        TestData(int line, const char8 *file, function *test) : line{line}, file{file}, test{test}, next{head}
        {
            head = this;
        }

        void operator()() const
        {
            test();
        }
    };

    SSSENGINE_GLOBAL
    bool Succeeded = true;

    SSSENGINE_GLOBAL
    void Execute()
    {
        for(auto *test = TestData::head; test; test = test->next)
        {
            try
            {
                // TODO: Info about the test about to start
                (*test)();
            }
            catch(...)
            {
                SSSENGINE_LOG_ERROR("Exception found while testing in file: {} line: {}.\n", test->file, test->line);

                Succeeded = false;
            }
        }
    }

    SSSENGINE_GLOBAL
    void ReportExpectFailure(int line, SSSEngine::Text::Utf8View file, SSSEngine::Text::Utf8View expected)
    {
        SSSENGINE_LOG_ERROR("Failed at {}:{}\n", file, line);
        SSSENGINE_LOG_ERROR("\tExpected {}\n", expected);
        /*SSSENGINE_LOG_ERROR("\tGot {} {} {}\n", first, #comparison, second);*/
    }

#define SSSTEST_TEST(name)                                                                                             \
    void name();                                                                                                       \
    TestData _##name({__LINE__, SSSENGINE_UTF8_FILE, name});                                                           \
    void name()

#define SSSTEST_COMPARE_(first, second, comparison)                                                                    \
    if((first)comparison(second))                                                                                      \
    {                                                                                                                  \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        ReportExpectFailure(SSSENGINE_LINE, SSSENGINE_UTF8_FILE, #first #comparison #second);                          \
        Succeeded = false;                                                                                             \
    }

#define SSSTEST_EXPECT_EQ(first, second) SSSTEST_COMPARE_(first, second, ==)
#define SSSTEST_EXPECT_NEQ(first, second) SSSTEST_COMPARE_(first, second, !=)
#define SSSTEST_EXPECT_GT(first, second) SSSTEST_COMPARE_(first, second, >)
#define SSSTEST_EXPECT_GE(first, second) SSSTEST_COMPARE_(first, second, >=)
#define SSSTEST_EXPECT_LE(first, second) SSSTEST_COMPARE_(first, second, <=)
#define SSSTEST_EXPECT_LT(first, second) SSSTEST_COMPARE_(first, second, <)
#define SSSTEST_EXPECT(value)                                                                                          \
    if((value))                                                                                                        \
    {                                                                                                                  \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        ReportExpectFailure(SSSENGINE_LINE, SSSENGINE_UTF8_FILE, #value);                                              \
        Succeeded = false;                                                                                             \
    }

#define SSSTEST_ASSERT_(first, second, comparison)                                                                     \
    if((first)comparison(second))                                                                                      \
    {                                                                                                                  \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        SSSENGINE_LOG_ERROR("Assertion failed at {}:{}\n", SSSENGINE_UTF8_FILE, __LINE__);                             \
        SSSENGINE_LOG_ERROR("\tExpected {} {} {}\n", #first, #comparison, #second);                                    \
        /*SSSENGINE_LOG_ERROR("\tGot {} {} {}\n", first, #comparison, second);*/                                       \
        Succeeded = false;                                                                                             \
        throw;                                                                                                         \
    }

#define SSSTEST_ASSERT_EQ(first, second) SSSTEST_ASSERT_(first, second, ==)
#define SSSTEST_ASSERT_NEQ(first, second) SSSTEST_ASSERT_(first, second, !=)
#define SSSTEST_ASSERT_GT(first, second) SSSTEST_ASSERT_(first, second, >)
#define SSSTEST_ASSERT_GE(first, second) SSSTEST_ASSERT_(first, second, >=)
#define SSSTEST_ASSERT_LE(first, second) SSSTEST_ASSERT_(first, second, <=)
#define SSSTEST_ASSERT_LT(first, second) SSSTEST_ASSERT_(first, second, <)
    // #define SSSTEST_ASSERT(value) SSSTEST_ASSERT_(value)

} // namespace SSSTest
