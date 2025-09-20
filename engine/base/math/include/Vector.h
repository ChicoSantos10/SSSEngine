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
 * @brief Vectors and their operations
 */

#pragma once

#include "Attributes.h"
#include "Concepts.h"
#include "Types.h"

// INVESTIGATE: Are Intrinsics worth it here? Would need to make sure of alignment
// TODO: Vector Math like cross product
namespace SSSEngine::Math
{
    template<SSSEngine::NumberConcept T>
    struct Vector2
    {
        T x{0};
        T y{0};

        SSSENGINE_FORCE_INLINE friend Vector2<T> operator+(const Vector2<T> &lhs, const Vector2<T> &rhs)
        {
            return {lhs.x + rhs.x, lhs.y + rhs.y};
        }

        SSSENGINE_FORCE_INLINE friend Vector2<T> operator-(Vector2<T> lhs, Vector2<T> rhs)
        {
            return {lhs.x - rhs.x, lhs.y - rhs.y};
        }

        SSSENGINE_FORCE_INLINE friend Vector2<T> operator*(const Vector2<T> &lhs, const Vector2<T> &rhs)
        {
            return {lhs.x * rhs.x, lhs.y * rhs.y};
        }

        SSSENGINE_FORCE_INLINE friend Vector2<T> operator/(Vector2<T> lhs, Vector2<T> rhs)
        {
            return {lhs.x / rhs.x, lhs.y / rhs.y};
        }

        SSSENGINE_FORCE_INLINE friend Vector2<T> operator*(Vector2<T> lhs, T scalar)
        {
            return {lhs.x * scalar, lhs.y * scalar};
        }

        SSSENGINE_FORCE_INLINE friend Vector2<T> operator/(Vector2<T> vector, T scalar)
        {
            return {vector.x / scalar, vector.y / scalar};
        }

        SSSENGINE_FORCE_INLINE friend Vector2<T> operator*(T scalar, Vector2<T> vector)
        {
            return vector * scalar;
        }

        SSSENGINE_FORCE_INLINE friend bool operator==(Vector2<T> lhs, Vector2<T> rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };

    template<SSSEngine::NumberConcept T>
    struct Vector3
    {
        T x{0};
        T y{0};
        T z{0};

        SSSENGINE_FORCE_INLINE friend Vector3<T> operator+(Vector3<T> lhs, Vector3<T> rhs)
        {
            return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
        }

        SSSENGINE_FORCE_INLINE friend Vector3<T> operator-(Vector3<T> lhs, Vector3<T> rhs)
        {
            return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
        }

        SSSENGINE_FORCE_INLINE friend Vector3<T> operator*(Vector3<T> lhs, Vector3<T> rhs)
        {
            return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
        }

        SSSENGINE_FORCE_INLINE friend Vector3<T> operator/(Vector3<T> lhs, Vector3<T> rhs)
        {
            return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
        }

        SSSENGINE_FORCE_INLINE friend Vector3<T> operator*(Vector3<T> lhs, T scalar)
        {
            return {lhs.x * scalar, lhs.y * scalar, lhs.z * scalar};
        }

        SSSENGINE_FORCE_INLINE friend Vector3<T> operator/(Vector3<T> vector, T scalar)
        {
            return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
        }

        SSSENGINE_FORCE_INLINE friend Vector3<T> operator*(T scalar, Vector3<T> vector)
        {
            return vector * scalar;
        }

        SSSENGINE_FORCE_INLINE friend bool operator==(Vector3<T> lhs, Vector3<T> rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
        }
    };

    template<SSSEngine::NumberConcept T>
    struct Vector4
    {
        T x{0};
        T y{0};
        T z{0};
        T w{0};

        SSSENGINE_FORCE_INLINE friend Vector4<T> operator+(Vector4<T> lhs, Vector4<T> rhs)
        {
            return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
        }

        SSSENGINE_FORCE_INLINE friend Vector4<T> operator-(Vector4<T> lhs, Vector4<T> rhs)
        {
            return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
        }

        SSSENGINE_FORCE_INLINE friend Vector4<T> operator*(Vector4<T> lhs, Vector4<T> rhs)
        {
            return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w};
        }

        SSSENGINE_FORCE_INLINE friend Vector4<T> operator/(Vector4<T> lhs, Vector4<T> rhs)
        {
            return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w};
        }

        SSSENGINE_FORCE_INLINE friend Vector4<T> operator*(Vector4<T> lhs, T scalar)
        {
            return {lhs.x * scalar, lhs.y * scalar, lhs.z * scalar, lhs.w * scalar};
        }

        SSSENGINE_FORCE_INLINE friend Vector4<T> operator/(Vector4<T> vector, T scalar)
        {
            return {vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w / scalar};
        }

        SSSENGINE_FORCE_INLINE friend Vector4<T> operator*(T scalar, Vector4<T> vector)
        {
            return vector * scalar;
        }

        SSSENGINE_FORCE_INLINE friend bool operator==(Vector4<T> lhs, Vector4<T> rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
        }
    };

    // INVESTIGATE: Should this be in a different file?
    using Int2 = Vector2<i32>;
    using Int3 = Vector3<i32>;
    using Int4 = Vector4<i32>;
    using UInt2 = Vector2<u32>;
    using UInt3 = Vector3<u32>;
    using UInt4 = Vector4<u32>;
    using Float2 = Vector2<f32>;
    using Float3 = Vector3<f32>;
    using Float4 = Vector4<f32>;
} // namespace SSSEngine::Math
