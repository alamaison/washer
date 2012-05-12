/**
    @file

    Tests for shell item classes.

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
#include "sandbox_fixture.hpp" // sandbox_fixture

#include <winapi/shell/shell_item.hpp> // test subject

#include <winapi/shell/shell.hpp> // pidl_from_parsing_name

#include <comet/util.h> // auto_coinit

#include <boost/filesystem/path.hpp> // wpath
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

using comet::auto_coinit;
using comet::com_ptr;

using namespace winapi::shell;
using winapi::test::sandbox_fixture;

using boost::filesystem::wpath;

using std::wstring;

namespace {

    class filesystem_file_fixture : sandbox_fixture
    {
    public:
        filesystem_file_fixture()
            :
        sandbox_fixture(),
        m_file(new_file_in_sandbox(new_directory_in_sandbox(), L".txt")),
        m_shell_item(
            pidl_shell_item(pidl_from_parsing_name(m_file.file_string())))
        {}

        const shell_item& item()
        {
            return m_shell_item;
        }

        const wpath& file()
        {
            return m_file;
        }

    private:
        const wpath m_file;
        pidl_shell_item m_shell_item;
    };

}

BOOST_FIXTURE_TEST_SUITE(
     shell_item_filesystem_file_tests, filesystem_file_fixture)

BOOST_AUTO_TEST_CASE( absolute_parsing_name )
{
    wstring	name = item().parsing_name(shell_item::parsing_name_type::absolute);

    BOOST_CHECK_EQUAL(name, file().file_string());
}

BOOST_AUTO_TEST_CASE( relative_parsing_name )
{
    wstring	name = item().parsing_name(shell_item::parsing_name_type::relative);

    BOOST_CHECK_EQUAL(name, file().filename());
}

BOOST_AUTO_TEST_CASE( global_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::in_global_context);

    BOOST_CHECK_EQUAL(name, file().stem()); // no extension
}

BOOST_AUTO_TEST_CASE( in_folder_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::in_folder_context);

    BOOST_CHECK_EQUAL(name, file().stem()); // no extension
}

BOOST_AUTO_TEST_CASE( editable_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::editable);

    BOOST_CHECK_EQUAL(name, file().stem()); // no extension
}

BOOST_AUTO_TEST_CASE( absolute_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::absolute);

    BOOST_CHECK_EQUAL(name, file().file_string()); // includes extension
}

BOOST_AUTO_TEST_CASE( relative_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::relative);

    BOOST_CHECK_EQUAL(name, file().filename()); // includes extension
}

BOOST_AUTO_TEST_SUITE_END();

namespace {

    class virtual_item_fixture : sandbox_fixture
    {
    public:
        virtual_item_fixture()
            : sandbox_fixture(),
            m_shell_item(pidl_shell_item(special_folder_pidl(CSIDL_PRINTERS)))
        {}

        const shell_item& item()
        {
            return m_shell_item;
        }

    private:
        const auto_coinit m_com;
        pidl_shell_item m_shell_item;
    };

}

BOOST_FIXTURE_TEST_SUITE(
    shell_item_virtual_item_tests, virtual_item_fixture)

BOOST_AUTO_TEST_CASE( absolute_parsing_name )
{
    wstring	name = item().parsing_name(shell_item::parsing_name_type::absolute);

    BOOST_CHECK_EQUAL(
        name,
        L"::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\"
        L"::{2227A280-3AEA-1069-A2DE-08002B30309D}");
}

BOOST_AUTO_TEST_CASE( relative_parsing_name )
{
    wstring	name = item().parsing_name(shell_item::parsing_name_type::relative);

    BOOST_CHECK_EQUAL(name, L"::{2227A280-3AEA-1069-A2DE-08002B30309D}");
}

BOOST_AUTO_TEST_CASE( global_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::in_global_context);

    BOOST_CHECK_EQUAL(name, L"Printers");
}

BOOST_AUTO_TEST_CASE( in_folder_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::in_folder_context);

    BOOST_CHECK_EQUAL(name, L"Printers");
}

BOOST_AUTO_TEST_CASE( editable_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::editable);

    BOOST_CHECK_EQUAL(name, L"Printers");
}

BOOST_AUTO_TEST_CASE( absolute_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::absolute);

    BOOST_CHECK_EQUAL(name, L"All Control Panel Items\\Printers");
}

BOOST_AUTO_TEST_CASE( relative_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::relative);

    BOOST_CHECK_EQUAL(name, L"Printers");
}

BOOST_AUTO_TEST_SUITE_END();

namespace {

    class drive_item_fixture : sandbox_fixture
    {
    public:
        // TODO: Use HOMEDRIVE environment variable
        drive_item_fixture()
            : sandbox_fixture(),
            m_shell_item(pidl_shell_item(pidl_from_parsing_name(L"C:\\")))
        {}

        const shell_item& item()
        {
            return m_shell_item;
        }

    private:
        pidl_shell_item m_shell_item;
    };

}

BOOST_FIXTURE_TEST_SUITE(shell_item_drive_tests, drive_item_fixture)

BOOST_AUTO_TEST_CASE( absolute_parsing_name )
{
    wstring	name = item().parsing_name(shell_item::parsing_name_type::absolute);

    BOOST_CHECK_EQUAL(name, L"C:\\");
}

BOOST_AUTO_TEST_CASE( relative_parsing_name )
{
    wstring	name = item().parsing_name(shell_item::parsing_name_type::relative);

    // For some reason, drives don't get a trailing slash here unlike the
    // absolute name.
    BOOST_CHECK_EQUAL(name, L"C:");
}

BOOST_AUTO_TEST_CASE( global_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::in_global_context);

    BOOST_CHECK_NE(name, L"C:\\");

    // this may fail depending on system
    BOOST_CHECK_EQUAL(name, L"Local Disk (C:)");
}

BOOST_AUTO_TEST_CASE( in_folder_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::in_folder_context);

    BOOST_CHECK_NE(name, L"C:\\");

    // this may fail depending on system
    BOOST_CHECK_EQUAL(name, L"Local Disk (C:)");
}

BOOST_AUTO_TEST_CASE( editable_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::editable);

    BOOST_CHECK_NE(name, L"C:\\");

    // this may fail depending on system
    BOOST_CHECK_EQUAL(name, L"Local Disk");
}

BOOST_AUTO_TEST_CASE( absolute_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::absolute);

    BOOST_CHECK_EQUAL(name, L"C:\\");
}

BOOST_AUTO_TEST_CASE( relative_friendly_name )
{
    wstring	name = item().friendly_name(
        shell_item::friendly_name_type::relative);

    // For some reason, drives don't get a trailing slash here unlike the
    // absolute name.
    BOOST_CHECK_EQUAL(name, L"C:");
}

BOOST_AUTO_TEST_SUITE_END();
