/*  SSS Engine
Copyright (C) 2024  Francisco Santos

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

#include <exception>
#include <iostream>
#include "Win32Utils.h"

void Win32::ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		char message[1024];

		GetErrorMessage(hr, message);
		std::cout << message << std::endl;

		throw std::exception();
	}
}

void Win32::GetErrorMessage(HRESULT hr, char *message)
{
	LPWSTR messageBuffer = nullptr;

	LCID langId;
	GetLocaleInfoEx(L"en-US", LOCALE_RETURN_NUMBER | LOCALE_ILANGUAGE, reinterpret_cast<LPWSTR>(&langId),
	                sizeof(langId));

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
	               hr, langId, (LPWSTR)&messageBuffer, 0, nullptr);

	//Copy the error message into a std::string.
	sprintf(message, "%ls", messageBuffer);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);
}

// TODO: This in directx dll
//void Win32::GetDeviceRemovedReason(const ComPtr<ID3D12Device>& device, char *message)
//{
//	HRESULT deviceRemovedReason = device->GetDeviceRemovedReason();
//	if (FAILED(deviceRemovedReason))
//		GetErrorMessage(deviceRemovedReason, message);
//}
