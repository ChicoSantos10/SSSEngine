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

// Exports and imports
#define SSSENGINE_DLL_EXPORT extern "C" [[maybe_unused]] __declspec(dllexport)
#define SSSENGINE_DLL_IMPORT __declspec(dllimport)

// Inlining
#ifdef SSSENGINE_ALLOW_INLINE_HINT
#define SSSENGINE_INLINE_HINT inline
#else
#define SSSENGINE_INLINE_HINT
#endif

#ifdef SSSENGINE_ALLOW_FORCE_INLINE
#ifdef SSSENGINE_MSVC
#define SSSENGINE_FORCE_INLINE __forceinline inline
#elif SSSENGINE_MINGW
#define SSSENGINE_FORCE_INLINE [[using gnu : always_inline, inline]] inline
#endif
#else
#define SSSENGINE_FORCE_INLINE SSSENGINE_INLINE_HINT
#endif

#ifdef SSSENGINE_ALLOW_NO_INLINE
#ifdef SSSENGINE_MSVC
#define SSSENGINE_NO_INLINE declspec(noinline)
#endif
#endif

#ifdef SSSENGINE_ALLOW_HINT_INLINE_STATEMENT
#define SSSENGINE_HINT_INLINE_STATEMENT #pragma inline
#define SSSENGINE_HINT_INLINE_STATEMENT_RECURSIVE #pragma inline recursive
#else
#define SSSENGINE_HINT_INLINE_STATEMENT
#define SSSENGINE_HINT_INLINE_STATEMENT_RECURSIVE
#endif

#ifdef SSSENGINE_ALLOW_FORCE_INLINE_STATEMENT
#define SSSENGINE_FORCE_INLINE_STATEMENT #pragma FORCEINLINE
#define SSSENGINE_FORCE_INLINE_STATEMENT_RECURSIVE #pragma FORCEINLINE recursive
#else
#define SSSENGINE_FORCE_INLINE_STATEMENT
#define SSSENGINE_FORCE_INLINE_STATEMENT_RECURSIVE
#endif

// TODO: Regular C++ attributes (e.g. nodiscard, noreturn, assume, likely, unlikely)
#define SSSENGINE_PURE_REASON(reason) [[nodiscard(#reason)]]
#define SSSENGINE_PURE [[nodiscard]]

// TODO: Pragmas

// INVESTIGATE: Check gnu and msvc attributes
