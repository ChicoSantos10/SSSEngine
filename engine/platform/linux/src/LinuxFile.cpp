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

/**
 * @file
 * @brief Linux implementation for file system
 */

#include "Bits.h"
#include "File.h"

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

namespace SSSEngine::Platform
{
    template<FilePermissions T>
    File<T>::FileHandle File<T>::PlatformOpenFile(const FilePath &path, bool create)
    {
        using enum FilePermissions;
        int flags = 0;
        if(HasBitSet(T, Join(Read, Write)))
        {
            flags = O_RDWR;
        }
        else if(HasBitSet(T, Read))
        {
            flags = O_RDONLY;
        }
        else if(HasBitSet(T, Write))
        {
            flags = O_WRONLY;
        }

        if(create)
        {
            flags = Join(flags, O_CREAT);
        }

        FileHandle handle = open(path, flags);

        if(handle < 0)
        {
            // TODO: Handle error
        }

        return handle;
    }

    template<FilePermissions T>
    bool File<T>::PlatformCloseFile()
    {
        int success = close(m_fileHandle);

        if(success == -1)
        {
            // TODO: Handle Error. Note that this error may be related to previous writing operations and not
            // necessarily with the closing
            return false;
        }

        return true;
    }

    template<FilePermissions T>
    bool File<T>::PlatformWriteFile(const void *data, Size size)
    {
        ssize_t writtenBytes = write(m_fileHandle, data, size);

        if(writtenBytes == -1)
        {
            // TODO: Handle Error
            return false;
        }

        if(writtenBytes != size)
        {
            // INVESTIGATE: Operation did not fail, but did not write all the data
            // What should we do? Try again? Let the caller decide?
            return false;
        }

        return true;
    }

    template<FilePermissions T>
    bool File<T>::PlatformReadFile(void *buffer, Size maxBytes) const
    {
        ssize_t bytesRead = read(m_fileHandle, buffer, maxBytes);

        if(bytesRead == -1)
        {
            // TODO: Handle Error
            return false;
        }

        return true;
    }

    FileData FromStat(struct stat &data)
    {
        constexpr auto Time = [](timespec time) -> ExtendedFileData::Time
        { return {.seconds = time.tv_sec, .nanoseconds = static_cast<u32>(time.tv_nsec)}; };

        return {.size = static_cast<Size>(data.st_size),
                .creationTime = Time(data.st_ctim), // FIX: This is actually not the creation time!!!
                .lastAccessTime = Time(data.st_atim),
                .lastWriteTime = Time(data.st_mtim)};
    }

    template<FilePermissions T>
    ExtendedFileData File<T>::PlatformFileInformation()
    {
        struct stat data{};
        int success = fstat(m_fileHandle, &data);

        if(success == -1)
        {
            // TODO: Handle Error
            return {};
        }

        return {FromStat(data)};
    }

    FileData GetFileData(const FilePath &path)
    {
        struct stat data{};
        int success = stat(path, &data);

        if(success == -1)
        {
            // TODO: Handle error
            return {};
        }

        return FromStat(data);
    }
} // namespace SSSEngine::Platform
