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
 * @brief Representation of matrices and respective functions
 */

#pragma once

#include "ConversionTraits.h"
#include "HelperMacros.h"
#include "Types.h"
#include "Concepts.h"
#include "Debug.h"
#include "ValueConstant.h"
#include "Utility.h"

namespace SSSEngine::Math
{
    using MatrixSize = u32;

    template<SSSEngine::NumberConcept T, MatrixSize C, MatrixSize R>
    struct Matrix;

    /**
     * @brief Checks if type T is a matrix
     *
     * @tparam T Type to check
     */
    template<typename T>
    struct IsMatrix : FalseType
    {
    };

    /**
     * @brief Checks if is a Matrix
     *
     * @tparam T Type to check
     */
    template<typename T, MatrixSize C, MatrixSize R>
    struct IsMatrix<Matrix<T, C, R>> : TrueType
    {
    };

    /**
     * @brief Checks if Type T is a Matrix
     *
     * @tparam T The type to check
     */
    template<typename T>
    concept MatrixTypeConcept = IsMatrix<T>::Value;

    /**
     * @brief Concept of a Matrix where Columns are the same as Rows
     *
     * @tparam T A type of matrix
     */
    template<typename T>
    concept SquareMatrixConcept = MatrixTypeConcept<T> && T::Rows == T::Columns;

    /**
     * @brief A Matrix representation
     *
     * @tparam MatrixSize C The amount of columns
     * @tparam MatrixSize R The amount of rows
     */
    template<SSSEngine::NumberConcept T, MatrixSize C, MatrixSize R>
    struct Matrix
    {
        static constexpr MatrixSize Rows = R;
        static constexpr MatrixSize Columns = C;
        static constexpr MatrixSize NumberElements = R * C;

        using Type = T;

        T data[NumberElements]{0};

        template<class Self>
        constexpr auto &&operator[](this Self &&self, MatrixSize row, MatrixSize col)
        {
            SSSENGINE_ASSERT(row < R);
            SSSENGINE_ASSERT(col < C);

            return Forward<Self>(self).data[row * C + col];
        }

        template<class Self>
        constexpr auto &&operator[](this Self &&self, MatrixSize index)
        {
            SSSENGINE_ASSERT(index < NumberElements);

            return Forward<Self>(self).data[index];
        }

        SSSENGINE_PURE
        static consteval Matrix Identity() noexcept
            requires SquareMatrixConcept<Matrix>
        {
            static constexpr MatrixSize Size = Matrix::Rows;

            Matrix m;

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
    };

    template<MatrixTypeConcept T, MatrixTypeConcept V>
        requires(IsSameType<T, V>) && (T::Columns == V::Rows)
    SSSENGINE_GLOBAL
    constexpr auto operator*(const T &lhs, const V &rhs)
    {
        using Type = typename T::Type;
        static constexpr MatrixSize RowsLhs = T::Rows;
        static constexpr MatrixSize RowsRhs = V::Rows;
        static constexpr MatrixSize ColumnsRhs = V::Columns;

        Matrix<Type, ColumnsRhs, RowsRhs> result;

        // PERF: Optimize this! Although for most scenarios we should do a simd multiplication. Here we either dont do
        // it or need to do it unaligned. The other question is column-order vs row-order
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
    SSSENGINE_GLOBAL
    constexpr bool operator==(const T &lhs, const T &rhs)
    {
        static constexpr MatrixSize Rows = T::Rows;
        static constexpr MatrixSize Columns = T::Columns;
        static constexpr MatrixSize Size = Rows * Columns;

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
    SSSENGINE_GLOBAL
    constexpr bool operator!=(const T &lhs, const T &rhs)
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
        return M::Identity();
    }

    using Float4x4 = Matrix<float, 4, 4>;

    SSSENGINE_STATIC_ASSERT(MatrixTypeConcept<Float4x4>, "Mat4x4f is a matrix");
    SSSENGINE_STATIC_ASSERT(!MatrixTypeConcept<int>, "int is not a matrix");
    SSSENGINE_STATIC_ASSERT((SquareMatrixConcept<Matrix<float, 4, 4>>), "A 4x4 matrix is a square matrix");
    SSSENGINE_STATIC_ASSERT((!SquareMatrixConcept<Matrix<float, 3, 4>>), "A 3x4 matrix is not a square matrix");
} // namespace SSSEngine::Math
