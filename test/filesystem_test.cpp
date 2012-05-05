/**
    @file

    Tests for file and directory functions.

    @if license

    Copyright (C) 2012  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#include <winapi/filesystem.hpp> // test subject

#include <boost/filesystem/path.hpp> // path, wpath
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

using winapi::filesystem::temporary_directory_path;
using winapi::filesystem::unique_path;

using boost::filesystem::path;
using boost::filesystem::wpath;

BOOST_AUTO_TEST_SUITE(filesystem_tests)

/**
 * Create unique file name.
 */
BOOST_AUTO_TEST_CASE( unique_name )
{
    BOOST_CHECK(!unique_path<char>().empty());
    BOOST_CHECK(!unique_path<wchar_t>().empty());
}

/**
 * Temp directory path.
 */
BOOST_AUTO_TEST_CASE( temp_directory )
{
    BOOST_CHECK(!temporary_directory_path<char>().empty());
    BOOST_CHECK(temporary_directory_path<char>().is_complete());
    BOOST_CHECK(is_directory(temporary_directory_path<char>()));

    BOOST_CHECK(!temporary_directory_path<wchar_t>().empty());
    BOOST_CHECK(temporary_directory_path<wchar_t>().is_complete());
    BOOST_CHECK(is_directory(temporary_directory_path<wchar_t>()));
}


BOOST_AUTO_TEST_SUITE_END();
