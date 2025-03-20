//
// Created by Francisco Santos on 23/09/2023.
//

#pragma once

#include <exception>
#include "Attributes.h"
#if defined(SSSENGINE_DEBUG) || defined(SSSENGINE_INTERNAL)

class NotImplementedException final : public std::exception
{
    static constexpr auto Reason = "Not implemented";

public:
    SSSENGINE_PURE const char *what() const noexcept override { return Reason; }
};

#endif
