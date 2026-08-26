#include "String.h"
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
} // namespace SSSTest
