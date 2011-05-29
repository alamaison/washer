/**
    @file

    Tests for global lock management class.

    @if license

    Copyright (C) 2011  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#include <winapi/global_lock.hpp> // test subject

#include <boost/shared_ptr.hpp> // shared_ptr
#include <boost/system/system_error.hpp> // system_error
#include <boost/test/unit_test.hpp>
#include <boost/type_traits/remove_pointer.hpp> // remove_pointer

#include <cstring> // memcpy
#include <string>

BOOST_AUTO_TEST_SUITE(global_lock_tests)

namespace {
    std::string test_data = "Mary had a little lamb";

    typedef boost::shared_ptr<boost::remove_pointer<HGLOBAL>::type> hglobal;

    hglobal test_global()
    {
        hglobal global(
            ::GlobalAlloc(GMEM_MOVEABLE, test_data.size()), ::GlobalFree);
        void* p = ::GlobalLock(global.get());
        std::memcpy(p, &test_data[0], test_data.size());
        ::GlobalUnlock(global.get());

        return global;
    }
}

/**
 * Lock test memory.  Should be unlocked automatically at the end of the test.
 */
BOOST_AUTO_TEST_CASE( lock_memory )
{
    hglobal global = test_global();
    winapi::global_lock<char> lock(global.get());
    BOOST_REQUIRE(lock.get());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        test_data.begin(), test_data.end(),
        lock.get(), lock.get() + test_data.size());
}

/**
 * Locking an invalid HGLOBAL should throw.
 */
BOOST_AUTO_TEST_CASE( invalid_lock )
{
    BOOST_CHECK_THROW(
        winapi::global_lock<char>(NULL), boost::system::system_error);
}

BOOST_AUTO_TEST_SUITE_END();
