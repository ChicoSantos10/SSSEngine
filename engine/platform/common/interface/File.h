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
 * @brief Utilities for file creation, reading and data
 */

#pragma once

#include "Attributes.h"
#include "Bits.h"
#include "Debug.h"
#include "Types.h"

#include <string>

namespace SSSEngine::Platform
{
    enum class FilePermissions : u8
    {
        None = 0,
        Read = 0x01,
        Write = 0x02,

        ReadWrite = Join(Read, Write),
    };

    struct FileData
    {
        Size size;

        struct Time
        {
            i64 seconds;
            u32 nanoseconds;
        };

        Time creationTime;
        Time lastAccessTime;
        Time lastWriteTime;
    };

    struct ExtendedFileData : FileData
    {
        // INVESTIGATE: Do we even need anything here? The reason for this extra struct is that on Windows certain
        // information is locked behind a method that takes a filehandle
    };

    // TODO: Create a Path struct. Make it consteval but allow conversion to string types explicitly
    // Must be convertible to Wide String for Windows
    using FilePath = std::wstring;

    template<FilePermissions T = FilePermissions::Read>
    class File final
    {
        using FileHandle = void *;

        public:
        static constexpr FilePermissions AccessPermissions = T;

        // INVESTIGATE: What happens if we fail to get the file?
        explicit File(const FilePath &path, bool create = false) : m_fileHandle{PlatformOpenFile(path, create)} {}

        File() = delete;
        File(File &&) = delete;
        File(const File &) = delete;
        File &operator=(File &&) = delete;
        File &operator=(const File &) = delete;

        ~File()
        {
            PlatformCloseFile();
        };

        /**
         * @brief Writes to a file size bytes from data
         *
         * @param file The writeable file to write into
         * @param data The data to write to
         * @param size The amount of bytes to write from data
         * @return True if the write was successful, false otherwise
         */
        SSSENGINE_FORCE_INLINE friend bool WriteFile(File file, void *data, Size size);
        /**
         * @brief Reads maxBytes from file into a buffer
         *
         * @param file The file to read into buffer
         * @param buffer The buffer to write the bytes read from file
         * @param maxBytes The maximum amount of bytes to read
         * @return True if successful, false otherwise
         */
        SSSENGINE_FORCE_INLINE friend bool ReadFile(File file, void *buffer, Size maxBytes);
        /**
         * @brief Get's the extended file information
         *
         * @important Use this only when you need the extended file data or already have
         * the handle. Do not open a file just to get the information once as opening a file has extra performance
         * costs!
         *
         * @param file The file to get the information
         * @return The file data
         */
        SSSENGINE_FORCE_INLINE friend ExtendedFileData GetExtendedFileData(File file);

        private:
        /**
         * @brief Opens or Creates a file
         *
         * @param path A relative or absolute path to a file
         * @return The platform specific handle to the file
         */
        FileHandle PlatformOpenFile(const FilePath &path, bool create);

        /**
         * @brief Closes a file opened by OpenFile(FilePath)
         *
         */
        bool PlatformCloseFile();

        /**
         * @brief Writes to a file. Only can happen if file was created with write permissions
         * @see FilePermissions
         *
         * @param data Address of the bytes to write
         * @param size How many bytes to write
         *
         * @return True if the write was successful, false otherwise
         */
        // TODO: Pass a buffer instead (address, size)
        bool PlatformWriteFile(void *data, Size size);

        /**
         * @brief Reads a file into a buffer. Can only happen if the file was opened with read permissions
         * @see FilePermissions
         *
         * @param buffer A buffer big enough to store the data read
         * @param readBytes Maximum amount of bytes to read
         * @return True if successful, false otherwise
         */
        // TODO: Should pass a buffer and assert that buffersize >= maxBytes
        bool PlatformReadFile(void *buffer, Size maxBytes) const;

        /**
         * @brief Platform specific way to get the file information
         *
         * @return Returns the complete file information
         */
        ExtendedFileData PlatformFileInformation();

        FileHandle m_fileHandle;
    };

    template<typename F>
    concept WriteFileConcept = (HasBitSet(F::AccessPermissions, FilePermissions::Write));

    SSSENGINE_STATIC_ASSERT(WriteFileConcept<File<FilePermissions::Write>>, "File with write permissions is a "
                                                                            "WriteFileConcept")
    SSSENGINE_STATIC_ASSERT(WriteFileConcept<File<FilePermissions::ReadWrite>>, "File with read and write permissions "
                                                                                "is a "
                                                                                "WriteFileConcept")

    template<typename F>
    concept ReadFileConcept = (HasBitSet(F::AccessPermissions, FilePermissions::Read));

    SSSENGINE_STATIC_ASSERT(ReadFileConcept<File<FilePermissions::Read>>, "File with read permissions is a "
                                                                          "ReadFileConcept")
    SSSENGINE_STATIC_ASSERT(ReadFileConcept<File<FilePermissions::ReadWrite>>, "File with read and write permissions "
                                                                               "is a "
                                                                               "ReadFileConcept")

    bool WriteFile(WriteFileConcept auto &file, void *data, Size size)
    {
        file.PlatformWriteFile(data, size);
    }

    bool ReadFile(const ReadFileConcept auto &file, void *buffer, Size maxBytes)
    {
        file.PlatformReadFile(buffer, maxBytes);
    }

    template<FilePermissions T>
    ExtendedFileData GetExtendedFileData(const File<T> &file)
    {
        file.PlatformFileInformation();
    }

    /**
     * @brief Get's a simplified data for the file at path
     *
     * Faster way to get file data without needing to open a file. If you need more data look at
     * @see GetExtendedFileData
     * @see ExtendedFileData
     *
     * @param path The absolute or relative path to the file
     * @return Returns a simplified data of the file
     */
    FileData GetFileData(const FilePath &path);
} // namespace SSSEngine::Platform
