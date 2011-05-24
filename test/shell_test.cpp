/**
    @file

    Tests for shell utility functions.

    @if license

    Copyright (C) 2010, 2011  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#include "wchar_output.hpp" // wstring output

#include <winapi/shell/shell.hpp> // test subject

#include <comet/util.h> // auto_coinit

#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>

using comet::auto_coinit;
using comet::com_ptr;

using namespace winapi::shell;
using winapi::shell::pidl::apidl_t;

using boost::filesystem::wpath;

using std::wstring;

BOOST_AUTO_TEST_SUITE(shell_tests)

/**
 * Round-trip STRRET test (narrow in, wide out).
 */
BOOST_AUTO_TEST_CASE( ansi_to_wide )
{
    STRRET strret = string_to_strret("Narrow (ANSI) test string");

    BOOST_CHECK_EQUAL(
        strret_to_string<wchar_t>(strret), L"Narrow (ANSI) test string");
}

/**
 * Round-trip STRRET test (wide in, narrow out)..
 */
BOOST_AUTO_TEST_CASE( wide_to_ansi )
{
    STRRET strret = string_to_strret(L"Wide (Unicode) test string");

    BOOST_CHECK_EQUAL(
        strret_to_string<char>(strret), "Wide (Unicode) test string");
}

BOOST_AUTO_TEST_CASE( desktop_ishellfolder )
{
    auto_coinit com;
    com_ptr<IShellFolder> desktop = desktop_folder();
    BOOST_REQUIRE(desktop);
}

BOOST_AUTO_TEST_CASE( parse_name_from_filesystem_pidl )
{
    auto_coinit com;
    wpath prog_files = special_folder_path<wchar_t>(CSIDL_PROGRAM_FILES);
    apidl_t pidl = pidl_from_parsing_name(prog_files.external_file_string());
    wstring name = parsing_name_from_pidl(pidl);

    BOOST_CHECK_EQUAL(prog_files, name);
}

BOOST_AUTO_TEST_CASE( parse_name_from_virtual_pidl )
{
    auto_coinit com;
    apidl_t pidl = special_folder_pidl(CSIDL_DRIVES);
    wstring name = parsing_name_from_pidl(pidl);

    BOOST_CHECK_EQUAL(L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}", name);
}


BOOST_AUTO_TEST_SUITE_END();
