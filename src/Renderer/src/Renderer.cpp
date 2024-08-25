/*  SSS Engine
    Copyright (C) ${YEAR}  Francisco Santos

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// TODO: Create a platform layer for loading and unloading dlls
#if SSSENGINE_WIN32

#include "windows.h"

#endif

#include "Renderer.h"

namespace SSSRenderer
{
	namespace
	{
		typedef void (*Init)();

		HMODULE module;
	}

	void Unload()
	{
		Terminate();
		FreeLibrary(module);
		module = nullptr;
	}

	template <typename T>
	SSSENGINE_FORCE_INLINE void LoadFunction(T &type, const char *name)
	{
		type = reinterpret_cast<T>(GetProcAddress(module, name));
		SSSENGINE_ASSERT(type);
	}

	void LoadDirectx()
	{
		if (module)
			Unload();

		// TODO: Relative Path
		module = LoadLibraryEx(LR"(N:\C++Projects\SSSEngine\build-debug-msvccl\bin\Directx12\Directx12.dll)",
		                       nullptr,
		                       0
		);

		if (!module)
			throw std::exception();

		Init init;
		LoadFunction(init, "Initialize");
		init();
		LoadFunction(LoadAssetsTest, "LoadAssetsTest");
		LoadFunction(CreateSwapChain, "CreateSwapChain");
		LoadFunction(ResizeSwapChain, "ResizeSwapChain");
		LoadFunction(Render, "Render");
		LoadFunction(Terminate, "Terminate");
	}
}
