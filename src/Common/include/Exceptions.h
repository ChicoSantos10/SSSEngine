//
// Created by Francisco Santos on 23/09/2023.
//

#pragma once

#include <exception>

class NotImplementedException : public std::exception
{
public:
	[[nodiscard]] const char* what() const noexcept override
	{
		return "Not implemented";
	}
};