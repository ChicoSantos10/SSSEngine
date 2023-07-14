//
// Created by Francisco Santos on 12/07/2023.
//

#include "Window.h"

#include <utility>

namespace SSSEngine
{
	Window::Window(Window::windowSize width, Window::windowSize height, std::string title)
			: m_Width(width), m_Height(height), m_Title(std::move(title))
	{}
} // SSSEngine