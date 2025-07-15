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

#include "../test.h"
#include "Matrix.h"

using namespace SSSMath;

namespace SSSTest
{
    SSS_TEST(MatrixMultiplication)
    {
        Mat4x4f m1{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        Mat4x4f m2{1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6};
        Mat4x4f result = m1 * m2;
        Mat4x4f expected = m2;

        SSS_EXPECT_EQ(result, expected);
    }

    SSS_TEST(MatrixSubscriptOperator)
    {
        Mat4x4f m{1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6};
        float f = m[2];
        float f1 = m[0, 3];
        float f2 = m[3, 2];

        SSS_EXPECT_EQ(f, 3);
        SSS_EXPECT_EQ(f1, 4);
        SSS_EXPECT_EQ(f2, 5);
    }

    SSS_TEST(MatrixIdentity)
    {
        {
            constexpr auto Identiy4x4 = IdentityMatrix<Mat4x4f>();
            constexpr Mat4x4f Expected = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

            SSS_EXPECT_EQ(Identiy4x4, Expected);
        }
        {
            using Mat3x3f = Matrix<float, 3, 3>;
            constexpr auto Identity3x3 = IdentityMatrix<Mat3x3f>();
            constexpr Mat3x3f Expected = {1, 0, 0, 0, 1, 0, 0, 0, 1};

            SSS_EXPECT_EQ(Identity3x3, Expected);
        }
        {
            using Mat2x2f = Matrix<float, 2, 2>;
            constexpr auto Identity2x2 = IdentityMatrix<Mat2x2f>();
            constexpr Mat2x2f Expected = {1, 0, 0, 1};

            SSS_EXPECT_EQ(Identity2x2, Expected);
        }
    }
} // namespace SSSTest
