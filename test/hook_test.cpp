/**
    @file

    Tests for window hooking

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

#include <winapi/hook.hpp> // test subject

#include <boost/system/system_error.hpp> // system_error
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(hook_tests)

LRESULT CALLBACK test_keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Win9x doesn't allow NULL as the first argument
    return ::CallNextHookEx(NULL /*hhook*/, nCode, wParam, lParam);
}

/**
 * Install a hook for the current thread.
 * Should be destroyed automatically at end of test.
 */
BOOST_AUTO_TEST_CASE( thread_hook )
{
    winapi::hhook hook = winapi::windows_hook(
        WH_KEYBOARD, test_keyboard_proc);
    BOOST_CHECK(hook);
}

/**
 * Install a global hook.  Should be destroyed automatically at end of test.
 */
BOOST_AUTO_TEST_CASE( global_hook )
{
    winapi::hhook hook = winapi::windows_global_hook(
        WH_KEYBOARD_LL, test_keyboard_proc);
    BOOST_CHECK(hook);
}

/**
 * Try to install a global-only hook as a thread hook.
 * It should throw an exception.
 */
BOOST_AUTO_TEST_CASE( thread_hook_fail )
{
    BOOST_CHECK_THROW(
        winapi::windows_hook(
            WH_KEYBOARD_LL, test_keyboard_proc), boost::system::system_error);
}

BOOST_AUTO_TEST_SUITE_END();
