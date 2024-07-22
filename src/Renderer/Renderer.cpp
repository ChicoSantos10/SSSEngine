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
		typedef void (* LoadAssetsTest)();
		typedef void (* CreateDepthStencilBuffer)(uint32_t width, uint32_t height);

		HMODULE module;
		CreateSwapChain createSwapChain;
		CreateRTV createRtv;
		Render render;
		Terminate terminate;
		LoadAssetsTest loadAssetsTest;
		CreateDepthStencilBuffer createDepthStencilBuffer;
	}

	void Unload()
	{
		terminate();
		FreeLibrary(module);
		module = nullptr;
	}

	template<typename T>
	void LoadFunction(T& type, const char* name)
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

//		auto init = (Init) GetProcAddress(module, "Initialize");
//		assert(init);
//		init();

		Init init;
		LoadFunction(init, "Initialize");
		init();
		LoadFunction(loadAssetsTest, "LoadAssetsTest");
		LoadFunction(createDepthStencilBuffer, "CreateDepthStencilBuffer");
		LoadFunction(createSwapChain, "CreateSwapChain");
		LoadFunction(createRtv, "CreateRTV");
		LoadFunction(render, "Render");
		LoadFunction(terminate, "Terminate");

//		loadAssetsTest = (LoadAssetsTest) GetProcAddress(module, "LoadAssetsTest");
//		assert(loadAssetsTest);
//
//		createDepthStencilBuffer = (CreateDepthStencilBuffer) GetProcAddress(module, "CreateDepthStencilBuffer");
//		assert(createDepthStencilBuffer);
//
//		createSwapChain = (CreateSwapChain) GetProcAddress(module, "CreateSwapChain");
//		assert(createSwapChain && "Could not load Create Swap Chain");
//
//		createRtv = (CreateRTV) GetProcAddress(module, "CreateRTV");
//		assert(createRtv && "Could not load Create RTV");
//
//		render = (Render) GetProcAddress(module, "Render");
//		assert(render && "Could not load Create Render");
//
//		terminate = (Terminate) GetProcAddress(module, "Terminate");
//		assert(terminate && "Could not load Create Render");
	}
}