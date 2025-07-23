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

#pragma once

#include "Concepts.h"
#include "HelperMacros.h"

// INVESTIGATE: Are Intrinsics worth it here? Would need to make sure of alignment
// TODO: Vector Math like cross product
namespace SSSMath
{
    template<SSSEngine::NumberConcept T>
    struct Vector2
    {
        T X{0}, Y{0};
    };

    template<SSSEngine::NumberConcept T>
    struct Vector3
    {
        T X{0}, Y{0}, Z{0};
    };

    template<SSSEngine::NumberConcept T>
    struct Vector4
    {
        T X{0}, Y{0}, Z{0}, W{0};
    };

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector2<T> operator+(Vector2<T> lhs, Vector2<T> rhs)
    {
        return {lhs.X + rhs.X, lhs.Y + rhs.Y};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector3<T> operator+(Vector3<T> lhs, Vector3<T> rhs)
    {
        return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector4<T> operator+(Vector4<T> lhs, Vector4<T> rhs)
    {
        return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector2<T> operator-(Vector2<T> lhs, Vector2<T> rhs)
    {
        return {lhs.X - rhs.X, lhs.Y - rhs.Y};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector3<T> operator-(Vector3<T> lhs, Vector3<T> rhs)
    {
        return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector4<T> operator-(Vector4<T> lhs, Vector4<T> rhs)
    {
        return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector2<T> operator*(Vector2<T> lhs, Vector2<T> rhs)
    {
        return {lhs.X * rhs.X, lhs.Y * rhs.Y};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector3<T> operator*(Vector3<T> lhs, Vector3<T> rhs)
    {
        return {lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector4<T> operator*(Vector4<T> lhs, Vector4<T> rhs)
    {
        return {lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z, lhs.W * rhs.W};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector2<T> operator/(Vector2<T> lhs, Vector2<T> rhs)
    {
        return {lhs.X / rhs.X, lhs.Y / rhs.Y};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector3<T> operator/(Vector3<T> lhs, Vector3<T> rhs)
    {
        return {lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector4<T> operator/(Vector4<T> lhs, Vector4<T> rhs)
    {
        return {lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z, lhs.W / rhs.W};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector2<T> operator*(Vector2<T> lhs, T scalar)
    {
        return {lhs.X * scalar, lhs.Y * scalar};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector3<T> operator*(Vector3<T> lhs, T scalar)
    {
        return {lhs.X * scalar, lhs.Y * scalar, lhs.Z * scalar};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector4<T> operator*(Vector4<T> lhs, T scalar)
    {
        return {lhs.X * scalar, lhs.Y * scalar, lhs.Z * scalar, lhs.W * scalar};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector2<T> operator/(Vector2<T> vector, T scalar)
    {
        return {vector.X / scalar, vector.Y / scalar};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector3<T> operator/(Vector3<T> vector, T scalar)
    {
        return {vector.X / scalar, vector.Y / scalar, vector.Z / scalar};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector4<T> operator/(Vector4<T> vector, T scalar)
    {
        return {vector.X / scalar, vector.Y / scalar, vector.Z / scalar, vector.W / scalar};
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector2<T> operator*(T scalar, Vector2<T> vector)
    {
        return vector * scalar;
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector3<T> operator*(T scalar, Vector3<T> vector)
    {
        return vector * scalar;
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL Vector4<T> operator*(T scalar, Vector4<T> vector)
    {
        return vector * scalar;
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL bool operator==(Vector2<T> lhs, Vector2<T> rhs)
    {
        return lhs.X == rhs.X && lhs.Y == rhs.Y;
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL bool operator==(Vector3<T> lhs, Vector3<T> rhs)
    {
        return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z;
    }

    template<SSSEngine::NumberConcept T>
    SSSENGINE_GLOBAL bool operator==(Vector4<T> lhs, Vector4<T> rhs)
    {
        return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z && lhs.W == rhs.W;
    }

    // INVESTIGATE: Should this be in a different file?
    using Int2 = Vector2<int>;
    using Int3 = Vector3<int>;
    using Int4 = Vector4<int>;
    using Float2 = Vector2<float>;
    using Float3 = Vector3<float>;
    using Float4 = Vector4<float>;
} // namespace SSSMath
