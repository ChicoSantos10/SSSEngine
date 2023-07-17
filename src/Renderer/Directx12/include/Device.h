//
// Created by Francisco Santos on 17/07/2023.
//

#pragma once

#include <wrl/client.h>
#include "directx/d3d12.h"

namespace Renderer::Directx
	{
		class Device
		{
		public:
			Device();
			[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() const { return m_Device; }
			explicit operator Microsoft::WRL::ComPtr<ID3D12Device2>() const { return m_Device; }
		private:
			Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;
		};

	} // Directx
