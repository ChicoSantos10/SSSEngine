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

#include "Formatter.h"
#include "String.h"
#include "Test.h"
#include "Vector.h"

using namespace SSSEngine::Math;

namespace SSSTest
{

    SSSTEST_TEST(VectorEq)
    {
        { // Vector2
            Int2 v1{.x = 1, .y = 2};
            Int2 v2{.x = 1, .y = 2};
            Int2 v3{.x = 5, .y = 7};

            SSSTEST_EXPECT_EQ(v1, v2);
            SSSTEST_EXPECT_NEQ(v1, v3);
        }

        { // Vector3
            Int3 v1{.x = 1, .y = 2, .z = 3};
            Int3 v2{.x = 1, .y = 2, .z = 3};
            Int3 v3{.x = 5, .y = 7, .z = 6};

            SSSTEST_EXPECT_EQ(v1, v2);
            SSSTEST_EXPECT_NEQ(v1, v3);
        }

        { // Vector4
            Int4 v1{.x = 1, .y = 2, .z = 4, .w = 8};
            Int4 v2{.x = 1, .y = 2, .z = 4, .w = 8};
            Int4 v3{.x = 5, .y = 7, .z = 2, .w = 8};

            SSSTEST_EXPECT_EQ(v1, v2);
            SSSTEST_EXPECT_NEQ(v1, v3);
        }
    }

    SSSTEST_TEST(VectorAdd)
    {
        { // V2
            Int2 v1{.x = 1, .y = 2};
            Int2 v2{.x = 4, .y = 5};
            Int2 expected{.x = 5, .y = 7};

            SSSTEST_EXPECT_EQ(v1 + v2, expected);
        }
        { // V3
            Int3 v1{.x = 1, .y = 2, .z = 2};
            Int3 v2{.x = 4, .y = 5, .z = 4};
            Int3 expected{.x = 5, .y = 7, .z = 6};

            SSSTEST_EXPECT_EQ(v1 + v2, expected);
        }
        { // V4
            Int4 v1{.x = 1, .y = 2, .z = 2, .w = 9};
            Int4 v2{.x = 4, .y = 5, .z = 9, .w = 1};
            Int4 expected{.x = 5, .y = 7, .z = 11, .w = 10};

            SSSTEST_EXPECT_EQ(v1 + v2, expected);
        }
    }

    SSSTEST_TEST(VectorSubract)
    {
        { // V2
            Int2 v1{.x = 1, .y = 2};
            Int2 v2{.x = 4, .y = 5};
            Int2 expected{.x = 3, .y = 3};

            SSSTEST_EXPECT_EQ(v2 - v1, expected);
        }
        { // V3
            Int3 v1{.x = 1, .y = 2, .z = 1};
            Int3 v2{.x = 4, .y = 5, .z = 8};
            Int3 expected{.x = 3, .y = 3, .z = 7};

            SSSTEST_EXPECT_EQ(v2 - v1, expected);
        }
        { // V4
            Int4 v1{.x = 1, .y = 2, .z = 4, .w = 5};
            Int4 v2{.x = 4, .y = 5, .z = 4, .w = 4};
            Int4 expected{.x = 3, .y = 3, .z = 0, .w = -1};

            SSSTEST_EXPECT_EQ(v2 - v1, expected);
        }
    }

    SSSTEST_TEST(VectorMult)
    {
        { // V2
            Int2 v1{.x = 1, .y = 2};
            Int2 v2{.x = 4, .y = 5};
            Int2 expected{.x = 4, .y = 10};

            SSSTEST_EXPECT_EQ(v1 * v2, expected);
        }
        { // V3
            Int3 v1{.x = 1, .y = 2, .z = 1};
            Int3 v2{.x = 4, .y = 5, .z = 8};
            Int3 expected{.x = 4, .y = 10, .z = 8};

            SSSTEST_EXPECT_EQ(v1 * v2, expected);
        }
        { // V4
            Int4 v1{.x = 1, .y = 2, .z = 4, .w = 5};
            Int4 v2{.x = 4, .y = 5, .z = 4, .w = 4};
            Int4 expected{.x = 4, .y = 10, .z = 16, .w = 20};

            SSSTEST_EXPECT_EQ(v1 * v2, expected);
        }
    }

    SSSTEST_TEST(VectorDiv)
    {
        { // V2
            Float2 v1{.x = 1, .y = 2};
            Float2 v2{.x = 4, .y = 5};
            Float2 expected{.x = 0.25f, .y = 0.4f};

            SSSTEST_EXPECT_EQ(v1 / v2, expected);
        }
        { // V3
            Float3 v1{.x = 1, .y = 2, .z = 1};
            Float3 v2{.x = 4, .y = 5, .z = 8};
            Float3 expected{.x = 0.25f, .y = 0.4f, .z = 0.125f};

            SSSTEST_EXPECT_EQ(v1 / v2, expected);
        }
        { // V4
            Float4 v1{.x = 1, .y = 2, .z = 4, .w = 5};
            Float4 v2{.x = 4, .y = 5, .z = 4, .w = 4};
            Float4 expected{.x = 0.25f, .y = 0.4f, .z = 1, .w = 1.25f};

            SSSTEST_EXPECT_EQ(v1 / v2, expected);
        }
    }

    SSSTEST_TEST(VectorScalar)
    {
        { // V2
            Int2 v1{.x = 1, .y = 2};
            int scalar{2};
            Int2 expected{.x = 2, .y = 4};

            SSSTEST_EXPECT_EQ(v1 * scalar, expected);
        }
        { // V3
            Int3 v1{.x = 1, .y = 2, .z = 1};
            int scalar{3};
            Int3 expected{.x = 3, .y = 6, .z = 3};

            SSSTEST_EXPECT_EQ(v1 * scalar, expected);
        }
        { // V4
            Int4 v1{.x = 1, .y = 2, .z = 4, .w = 5};
            int scalar{2};
            Int4 expected{.x = 2, .y = 4, .z = 8, .w = 10};

            SSSTEST_EXPECT_EQ(v1 * scalar, expected);
        }
    }

    SSSTEST_TEST(VectorFormatting)
    {
        { // V2
            Int2 v1{.x = 1, .y = 2};
            auto result = SSSEngine::Text::Format<SSSEngine::Text::AsciiEncoding>("{}", v1);
            SSSEngine::Text::AsciiView expected = "[ 1 2 ]";

            SSSTEST_EXPECT_EQ(result, expected);
        }
        { // V3
            Int3 v1{.x = 1, .y = 2, .z = 1};
            auto result = SSSEngine::Text::Format<SSSEngine::Text::AsciiEncoding>("{}", v1);
            SSSEngine::Text::AsciiView expected = "[ 1 2 1 ]";

            SSSTEST_EXPECT_EQ(result, expected);
        }
        { // V4
            Int4 v1{.x = 1, .y = 2, .z = 4, .w = 5};
            auto result = SSSEngine::Text::Format<SSSEngine::Text::AsciiEncoding>("{}", v1);
            SSSEngine::Text::AsciiView expected = "[ 1 2 4 5 ]";

            SSSTEST_EXPECT_EQ(result, expected);
        }
    }
} // namespace SSSTest
