//
// Created by Francisco Santos on 12/07/2023.
//

#pragma once

#include <cstdint>

namespace SSSEngine
{

	class Window
	{
	public:
		explicit Window(void *handle);

		using windowSize = uint32_t;
	public:
		void* Handle;
	private:
		windowSize Width;
		windowSize Height;
	};

} // SSSEngine

