/**
    @file

    Tests for shell utility functions.

    @if license

    Copyright (C) 2010, 2011, 2012  Alexander Lamaison <awl03@doc.ic.ac.uk>

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
#include "sandbox_fixture.hpp" // sandbox_fixture

#include <winapi/shell/shell.hpp> // test subject

#include <comet/ptr.h> // com_ptr
#include <comet/util.h> // auto_coinit

#include <boost/filesystem/path.hpp> // wpath
#include <boost/filesystem/fstream.hpp> // ofstream
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

using comet::auto_coinit;
using comet::com_ptr;

using namespace winapi::shell;
using winapi::shell::pidl::apidl_t;
using winapi::test::sandbox_fixture;

using boost::filesystem::ofstream;
using boost::filesystem::wpath;
using boost::test_tools::predicate_result;

using std::string;
using std::wstring;
using std::vector;

namespace { // private

    /**
     * Check that a PIDL and a filesystem path refer to the same item.
     */
    predicate_result pidl_path_equivalence(apidl_t pidl, wpath path)
    {
        wstring parsing_name = parsing_name_from_pidl(pidl);

        if (path != parsing_name)
        {
            predicate_result res(false);
            res.message()
                << "Different items [" << parsing_name
                << " != " << path.file_string() << "]";
            return res;
        }

        return true;
    }

}

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

BOOST_FIXTURE_TEST_SUITE(file_binding_tests, sandbox_fixture)

BOOST_AUTO_TEST_CASE( stream_from_file_pidl )
{
    wpath test_file_path = new_file_in_sandbox();

    string test_string = "Mary had a little lamb";

    ofstream test_file(test_file_path);
    test_file << test_string;
    test_file.close();

    com_ptr<IStream> stream = stream_from_pidl(
        pidl_from_parsing_name(test_file_path.file_string()));

    vector<char> stream_output(100); // bigger than test input
    ULONG bytes_read = 0;
    HRESULT hr = stream->Read(
        &stream_output[0], (ULONG)stream_output.size(), &bytes_read);
    BOOST_REQUIRE_EQUAL(hr, S_FALSE); // should be short read
    // shrink vector to size read
    stream_output.swap(
        vector<char>(
            stream_output.begin(), stream_output.begin() + bytes_read));

    BOOST_CHECK_EQUAL_COLLECTIONS(
        stream_output.begin(), stream_output.end(),
        test_string.begin(), test_string.end());
}

/**
 * Ask for the IShellFolder handler of the sandbox folder. Check that the
 * enumeration of this folder has the expected contents.
 *
 * Tests bind_to_handler_object().
 */
BOOST_AUTO_TEST_CASE( handler_object )
{
    wpath file = new_file_in_sandbox();

    apidl_t sandbox_pidl = pidl_from_parsing_name(sandbox().directory_string());
    com_ptr<IShellFolder> folder = bind_to_handler_object<IShellFolder>(
        sandbox_pidl);

    com_ptr<IEnumIDList> enum_items;
    HRESULT hr = folder->EnumObjects(
        NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, enum_items.out());
    BOOST_REQUIRE_EQUAL(hr, S_OK);

    enum_items->Reset();

    PITEMID_CHILD child_pidl;
    hr = enum_items->Next(1, &child_pidl, NULL);
    BOOST_REQUIRE_EQUAL(hr, S_OK);

    apidl_t pidl = sandbox_pidl + child_pidl;

    BOOST_REQUIRE(pidl_path_equivalence(pidl, file));
    BOOST_REQUIRE_NE(enum_items->Next(1, &child_pidl, NULL), S_OK);
}

BOOST_AUTO_TEST_SUITE_END();

/**
 * Ask for an IShellFolder handler using a NULL PIDL.  This should return
 * the handler of the Desktop folder.
 *
 * Tests bind_to_handler_object().
 *
 * This relies on desktop_folder returning the same object.  A better way
 * would be to make use of IPersistFolder to get its identity.
 */
BOOST_AUTO_TEST_CASE( handler_object_null_pidl )
{
    auto_coinit com; // to make desktop folder return same object
    com_ptr<IShellFolder> desktop = desktop_folder();
    com_ptr<IShellFolder> folder = bind_to_handler_object<IShellFolder>(NULL);

    BOOST_REQUIRE(folder == desktop);
}

/**
 * Ask for an IShellFolder handler using an empty PIDL.  This should return
 * the handler of the Desktop folder.
 *
 * Tests bind_to_handler_object().
 *
 * This relies on desktop_folder returning the same object.  A better way
 * would be to make use of IPersistFolder to get its identity.
 */
BOOST_AUTO_TEST_CASE( handler_object_empty_pidl )
{
    auto_coinit com; // to make desktop folder return same object
    com_ptr<IShellFolder> desktop = desktop_folder();
    SHITEMID empty = {0, {0}};
    PCIDLIST_ABSOLUTE pidl = reinterpret_cast<PCIDLIST_ABSOLUTE>(&empty);

    com_ptr<IShellFolder> folder = bind_to_handler_object<IShellFolder>(pidl);

    BOOST_REQUIRE(folder == desktop);
}

BOOST_AUTO_TEST_SUITE_END();
