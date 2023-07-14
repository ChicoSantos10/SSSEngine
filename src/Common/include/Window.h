//
// Created by Francisco Santos on 12/07/2023.
//

#pragma once

#include <cstdint>
#include <string_view>

namespace SSSEngine
{
	class Window
	{
	public:
		using windowSize = uint16_t;

		Window(windowSize width, windowSize height, std::string title);
		~Window() = default;

		[[nodiscard]] inline windowSize GetWidth() const { return m_Width; }
		[[nodiscard]] inline windowSize GetHeight() const { return m_Height; }
		[[nodiscard]] inline std::string_view GetTitle() const { return m_Title; }

		virtual void ChangeWindowTitle(std::string_view title) = 0;
		/* TODO:
		 *  - Add a method to change the window size
		 *  - Add a method to change the window position
		 *  - Add a method to change the window icon
		 *  - Add a method to change the window cursor
		 *  - Add a method to minimize the window
		 */
	protected:
		windowSize m_Width{};
		windowSize m_Height{};

		const std::string m_Title;
	};
} // SSSEngine

