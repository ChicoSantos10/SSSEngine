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

#include "Ratio.h"
#include "Test.h"
#include "Quantity.h"

namespace SSSTest
{
    using namespace SSSEngine::Math;
    using DummyQuantityType = Quantity<int, i64, Nano>;
    using DummyQuantityType2 = Quantity<int, i64, Micro>;

    SSSTEST_TEST(EqualityComparisons)
    {
        DummyQuantityType a{10};
        DummyQuantityType b{6};
        DummyQuantityType c{19};
        DummyQuantityType d{10};

        SSSTEST_EXPECT_EQ(a, d);
        SSSTEST_EXPECT_NEQ(a, b);
        SSSTEST_EXPECT_LT(a, c);
        SSSTEST_EXPECT_LE(a, d);
        SSSTEST_EXPECT_GT(a, b);
        SSSTEST_EXPECT_GE(a, d);
    }

    SSSTEST_TEST(OperatorUnaryPlus)
    {
        DummyQuantityType a{10};
        DummyQuantityType b = +a;

        SSSTEST_EXPECT_EQ(a, b);
    }

    SSSTEST_TEST(OperatorUnaryMinus)
    {
        DummyQuantityType a{10};
        DummyQuantityType b = -a;

        SSSTEST_EXPECT_EQ(b.value, -10);
    }

    SSSTEST_TEST(OperatorPreIncrement)
    {
        DummyQuantityType a{10};
        DummyQuantityType b = ++a;

        SSSTEST_EXPECT_EQ(a.value, 11);
        SSSTEST_EXPECT_EQ(b, a);
    }

    SSSTEST_TEST(OperatorPostIncrement)
    {
        DummyQuantityType a{10};
        DummyQuantityType b = a++;

        SSSTEST_EXPECT_EQ(a.value, 11);
        SSSTEST_EXPECT_NEQ(b, a);
    }

    SSSTEST_TEST(OperatorPreDecrement)
    {
        DummyQuantityType a{10};
        DummyQuantityType b = --a;

        SSSTEST_EXPECT_EQ(a.value, 9);
        SSSTEST_EXPECT_EQ(b, a);
    }

    SSSTEST_TEST(OperatorPostDecrement)
    {
        DummyQuantityType a{10};
        DummyQuantityType b = a--;

        SSSTEST_EXPECT_EQ(a.value, 9);
        SSSTEST_EXPECT_NEQ(b, a);
    }

    SSSTEST_TEST(OperatorPlusAssignment)
    {
        DummyQuantityType a{10};
        DummyQuantityType b{10};
        a += b;

        SSSTEST_EXPECT_EQ(a.value, 20);
    }

    SSSTEST_TEST(OperatorMinusAssignment)
    {
        DummyQuantityType a{10};
        DummyQuantityType b{6};
        a -= b;

        SSSTEST_EXPECT_EQ(a.value, 4);
    }

    SSSTEST_TEST(OperatorMultiplyAssignment)
    {
        DummyQuantityType a{10};
        a *= 6;

        SSSTEST_EXPECT_EQ(a.value, 60);
    }

    SSSTEST_TEST(OperatorDivideAssignment)
    {
        DummyQuantityType a{10};
        a /= 2;

        SSSTEST_EXPECT_EQ(a.value, 5);
    }

    SSSTEST_TEST(OperatorModuloAssignment)
    {
        DummyQuantityType a{10};
        a %= 6;

        SSSTEST_EXPECT_EQ(a.value, 4);
    }

    SSSTEST_TEST(MaxMinValues)
    {
        DummyQuantityType a = DummyQuantityType::Max();
        DummyQuantityType b = DummyQuantityType::Min();

        SSSTEST_EXPECT_EQ(a.value, SSSEngine::NumericTraitsType<DummyQuantityType::ValueType>::Max);
        SSSTEST_EXPECT_EQ(b.value, SSSEngine::NumericTraitsType<DummyQuantityType::ValueType>::Min);
    }

    SSSTEST_TEST(OperatorPlus)
    {
        DummyQuantityType a{10};
        DummyQuantityType b{5};

        SSSTEST_EXPECT_EQ(a + b, DummyQuantityType{15});
    }

    SSSTEST_TEST(OperatorMinus)
    {
        DummyQuantityType a{10};
        DummyQuantityType b{5};

        SSSTEST_EXPECT_EQ(a - b, DummyQuantityType{5});
    }

    SSSTEST_TEST(OperatorMultiply)
    {
        DummyQuantityType a{10};
        auto b = 2;

        DummyQuantityType result{20};
        SSSTEST_EXPECT_EQ(a * b, result);
        SSSTEST_EXPECT_EQ(b * a, result);
    }

    SSSTEST_TEST(OperatorDivide)
    {
        DummyQuantityType a{10};
        DummyQuantityType b{5};

        SSSTEST_EXPECT_EQ(a / 2, DummyQuantityType{5});
        SSSTEST_EXPECT_EQ(b / a, 2)
    }

    SSSTEST_TEST(OperatorModulo)
    {
        DummyQuantityType a{10};

        SSSTEST_EXPECT_EQ(a % 8, DummyQuantityType{2});
    }

    SSSTEST_TEST(Conversion)
    {
        DummyQuantityType2 a{10};
        DummyQuantityType b{a};

        SSSTEST_EXPECT_EQ(b, DummyQuantityType{10000});
    }
} // namespace SSSTest
