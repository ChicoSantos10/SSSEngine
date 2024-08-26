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

#include "Renderer.h"
#include "Library.h"

namespace SSSRenderer
{
	using Init_t = void (*)();

	static void *module;

	void Unload()
	{
		Terminate();
		SSSEngine::UnloadSharedLibrary(module);
		module = nullptr;
	}

	template <typename T> concept FunctionPointer = std::is_pointer_v<T>;

	// TODO: The assert should become run time check
	template <FunctionPointer T>
	SSSENGINE_FORCE_INLINE T LoadFunction(const char *name)
	{
		T type = reinterpret_cast<T>(SSSEngine::GetFunctionAddressFromLibrary(module, name));
		SSSENGINE_ASSERT(type);
		return type;
	}

	void LoadDirectx()
	{
		if (module)
			Unload();

		constexpr auto relativePath = LR"(Directx12\Directx12.dll)";
		module = SSSEngine::LoadSharedLibrary(relativePath, 0);

		// TODO: Proper handling / exception throwing
		if (!module)
			throw std::exception();

		LoadFunction<Init_t>("Initialize")();
		LoadAssetsTest = LoadFunction<LoadAssetsTest_t>("LoadAssetsTest");
		CreateSwapChain = LoadFunction<CreateSwapChain_t>("CreateSwapChain");
		ResizeSwapChain = LoadFunction<ResizeSwapChain_t>("ResizeSwapChain");
		Render = LoadFunction<Render_t>("Render");
		Terminate = LoadFunction<Terminate_t>("Terminate");
	}
}
