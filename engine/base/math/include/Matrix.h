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

namespace SSSEngine::Math
{
    using MatrixSize = u32;
    /**
     * @brief A Matrix representation
     *
     * @tparam MatrixSize C The amount of columns
     * @tparam MatrixSize R The amount of rows
     */
    template<SSSEngine::NumberConcept T, MatrixSize C, MatrixSize R>
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

    /**
     * @brief Checks if type T is a matrix
     *
     * @tparam T [TODO:tparam]
     */
    template<typename T, template<typename, MatrixSize, MatrixSize> typename V>
    struct IsMatrix : std::false_type
    {
    };

    /**
     * @brief Checks if is a Matrix
     *
     * @tparam U [TODO:tparam]
     */
    template<template<typename, MatrixSize, MatrixSize> typename V, typename U, MatrixSize C, MatrixSize R>
    struct IsMatrix<V<U, C, R>, V> : std::true_type
    {
    };

    /**
     * @brief Checks if Type T is a Matrix
     *
     * @tparam T [TODO:tparam]
     */
    template<typename T>
    concept MatrixTypeConcept = IsMatrix<T, Matrix>::value;

    SSSENGINE_STATIC_ASSERT(MatrixTypeConcept<Mat4x4f>, "Mat4x4f is a matrix")
    SSSENGINE_STATIC_ASSERT(!MatrixTypeConcept<int>, "int is not a matrix")

    /**
     * @brief Concept of a Matrix where Columns are the same as Rows
     *
     * @tparam T A type of matrix
     */
    template<typename T>
    concept SquareMatrixConcept = MatrixTypeConcept<T> && T::Rows() == T::Columns();

    SSSENGINE_STATIC_ASSERT((SquareMatrixConcept<Matrix<float, 4, 4>>), "A 4x4 matrix is a square matrix");
    SSSENGINE_STATIC_ASSERT((!SquareMatrixConcept<Matrix<float, 3, 4>>), "A 3x4 matrix is not a square matrix");

    template<MatrixTypeConcept T, MatrixTypeConcept V>
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

    template<MatrixTypeConcept T>
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

    template<MatrixTypeConcept T>
    SSSENGINE_GLOBAL constexpr bool operator!=(const T &lhs, const T &rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * @brief Get a matrix where column == row is set to 1 and 0 otherwise
     *
     * @tparam M A square matrix
     * @return An identity matrix of type M
     */
    template<SquareMatrixConcept M>
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
} // namespace SSSEngine::Math
