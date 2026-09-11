#include "Logger.h"
#include "String.h"
#include "StringView.h"
#include "Test.h"
#include "Utf8Encoding.h"
#include "Formatter.h"

namespace SSSTest
{
    using namespace SSSEngine::Text;

    SSSTEST_TEST(FormatIntegrals)
    {
        {
            i8 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u8 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i16 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u16 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u32 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i64 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            u64 v = 10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"10");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatNegativeIntegrals)
    {
        {
            i8 v = -10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i16 v = -10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i32 v = -10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
        {
            i64 v = -10;
            Utf8 string = Format<Utf8Encoding>("{}", v);
            Utf8 result(u8"-10");
            SSSTEST_EXPECT_EQ(string, result);
        }
    }

    SSSTEST_TEST(FormatString)
    {
        Utf8View view(u8"Hello!");
        Utf8 string = Format<Utf8Encoding>("{}", view);
        Utf8 result(u8"Hello!");
        SSSTEST_EXPECT_EQ(string, result);
    }

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
} // namespace SSSTest
