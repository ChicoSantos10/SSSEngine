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

#include "Test.h"

// INVESTIGATE: Should we still differentiate between windows and others? This does not need to be a gui app so we
// don't need winMain so verify that we do not set the compiler option for that
int main()
{
    SSSTest::Test::Execute();
    return SSSTest::Succeeded ? 0 : 1;
}
