/*  SSS Engine
    Copyright (C) 2025  Francisco Santos

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

#include "Bits.h"
#include "Debug.h"
#include "File.h"
#include "TimeDuration.h"

#include "Win32Utils.h"
#include "Windows.h"

// INVESTIGATE: We could potentially create files onto a memory pool and retrieve a handle to it here
namespace SSSEngine::Platform
{
    namespace Windows
    {
        using ::CreateFile2;
        using ::CloseHandle;
        using ::WriteFile;
        using ::ReadFile;
        using ::GetFileInformationByHandle;
        using ::GetFileAttributesEx;
    } // namespace Windows

    template<FilePermissions T>
    File<T>::FileHandle File<T>::PlatformOpenFile(const FilePath &path, bool create)
    {
        using enum FilePermissions;
        auto access = Join(HasBitSet(T, Read) * GENERIC_READ, HasBitSet(T, Write) * GENERIC_WRITE);

        // INVESTIGATE: What other flags may be necessary
        auto handle = Windows::CreateFile2(path.c_str(), access, 0, create ? CREATE_NEW : OPEN_EXISTING, nullptr);

        if(handle == INVALID_HANDLE_VALUE)
        {
            // INVESTIGATE: What to do when the class becomes malformed
            return nullptr;
        }

        return handle;
    }

    template<FilePermissions T>
    bool File<T>::PlatformCloseFile()
    {
        bool success = Windows::CloseHandle(m_fileHandle);
        m_fileHandle = nullptr;

        return success;
    }

    template<FilePermissions T>
    bool File<T>::PlatformWriteFile(void *data, Size size)
    {
        // INVESTIGATE: WriteFileEx and async writes (do we need async or can we just use threads that block essentially
        // doing the async ourselves?)
        // - Where to write (begin, end, offset)

        SSSENGINE_ASSERT(m_fileHandle);
        SSSENGINE_ASSERT(HasBitSet(T, FilePermissions::Write));

        DWORD bytes{0};
        LPDWORD writtenBytes = &bytes;
        return Windows::WriteFile(m_fileHandle, data, size, writtenBytes, nullptr);
    }

    template<FilePermissions T>
    bool File<T>::PlatformReadFile(void *buffer, Size maxBytes) const
    {
        SSSENGINE_ASSERT(m_fileHandle);
        SSSENGINE_ASSERT(HasBitSet(T, FilePermissions::Read));

        return Windows::ReadFile(m_fileHandle, buffer, maxBytes, nullptr, nullptr);
    }

    /**
     * @brief Converts time from windows to time since epoch
     *
     * Since Windows uses the amount of 100-nanoseconds since January 1, 1601, we need to convert into a time that can
     * be used by all systems. We chose the common seconds since epoch (January 1, 1970)
     *
     * @param windowsTime The time from windows system
     * @return The time converted to seconds since epoch
     */
    FileData::Time ConvertWindowsTimeToPosix(FILETIME windowsTime)
    {
        using Seconds_t = decltype(FileData::Time::seconds);
        using Nanoseconds_t = decltype(FileData::Time::nanoseconds);
        constexpr char WindowsInterval = 100;
        constexpr i64 SecondsDifferenceWindowsToEpoch = 11644473600LL; // How many seconds from 01/01/1601 to 01/01/1970

        auto time = Win32::Convert32to64<Seconds_t>(windowsTime.dwHighDateTime, windowsTime.dwLowDateTime);

        Seconds_t totalNanoseconds = time * WindowsInterval;
        Seconds_t windowsTotalSeconds = totalNanoseconds / Nanoseconds;

        Seconds_t seconds = windowsTotalSeconds - SecondsDifferenceWindowsToEpoch;
        Nanoseconds_t nanoseconds = totalNanoseconds % Nanoseconds;

        return {.seconds = seconds, .nanoseconds = nanoseconds};
    }

    template<FilePermissions T>
    ExtendedFileData File<T>::PlatformFileInformation()
    {
        BY_HANDLE_FILE_INFORMATION info;
        bool success = Windows::GetFileInformationByHandle(m_fileHandle, &info);

        // TODO: Return an optional/expected instead of asserting
        SSSENGINE_ASSERT(success);

        Size fileSize = Win32::Convert32to64<Size>(info.nFileSizeHigh, info.nFileSizeLow);

        return {{.size = fileSize,
                 .creationTime = ConvertWindowsTimeToPosix(info.ftCreationTime),
                 .lastAccessTime = ConvertWindowsTimeToPosix(info.ftLastAccessTime),
                 .lastWriteTime = ConvertWindowsTimeToPosix(info.ftLastWriteTime)}};
    }

    FileData GetFileData(const FilePath &path)
    {
        WIN32_FILE_ATTRIBUTE_DATA data;
        bool success = Windows::GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &data);

        // TODO: Return an expected/optional
        SSSENGINE_ASSERT(success);

        return {.size = Win32::Convert32to64<Size>(data.nFileSizeHigh, data.nFileSizeLow),
                .creationTime = ConvertWindowsTimeToPosix(data.ftCreationTime),
                .lastAccessTime = ConvertWindowsTimeToPosix(data.ftLastAccessTime),
                .lastWriteTime = ConvertWindowsTimeToPosix(data.ftLastWriteTime)};
    }

} // namespace SSSEngine::Platform
