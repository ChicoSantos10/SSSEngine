//
// Created by Francisco Santos on 20/09/2023.
//
#include "Renderer.h"

namespace Renderer
{
	namespace
	{
		typedef void (* Init)();

		HMODULE module;
	}

	void Unload()
	{
		Terminate();
		FreeLibrary(module);
		module = nullptr;
	}

	template<typename T>
	inline void LoadFunction(T& type, const char* name)
	{
		type = (T) GetProcAddress(module, name);
		assert(type);
	}

	void LoadDirectx()
	{
		if (module)
			Unload();

		// TODO: Relative Path
		module = LoadLibraryEx(LR"(N:\C++Projects\SSSEngine\build-debug-msvc\bin\Directx12\Directx12.dll)", nullptr, 0);

		if (!module)
			throw std::exception();

		Init init;
		LoadFunction(init, "Initialize");
		init();
		LoadFunction(LoadAssetsTest, "LoadAssetsTest");
		LoadFunction(CreateDepthStencilBuffer, "CreateDepthStencilBuffer");
		LoadFunction(CreateSwapChain, "CreateSwapChain");
		LoadFunction(CreateRtv, "CreateRTV");
		LoadFunction(Render, "Render");
		LoadFunction(Terminate, "Terminate");
	}
}