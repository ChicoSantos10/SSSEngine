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
#include "Types.h"
#include "String.h"
#include "Utility.h"
#include "DynamicArray.h"

namespace SSSTest
{
    // INVESTIGATE: Logging system or just keep std? Use vector or custom container when created?
    // TODO: Meaningful messages
    // TODO: Test Allocator

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
                (*test)();
            }
            catch(...)
            {
                // TODO: Test details (name, file, line)
                SSSENGINE_LOG_ERROR("Exception found while testing in file: {} line: {}.\n", test->file, test->line);

                Succeeded = false;
            }
        }
    }

#define SSSTEST_TEST(name)                                                                                             \
    void name();                                                                                                       \
    TestData _##name({__LINE__, SSSENGINE_UTF8_FILE, name});                                                           \
    void name()

#define SSSTEST_EXPECT_(value)                                                                                         \
    if((value))                                                                                                        \
    {                                                                                                                  \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        SSSENGINE_LOG_ERROR("Failed at {}:{}\n", SSSENGINE_UTF8_FILE, __LINE__);                                       \
        SSSENGINE_LOG_ERROR("{}\n", SSSENGINE_STRING(value));                                                          \
        Succeeded = false;                                                                                             \
    }

#define SSSTEST_COMPARE_(first, second, comparison) SSSTEST_EXPECT_((first)comparison(second))

#define SSSTEST_EXPECT_EQ(first, second) SSSTEST_COMPARE_(first, second, ==)
#define SSSTEST_EXPECT_NEQ(first, second) SSSTEST_COMPARE_(first, second, !=)
#define SSSTEST_EXPECT_GT(first, second) SSSTEST_COMPARE_(first, second, >)
#define SSSTEST_EXPECT_GE(first, second) SSSTEST_COMPARE_(first, second, >=)
#define SSSTEST_EXPECT_LE(first, second) SSSTEST_COMPARE_(first, second, <=)
#define SSSTEST_EXPECT_LT(first, second) SSSTEST_COMPARE_(first, second, <)
#define SSSTEST_EXPECT(value) SSSTEST_EXPECT_(value)

    // TODO: Implement assert
    //
} // namespace SSSTest
