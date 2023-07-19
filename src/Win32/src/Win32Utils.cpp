//
// Created by Francisco Santos on 11/07/2023.
//

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

void Win32::GetErrorMessage(HRESULT hr, char* message)
{
	LPWSTR messageBuffer = nullptr;

	LCID langId;
	GetLocaleInfoEx(L"en-US", LOCALE_RETURN_NUMBER | LOCALE_ILANGUAGE, reinterpret_cast<LPWSTR>(&langId), sizeof(langId));

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               nullptr, hr, langId, (LPWSTR)&messageBuffer, 0, nullptr);

	//Copy the error message into a std::string.
	sprintf(message, "%ls", messageBuffer);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);
}

void Win32::GetDeviceRemovedReason(const ComPtr<ID3D12Device>& device, char *message)
{
	HRESULT deviceRemovedReason = device->GetDeviceRemovedReason();
	if (FAILED(deviceRemovedReason))
		GetErrorMessage(deviceRemovedReason, message);
}
