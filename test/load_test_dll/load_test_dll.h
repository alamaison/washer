/**
    @file

    Dummy DLL to test dynamic linking.

    @if license

    Copyright (C) 2013, 2014  Alexander Lamaison <awl03@doc.ic.ac.uk>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    If you modify this Program, or any covered work, by linking or
    combining it with the OpenSSL project's OpenSSL library (or a
    modified version of that library), containing parts covered by the
    terms of the OpenSSL or SSLeay licenses, the licensors of this
    Program grant you additional permission to convey the resulting work.

    @endif
*/

#ifndef WINAPI_TEST_LOAD_TEST_DLL_LOAD_TEST_DLL_H
#define WINAPI_TEST_LOAD_TEST_DLL_LOAD_TEST_DLL_H

extern "C"
{

// The functions must also be listed in the DEF file so that
// GetProcAddress can find them without needed a mangled name
// (http://stackoverflow.com/a/597573/67013). (Really only
// stdcall and fastcall seem to need this but that's probably
// compiler-specific)

char* test_function();

int unary_test_function(int);

int binary_test_function(int, int);

int __cdecl cdecl_test_function(int x);

int __stdcall stdcall_test_function(int x);

int __fastcall fastcall_test_function(int x);

}

#endif
