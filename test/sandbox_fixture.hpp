/**
    @file

    Fixture creating a temporary sandbox directory.

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

#ifndef WINAPI_TEST_SANDBOX_FIXTURE_HPP
#define WINAPI_TEST_SANDBOX_FIXTURE_HPP
#pragma once

#include <winapi/filesystem.hpp> // temporary_directory_path, unique_path

#include <boost/filesystem.hpp> // wpath
#include <boost/filesystem/fstream.hpp> // wofstream
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp> // BOOST_REQUIRE etc.

#include <cstdio> // _wtempnam
#include <string>
#include <vector>

namespace winapi {
namespace test {

namespace detail {

    const std::wstring SANDBOX_NAME = L"winapi-test-sandbox";

    /**
     * Return the path to the sandbox directory.
     */
    boost::filesystem::wpath sandbox_directory()
    {
        boost::shared_ptr<wchar_t> name(
            _wtempnam(NULL, SANDBOX_NAME.c_str()), free);
        BOOST_REQUIRE(name);
        
        return name.get();
    }
}

/**
 * Fixture that creates and destroys a sandbox directory.
 */
class sandbox_fixture
{
public:
    sandbox_fixture() : m_sandbox(detail::sandbox_directory())
    {
        create_directory(m_sandbox);
    }

    ~sandbox_fixture()
    {
        try
        {
            remove_all(m_sandbox);
        }
        catch (...) {}
    }

    boost::filesystem::wpath sandbox()
    {
        return m_sandbox;
    }

    /**
     * Create a new empty file in the fixture sandbox with a random name
     * and return the path.
     */
    boost::filesystem::wpath new_file_in_sandbox()
    {
        boost::filesystem::wpath p =
            sandbox() / winapi::filesystem::unique_path<wchar_t>();

        boost::filesystem::wofstream s(p);

        BOOST_CHECK(exists(p));
        BOOST_CHECK(is_regular_file(p));
        BOOST_CHECK(p.is_complete());
        return p;
    }


private:
    boost::filesystem::wpath m_sandbox;
};

}} // namespace winapi::test

#endif
