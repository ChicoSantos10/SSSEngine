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

#include "HelperMacros.h"
#include "Types.h"
#include "Concepts.h"
#include "Debug.h"
#include <concepts>
#include <type_traits>

namespace SSSMath
{
    using MatrixSize = u32;
    template<SSSEngine::Number T, MatrixSize C, MatrixSize R>
    struct Matrix
    {
        static consteval MatrixSize Rows() { return R; }
        static consteval MatrixSize Columns() { return C; }
        static consteval MatrixSize NumberElements() { return R * C; }
        using Type = T;

        T data[NumberElements()]{0};

        template<class Self>
        constexpr auto &&operator[](this Self &&self, MatrixSize row, MatrixSize col)
        {
            SSSENGINE_ASSERT(row < R);
            SSSENGINE_ASSERT(col < C);

            return std::forward<Self>(self).data[row * C + col];
        }

        template<class Self>
        constexpr auto &&operator[](this Self &&self, MatrixSize index)
        {
            SSSENGINE_ASSERT(index < NumberElements());

            return std::forward<Self>(self).data[index];
        }
    };

    using Mat4x4f = Matrix<float, 4, 4>;

    template<typename T, template<typename, MatrixSize, MatrixSize> typename V>
    struct IsMatrix : std::false_type
    {
    };

    template<template<typename, MatrixSize, MatrixSize> typename V, typename U, MatrixSize C, MatrixSize R>
    struct IsMatrix<V<U, C, R>, V> : std::true_type
    {
    };

    template<typename T>
    concept MatrixType = IsMatrix<T, Matrix>::value;

    template<typename T>
    concept SquareMatrix = requires { IsMatrix<T, Matrix>::value &&T::Rows() == T::Columns(); };

    template<MatrixType T, MatrixType V>
        requires(std::same_as<T, V>) && (T::Columns() == V::Rows())
    SSSENGINE_GLOBAL constexpr auto operator*(const T &lhs, const V &rhs)
    {
        using Type = typename T::Type;
        constexpr MatrixSize RowsLhs = T::Rows();
        constexpr MatrixSize RowsRhs = V::Rows();
        constexpr MatrixSize ColumnsRhs = V::Columns();

        Matrix<Type, ColumnsRhs, RowsRhs> result;

        for(MatrixSize i = 0; i < RowsLhs; ++i)
        {
            for(MatrixSize k = 0; k < RowsLhs; ++k)
            {
                for(MatrixSize j = 0; j < RowsLhs; ++j)
                {
                    result[i, j] += lhs[i, k] * rhs[k, j];
                }
            }
        }

        return result;
    }

    template<MatrixType T>
    SSSENGINE_GLOBAL constexpr bool operator==(const T &lhs, const T &rhs)
    {
        constexpr MatrixSize Rows = T::Rows();
        constexpr MatrixSize Columns = T::Columns();
        constexpr MatrixSize Size = Rows * Columns;

        for(MatrixSize i = 0; i < Size; ++i)
        {
            if(lhs[i] != rhs[i])
            {
                return false;
            }
        }

        return true;
    }

    template<MatrixType T>
    SSSENGINE_GLOBAL constexpr bool operator!=(const T &lhs, const T &rhs)
    {
        return !(lhs == rhs);
    }

    template<SquareMatrix M>
    consteval M IdentityMatrix()
    {
        constexpr MatrixSize Size = M::Rows();

        M m;

        for(int i = 0; i < Size; ++i)
        {
            for(int j = 0; j < Size; ++j)
            {
                if(i == j)
                {
                    m[i, j] = 1;
                }
                else
                {
                    m[i, j] = 0;
                }
            }
        }
        return m;
    }
} // namespace SSSMath
