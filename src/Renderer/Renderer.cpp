//
// Created by Francisco Santos on 20/09/2023.
//

namespace Renderer
{
	namespace
	{
		typedef void (* Init)();
		typedef void (* CreateSwapChain)(HWND);
		typedef void (* CreateRTV)();
		typedef void (* Render)();
		typedef void (* Terminate)();

		HMODULE module;
		CreateSwapChain createSwapChain;
		CreateRTV createRtv;
		Render render;
		Terminate terminate;
	}

	void LoadDirectx()
	{
		// TODO: Relative Path
		module = LoadLibraryEx(L"N:\\C++Projects\\SSSEngine\\build-debug-msvc\\bin\\Directx12\\Directx12.dll", nullptr, 0);

		if (!module)
			throw std::exception();

		Init init = (Init) GetProcAddress(module, "Initialize");
		if (init)
		{
			init();
		}

		createSwapChain = (CreateSwapChain) GetProcAddress(module, "CreateSwapChain");
		assert(createSwapChain && "Could not load Create Swap Chain");

		createRtv = (CreateRTV) GetProcAddress(module, "CreateRTV");
		assert(createRtv && "Could not load Create RTV");

		render = (Render) GetProcAddress(module, "Render");
		assert(render && "Could not load Create Render");

		terminate = (Terminate) GetProcAddress(module, "Terminate");
		assert(terminate && "Could not load Create Render");
	}

	void Unload()
	{
		terminate();
		FreeLibrary(module);
		module = nullptr;
	}
}