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

#ifndef WASHER_TEST_SANDBOX_FIXTURE_HPP
#define WASHER_TEST_SANDBOX_FIXTURE_HPP
#pragma once

#include <washer/filesystem.hpp> // temporary_directory_path, unique_path

#include <boost/filesystem.hpp> // wpath
#include <boost/filesystem/fstream.hpp> // wofstream
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp> // BOOST_REQUIRE etc.

#include <cstdio> // _wtempnam
#include <string>
#include <vector>

namespace washer {
namespace test {

namespace detail {

    const std::wstring SANDBOX_NAME = L"washer-test-sandbox";

    /**
     * Return the path to the sandbox directory.
     */
    inline boost::filesystem::wpath sandbox_directory()
    {
        boost::shared_ptr<wchar_t> name(
            _wtempnam(NULL, SANDBOX_NAME.c_str()), free);
        BOOST_REQUIRE(name);

        return name.get();
    }

    inline bool is_child_path(
        const boost::filesystem::wpath& parent,
        const boost::filesystem::wpath& child)
    {
        boost::filesystem::wpath lhs = complete(parent);
        boost::filesystem::wpath rhs = complete(child);

        boost::filesystem::wpath::iterator left(lhs.begin());
        boost::filesystem::wpath::iterator right(rhs.begin());

        for (;*left == *right && left != lhs.end() && right != rhs.end();
            ++left, ++right);

        return left == lhs.end();
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
    boost::filesystem::wpath new_file_in_sandbox(
        const boost::filesystem::wpath& subdirectory,
        const std::wstring& extension)
    {
        BOOST_REQUIRE_MESSAGE(
            detail::is_child_path(sandbox(), subdirectory),
            "Test file must not stray outside sandbox");

        boost::filesystem::wpath filename =
            washer::filesystem::unique_path<wchar_t>();
        boost::filesystem::wpath p =
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION > 2
            subdirectory / (filename.wstring() + extension);
#else
            subdirectory / (filename.string() + extension);
#endif

        BOOST_REQUIRE(!exists(p));
        BOOST_REQUIRE(p.is_complete());

        boost::filesystem::wofstream s(p);

        BOOST_REQUIRE(exists(p));
        BOOST_REQUIRE(is_regular_file(p));

        return p;
    }

    boost::filesystem::wpath new_file_in_sandbox()
    {
        return new_file_in_sandbox(sandbox(), std::wstring());
    }

    /**
     * Create a new empty directory in the fixture sandbox with a random name
     * and return the path.
     */
    boost::filesystem::wpath new_directory_in_sandbox(
        const boost::filesystem::wpath& subdirectory)
    {
        BOOST_REQUIRE_MESSAGE(
            detail::is_child_path(sandbox(), subdirectory),
            "Test directory must not stray outside sandbox");

        boost::filesystem::wpath p =
            subdirectory / washer::filesystem::unique_path<wchar_t>();

        create_directory(p);

        BOOST_CHECK(exists(p));
        BOOST_CHECK(is_directory(p));
        BOOST_CHECK(p.is_complete());
        return p;
    }

    boost::filesystem::wpath new_directory_in_sandbox()
    {
        return new_directory_in_sandbox(sandbox());
    }

private:
    boost::filesystem::wpath m_sandbox;
};

}} // namespace washer::test

#endif
