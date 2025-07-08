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

#include "test.h"
#include "Debug.h"
#include <exception>
#include <iostream>

// TODO: What should this be?
void ReportAssertionFailure(const wchar_t *message, const wchar_t *file, unsigned int line)
{
    std::wcerr << L"Assertion failed at file " << file << L" in line " << line << "\n";
}

int main()
{
    SSSTest::Test::Execute();
    return SSSTest::Succeeded ? 0 : 1;
}
