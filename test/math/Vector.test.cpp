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

#include "Test.h"
#include "Vector.h"

using namespace SSSEngine::Math;

namespace SSSTest
{

    SSSTEST_TEST(VectorEq)
    {
        { // Vector2
            Int2 v1{1, 2};
            Int2 v2{1, 2};
            Int2 v3{5, 7};

            SSSTEST_EXPECT_EQ(v1, v2);
            SSSTEST_EXPECT_NEQ(v1, v3);
        }

        { // Vector3
            Int3 v1{1, 2, 3};
            Int3 v2{1, 2, 3};
            Int3 v3{5, 7, 6};

            SSSTEST_EXPECT_EQ(v1, v2);
            SSSTEST_EXPECT_NEQ(v1, v3);
        }

        { // Vector4
            Int4 v1{1, 2, 4, 8};
            Int4 v2{1, 2, 4, 8};
            Int4 v3{5, 7, 2, 8};

            SSSTEST_EXPECT_EQ(v1, v2);
            SSSTEST_EXPECT_NEQ(v1, v3);
        }
    }

    SSSTEST_TEST(VectorAdd)
    {
        { // V2
            Int2 v1{1, 2};
            Int2 v2{4, 5};
            Int2 expected{5, 7};

            SSSTEST_EXPECT_EQ(v1 + v2, expected);
        }
        { // V3
            Int3 v1{1, 2, 2};
            Int3 v2{4, 5, 4};
            Int3 expected{5, 7, 6};

            SSSTEST_EXPECT_EQ(v1 + v2, expected);
        }
        { // V4
            Int4 v1{1, 2, 2, 9};
            Int4 v2{4, 5, 9, 1};
            Int4 expected{5, 7, 11, 10};

            SSSTEST_EXPECT_EQ(v1 + v2, expected);
        }
    }

    SSSTEST_TEST(VectorSubract)
    {
        { // V2
            Int2 v1{1, 2};
            Int2 v2{4, 5};
            Int2 expected{3, 3};

            SSSTEST_EXPECT_EQ(v2 - v1, expected);
        }
        { // V3
            Int3 v1{1, 2, 1};
            Int3 v2{4, 5, 8};
            Int3 expected{3, 3, 7};

            SSSTEST_EXPECT_EQ(v2 - v1, expected);
        }
        { // V4
            Int4 v1{1, 2, 4, 5};
            Int4 v2{4, 5, 4, 4};
            Int4 expected{3, 3, 0, -1};

            SSSTEST_EXPECT_EQ(v2 - v1, expected);
        }
    }

    SSSTEST_TEST(VectorMult)
    {
        { // V2
            Int2 v1{1, 2};
            Int2 v2{4, 5};
            Int2 expected{4, 10};

            SSSTEST_EXPECT_EQ(v1 * v2, expected);
        }
        { // V3
            Int3 v1{1, 2, 1};
            Int3 v2{4, 5, 8};
            Int3 expected{4, 10, 8};

            SSSTEST_EXPECT_EQ(v1 * v2, expected);
        }
        { // V4
            Int4 v1{1, 2, 4, 5};
            Int4 v2{4, 5, 4, 4};
            Int4 expected{4, 10, 16, 20};

            SSSTEST_EXPECT_EQ(v1 * v2, expected);
        }
    }

    SSSTEST_TEST(VectorDiv)
    {
        { // V2
            Float2 v1{1, 2};
            Float2 v2{4, 5};
            Float2 expected{0.25f, 0.4f};

            SSSTEST_EXPECT_EQ(v1 / v2, expected);
        }
        { // V3
            Float3 v1{1, 2, 1};
            Float3 v2{4, 5, 8};
            Float3 expected{0.25f, 0.4f, 0.125f};

            SSSTEST_EXPECT_EQ(v1 / v2, expected);
        }
        { // V4
            Float4 v1{1, 2, 4, 5};
            Float4 v2{4, 5, 4, 4};
            Float4 expected{0.25f, 0.4f, 1, 1.25f};

            SSSTEST_EXPECT_EQ(v1 / v2, expected);
        }
    }

    SSSTEST_TEST(VectorScalar)
    {
        { // V2
            Int2 v1{1, 2};
            int scalar{2};
            Int2 expected{2, 4};

            SSSTEST_EXPECT_EQ(v1 * scalar, expected);
        }
        { // V3
            Int3 v1{1, 2, 1};
            int scalar{3};
            Int3 expected{3, 6, 3};

            SSSTEST_EXPECT_EQ(v1 * scalar, expected);
        }
        { // V4
            Int4 v1{1, 2, 4, 5};
            int scalar{2};
            Int4 expected{2, 4, 8, 10};

            SSSTEST_EXPECT_EQ(v1 * scalar, expected);
        }
    }
} // namespace SSSTest
