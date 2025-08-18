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
 * @brief
 */

#pragma once

#include "Types.h"

namespace SSSEngine::Core::Gameobjects
{
    struct SubmeshData
    {
        u32 indexCount{0};
        u32 startIndex{0};
        u32 baseVertexLocation{0};

        // TODO: Bounding box
    };

    struct MeshGeometry
    {
        // TODO: This should not be void pointers but we need memory management before changing this
        void *vertexBufferCpu = nullptr;
        void *indexBufferCpu = nullptr;
        void *vertexBufferGpu = nullptr;
        void *indexBufferGpu = nullptr;
        void *vertexBufferUploader = nullptr;
        void *indexBufferUploader = nullptr;

        u32 vertexByteStride{0}, vertexBufferByteSize{0};
        // TODO: Index format / size (16, 32)
        u32 indexBufferByteSize{0};
    };
} // namespace SSSEngine::Core::Gameobjects
