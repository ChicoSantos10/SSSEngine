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

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include "HelperMacros.h"
#include "Types.h"

namespace SSSTest
{
    // INVESTIGATE: Logging system or just keep std? Use vector or custom container when created?
    // TODO: Meaningful messages

    struct TestData
    {
        using function = void();
        int line;
        std::string file;
        function *test;

        TestData(int line, std::string file, function *test) : line{line}, file{std::move(file)}, test{test} {}

        void operator()() const
        {
            test();
        }
    };

    SSSENGINE_GLOBAL std::vector<TestData> Tests{};
    SSSENGINE_GLOBAL bool Succeeded = true;

    class Test
    {
        public:
        explicit Test(const TestData &data)
        {
            Add(data);
        }

        static void Add(const TestData &data)
        {
            Tests.push_back(data);
        }

        static void Execute()
        {
            for(auto const &test: Tests)
            {
                try
                {
                    test();
                }
                catch(...)
                {
                    // TODO: Test details (name, file, line)
                    std::cerr << "Exception found while testing in file: " << test.file << " line: " << test.line << "\n";

                    Succeeded = false;
                }
            }
        }
    };

#define SSSTEST_TEST(name)                                                                                             \
    void name();                                                                                                       \
    Test _##name({__LINE__, __FILE__, name});                                                                          \
    void name()

#define SSSTEST_EXPECT(first, second, comparison)                                                                      \
    if((first)comparison(second))                                                                                      \
    {                                                                                                                  \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        std::cerr << "Failed at " << __FILE__ << " " << __LINE__ << "\n";                                              \
        Succeeded = false;                                                                                             \
    }

#define SSSTEST_EXPECT_EQ(first, second) SSSTEST_EXPECT(first, second, ==)
#define SSSTEST_EXPECT_NEQ(first, second) SSSTEST_EXPECT(first, second, !=)
#define SSSTEST_EXPECT_GT(first, second) SSSTEST_EXPECT(first, second, >)
#define SSSTEST_EXPECT_GE(first, second) SSSTEST_EXPECT(first, second, >=)
#define SSSTEST_EXPECT_LE(first, second) SSSTEST_EXPECT(first, second, <=)
#define SSSTEST_EXPECT_LT(first, second) SSSTEST_EXPECT(first, second, <)

    // TODO: Implement assert
    //
} // namespace SSSTest
