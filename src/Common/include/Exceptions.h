//
// Created by Francisco Santos on 23/09/2023.
//

#pragma once

#if defined(SSSENGINE_DEBUG) || defined(SSSENGINE_INTERNAL)
#include <exception>

class NotImplementedException final : public std::exception
{
	static constexpr auto Reason = "Not implemented";

public:
	SSSENGINE_PURE const char* what() const noexcept override
	{
		return Reason;
	}
};
#endif

