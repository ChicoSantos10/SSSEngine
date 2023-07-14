//
// Created by Francisco Santos on 14/07/2023.
//

#pragma once

#ifndef SSSENGINE_ENTRY_POINT
	#error "You must define a entry point for the application!"
#endif

#include "Window.h"

namespace SSSEngine
{
	class Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;

	private:
		void Run() const;
		friend int ::SSSENGINE_ENTRY_POINT;

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

} // SSSEngine