/**
 * @file
 * @brief C String Helper Functions
 */

#pragma once

#include "Attributes.h"
#include "Encoding.h"

namespace SSSEngine::Text
{
    template<EncodingConcept Encoding>
        SSSENGINE_PURE
    bool StringEqual(const typename Encoding::CodeUnitType *lhs, const typename Encoding::CodeUnitType *rhs) noexcept
    {
        if(lhs == rhs)
        {
            return true;
        }

        while(*lhs == *rhs)
        {
            if(!*lhs)
            {
                return true;
            }

            ++lhs, ++rhs;
        }

        return false;
    }
} // namespace SSSEngine::Text
