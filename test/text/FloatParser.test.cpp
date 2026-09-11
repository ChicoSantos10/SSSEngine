#include "ArrayTraits.h"
#include "Float.h"
#include "HelperMacros.h"
#include "Logger.h"
#include "StringView.h"
#include "Test.h"
#include "FloatParser.h"

namespace SSSTest
{
    SSSTEST_TEST(FloatToString)
    {
        SSSENGINE_FUNCTION_LOCAL constexpr f32 Numbers[]{
            1.0f,
            1.01f,
            1.1f,
            1.0002984f,
            2.0f,
            2.5f,
            4.0f,
            8.f,
            100.f,
            1e30f,
            1e-30,
            120.f,
            12'000'000.f,
            SSSEngine::BitCopy<f32>(0x00000001),
            SSSEngine::BitCopy<f32>(0x007FFFFF),
            SSSEngine::FloatTraits<f32>::Max,
        };
        SSSENGINE_FUNCTION_LOCAL constexpr const char *Digits[]{
            "1",
            "101",
            "11",
            "10002984",
            "2",
            "25",
            "4",
            "8",
            "1",
            "1",
            "1",
            "12",
            "12",
            "1",
            "11754942",
            "34028235",
        };
        SSSENGINE_FUNCTION_LOCAL constexpr i32 Exponents[]{
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            2,
            30,
            -30,
            2,
            7,
            -45,
            -38,
            38,
        };
        SSSENGINE_FUNCTION_LOCAL constexpr u32 SigDigits[]{
            1,
            3,
            2,
            8,
            1,
            2,
            1,
            1,
            1,
            1,
            1,
            2,
            2,
            1,
            8,
            8,
        };

        for(SizeType i = 0; i < SSSEngine::CountOf<decltype(Numbers)>; ++i)
        {
            f32 v = Numbers[i];
            auto end = SSSEngine::Text::FloatToAscii(v);
            SSSEngine::Text::AsciiView string(end.digits.Data(), SigDigits[i]);
            SSSTEST_EXPECT_EQ(string, Digits[i]);
            SSSTEST_EXPECT_EQ(end.exponent, Exponents[i]);
            SSSTEST_EXPECT_EQ(end.significantDigits, SigDigits[i]);
        }

        auto end = SSSEngine::Text::FloatToAscii(0.f);
        SSSEngine::Text::AsciiView string(end.digits.Data(), 1);
        SSSTEST_EXPECT_EQ(string, "0");
        SSSTEST_EXPECT_EQ(end.exponent, 0);
        SSSTEST_EXPECT_EQ(end.significantDigits, 0);
    }

} // namespace SSSTest
