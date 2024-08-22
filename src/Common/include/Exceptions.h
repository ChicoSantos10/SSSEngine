//
// Created by Francisco Santos on 23/09/2023.
//

#pragma once

#include <exception>

// TODO: Remove this from release builds
class NotImplementedException : public std::exception
{
	static constexpr char const* const reason = "Not implemented";
public:
	[[nodiscard]] const char* what() const noexcept override
	{
		return reason;
	}
};