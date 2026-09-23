#include "Logger.h"
#include "String.h"
#include "StringView.h"
#include "Test.h"
#include "Types.h"
#include "Utf8Encoding.h"
#include "Formatter.h"

namespace SSSTest
{
    using namespace SSSEngine::Text;

    // =================================================================================================================
    // Integrals
    // =================================================================================================================

    SSSTEST_TEST(FormatIntegrals)
    {
        {
            i8 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u8 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i16 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u16 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u32 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i64 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u64 v = 10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatNegativeIntegrals)
    {
        {
            i8 v = -10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i16 v = -10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = -10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i64 v = -10;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i64 v = SSSEngine::IntTraits<i64>::Min;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-9223372036854775808");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatIntSignNegative)
    {
        {
            i32 v = -200;
            Utf8 string = Format<Utf8Encoding>(u8"{:-}", v);
            Utf8 result(u8"-200");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = 200;
            Utf8 string = Format<Utf8Encoding>(u8"{:-}", v);
            Utf8 result(u8"200");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatIntSignAlways)
    {
        {
            i32 v = -200;
            Utf8 string = Format<Utf8Encoding>(u8"{:+}", v);
            Utf8 result(u8"-200");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = 200;
            Utf8 string = Format<Utf8Encoding>(u8"{:+}", v);
            Utf8 result(u8"+200");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatIntSignSpace)
    {
        {
            i32 v = -200;
            Utf8 string = Format<Utf8Encoding>(u8"{: }", v);
            Utf8 result(u8"-200");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = 200;
            Utf8 string = Format<Utf8Encoding>(u8"{: }", v);
            Utf8 result(u8" 200");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatIntBinary)
    {
        i32 v = 0b1111000011110000;
        Utf8 string = Format<Utf8Encoding>(u8"{:b}", v);
        Utf8 result(u8"1111000011110000");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatIntBinaryAlternateForm)
    {
        i32 v = 0b1111000011110000;
        Utf8 string = Format<Utf8Encoding>(u8"{:#b}", v);
        Utf8 result(u8"0b1111000011110000");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatIntHexForm)
    {
        i32 v = 0x0FABFFA0;
        Utf8 string = Format<Utf8Encoding>(u8"{:x}", v);
        Utf8 result(u8"FABFFA0");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatIntHexAlternateForm)
    {
        i32 v = 0x0FABFFA0;
        Utf8 string = Format<Utf8Encoding>(u8"{:#x}", v);
        Utf8 result(u8"0xFABFFA0");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatIntWidth)
    {
        i32 v = 1024;
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:8}", v);
            Utf8 result(u8"    1024");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            int w = 12;
            Utf8 string = Format<Utf8Encoding>(u8"{:{1}}", v, w);
            Utf8 result(u8"        1024");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatIntZeroPad)
    {
        {
            i32 v = 1024;
            Utf8 string = Format<Utf8Encoding>(u8"{:08}", v);
            Utf8 result(u8"00001024");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = 0x234AA;
            Utf8 string = Format<Utf8Encoding>(u8"{:#08x}", v);
            Utf8 result(u8"0x0234AA");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatIntFillAndAlignment)
    {
        i32 v = 1024;
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:<8}", v);
            Utf8 result(u8"1024    ");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:>8}", v);
            Utf8 result(u8"    1024");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:^8}", v);
            Utf8 result(u8"  1024  ");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:*<8}", v);
            Utf8 result(u8"1024****");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:*<#8x}", v);
            Utf8 result(u8"0x400***");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:*>#8x}", v);
            Utf8 result(u8"***0x400");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatIntAll)
    {
        i32 v = 1024;
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:<+8}", v);
            Utf8 result(u8"+1024   ");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:>+8}", -v);
            Utf8 result(u8"   -1024");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:*< 8}", v);
            Utf8 result(u8" 1024***");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:*< #8x}", v);
            Utf8 result(u8" 0x400**");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            Utf8 string = Format<Utf8Encoding>(u8"{:*>+#8x}", v);
            Utf8 result(u8"**+0x400");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    // =================================================================================================================
    // Floating Point
    // =================================================================================================================

