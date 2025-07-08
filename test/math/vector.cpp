/*  SSS Engine
    Copyright (C) 2024  Francisco Santos

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// TODO: Add include folders
#include "../test.h"
#include "../../src/Math/include/Vector.h"

using namespace SSSMath;

namespace SSSTest
{

    SSS_TEST(VectorEq)
    {
        { // Vector2
            Int2 v1{1, 2};
            Int2 v2{1, 2};
            Int2 v3{5, 7};

            SSS_EXPECT_EQ(v1, v2);
            SSS_EXPECT_NEQ(v1, v3);
        }

        { // Vector3
            Int3 v1{1, 2, 3};
            Int3 v2{1, 2, 3};
            Int3 v3{5, 7, 6};

            SSS_EXPECT_EQ(v1, v2);
            SSS_EXPECT_NEQ(v1, v3);
        }

        { // Vector4
            Int4 v1{1, 2, 4, 8};
            Int4 v2{1, 2, 4, 8};
            Int4 v3{5, 7, 2, 8};

            SSS_EXPECT_EQ(v1, v2);
            SSS_EXPECT_NEQ(v1, v3);
        }
    }

    SSS_TEST(VectorAdd)
    {
        { // V2
            Int2 v1{1, 2};
            Int2 v2{4, 5};
            Int2 expected{5, 7};

            SSS_EXPECT_EQ(v1 + v2, expected);
        }
        { // V3
            Int3 v1{1, 2, 2};
            Int3 v2{4, 5, 4};
            Int3 expected{5, 7, 6};

            SSS_EXPECT_EQ(v1 + v2, expected);
        }
        { // V4
            Int4 v1{1, 2, 2, 9};
            Int4 v2{4, 5, 9, 1};
            Int4 expected{5, 7, 11, 10};

            SSS_EXPECT_EQ(v1 + v2, expected);
        }
    }

    SSS_TEST(VectorSubract)
    {
        { // V2
            Int2 v1{1, 2};
            Int2 v2{4, 5};
            Int2 expected{3, 3};

            SSS_EXPECT_EQ(v2 - v1, expected);
        }
        { // V3
            Int3 v1{1, 2, 1};
            Int3 v2{4, 5, 8};
            Int3 expected{3, 3, 7};

            SSS_EXPECT_EQ(v2 - v1, expected);
        }
        { // V4
            Int4 v1{1, 2, 4, 5};
            Int4 v2{4, 5, 4, 4};
            Int4 expected{3, 3, 0, -1};

            SSS_EXPECT_EQ(v2 - v1, expected);
        }
    }

    SSS_TEST(VectorMult)
    {
        { // V2
            Int2 v1{1, 2};
            Int2 v2{4, 5};
            Int2 expected{4, 10};

            SSS_EXPECT_EQ(v1 * v2, expected);
        }
        { // V3
            Int3 v1{1, 2, 1};
            Int3 v2{4, 5, 8};
            Int3 expected{4, 10, 8};

            SSS_EXPECT_EQ(v1 * v2, expected);
        }
        { // V4
            Int4 v1{1, 2, 4, 5};
            Int4 v2{4, 5, 4, 4};
            Int4 expected{4, 10, 16, 20};

            SSS_EXPECT_EQ(v1 * v2, expected);
        }
    }

    SSS_TEST(VectorDiv)
    {
        { // V2
            Float2 v1{1, 2};
            Float2 v2{4, 5};
            Float2 expected{0.25f, 0.4f};

            SSS_EXPECT_EQ(v1 / v2, expected);
        }
        { // V3
            Float3 v1{1, 2, 1};
            Float3 v2{4, 5, 8};
            Float3 expected{0.25f, 0.4f, 0.125f};

            SSS_EXPECT_EQ(v1 / v2, expected);
        }
        { // V4
            Float4 v1{1, 2, 4, 5};
            Float4 v2{4, 5, 4, 4};
            Float4 expected{0.25f, 0.4f, 1, 1.25f};

            SSS_EXPECT_EQ(v1 / v2, expected);
        }
    }

    SSS_TEST(VectorScalar)
    {
        { // V2
            Int2 v1{1, 2};
            int scalar{2};
            Int2 expected{2, 4};

            SSS_EXPECT_EQ(v1 * scalar, expected);
        }
        { // V3
            Int3 v1{1, 2, 1};
            int scalar{3};
            Int3 expected{3, 6, 3};

            SSS_EXPECT_EQ(v1 * scalar, expected);
        }
        { // V4
            Int4 v1{1, 2, 4, 5};
            int scalar{2};
            Int4 expected{2, 4, 8, 10};

            SSS_EXPECT_EQ(v1 * scalar, expected);
        }
    }
} // namespace SSSTest