    SSSTEST_TEST(FormatFloat32Zero)
    {
        f32 v = 0;
        Utf8 string = Format<Utf8Encoding>(u8"{}", v);
        Utf8 result(u8"0");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatFloat32)
    {
        {
            f32 v = 1.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"1");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 2.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"2");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 10.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 0.5f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"0.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 0.99025f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"0.99025");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 0.000125f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"0.000125");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 1.5f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"1.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 10.5f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = 10183.52f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10183.52");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatNegativeFloat32)
    {
        {
            f32 v = -10.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = -0.5f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-0.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = -0.99025f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-0.99025");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = -0.000125f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-0.000125");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = -1.5f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-1.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = -10.5f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = -10183.52f;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10183.52");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatFloat32Nan)
    {
        f32 v = SSSEngine::FloatTraits<f32>::NaN;
        Utf8 string = Format<Utf8Encoding>(u8"{}", v);
        Utf8 result(u8"NaN");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatFloat32Infinity)
    {
        {
            f32 v = SSSEngine::FloatTraits<f32>::PositiveInfinity;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"Infinity");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f32 v = SSSEngine::FloatTraits<f32>::NegativeInfinity;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-Infinity");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatFloat64Zero)
    {
        f64 v = 0;
        Utf8 string = Format<Utf8Encoding>(u8"{}", v);
        Utf8 result(u8"0");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatFloat64)
    {
        {
            f64 v = 1.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"1");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 2.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"2");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 10.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 10.581;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10.581");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 0.5;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"0.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 0.99025;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"0.99025");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 0.000125;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"0.000125");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 1.5;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"1.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 10.5;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = 10183.52;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"10183.52");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatNegativeFloat64)
    {
        {
            f64 v = -10.0;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = -0.5;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-0.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = -0.99025;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-0.99025");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = -0.000125;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-0.000125");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = -1.5;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-1.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = -10.5;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10.5");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = -10183.52;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-10183.52");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatFloat64Nan)
    {
        f64 v = SSSEngine::FloatTraits<f64>::NaN;
        Utf8 string = Format<Utf8Encoding>(u8"{}", v);
        Utf8 result(u8"NaN");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatFloat64Infinity)
    {
        {
            f64 v = SSSEngine::FloatTraits<f64>::PositiveInfinity;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"Infinity");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            f64 v = SSSEngine::FloatTraits<f64>::NegativeInfinity;
            Utf8 string = Format<Utf8Encoding>(u8"{}", v);
            Utf8 result(u8"-Infinity");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    // =================================================================================================================
    // Characters & Strings
    // =================================================================================================================

    SSSTEST_TEST(FormatString)
    {
        Utf8View view(u8"Hello!");
        Utf8 string = Format<Utf8Encoding>(u8"{}", view);
        Utf8 result(u8"Hello!");
        SSSTEST_EXPECT_EQ(string, result);
    }

    SSSTEST_TEST(FormatChar)
    {
        char8 v = u8'o';
        auto string = Format<Utf8Encoding>(u8"{}", v);
        Utf8View result(u8"o");
        SSSTEST_EXPECT_EQ(string, result);
    }

    // =================================================================================================================
    // Bool
    // =================================================================================================================

    SSSTEST_TEST(FormatBool)
    {
        {
            bool v = true;
            auto string = Format<Utf8Encoding>(u8"{}", v);
            Utf8View result(u8"True");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            bool v = false;
            auto string = Format<Utf8Encoding>(u8"{}", v);
            Utf8View result(u8"False");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    // =================================================================================================================
    // Index based
    // =================================================================================================================

    SSSTEST_TEST(FormatWithIndices)
    {
        {
            int a = 10;
            int b = 15;
            auto result = Format<Utf8Encoding>(u8"{1} {0}", a, b);
            Utf8View expected = u8"15 10";
            SSSTEST_EXPECT_EQ(result, expected);
        }
        {
            int a = 10;
            int b = 15;
            auto result = Format<Utf8Encoding>(u8"{10} {0}", a, a, a, a, a, a, a, a, a, a, b);
            Utf8View expected = u8"15 10";
            SSSTEST_EXPECT_EQ(result, expected);
        }
    }

    // =================================================================================================================
    // Pointer
    // =================================================================================================================

    SSSTEST_TEST(FormatPointer)
    {
        int v = 1230;
        {
            void *add = &v;
            auto result = Format<Utf8Encoding>(u8"{}", add);
            auto expected = Format<Utf8Encoding>(u8"{:#x}", reinterpret_cast<u64>(&v));
            SSSTEST_EXPECT_EQ(result, expected);
        }
        {
            int *add = &v;
            auto result = Format<Utf8Encoding>(u8"{}", add);
            auto expected = Format<Utf8Encoding>(u8"{:#x}", reinterpret_cast<u64>(&v));
            SSSTEST_EXPECT_EQ(result, expected);
        }
    }

    // ==============================================================================================================
    // ToString
    // ==============================================================================================================

    SSSTEST_TEST(FormatToString)
    {
        {
            int v = 98123;
            auto result = ToString<Utf8Encoding>(v);
            Utf8View expected = u8"98123";
            SSSTEST_EXPECT_EQ(result, expected);
        }
        {
            f32 v = 8.23f;
            auto result = ToString<Utf8Encoding>(v);
            Utf8View expected = u8"8.23";
            SSSTEST_EXPECT_EQ(result, expected);
        }
    }
} // namespace SSSTest
