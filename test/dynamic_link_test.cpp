/**
    @file

    Tests for dynamic linking and loading functions.

    @if license

    Copyright (C) 2010, 2011, 2013, 2014
    Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#include <washer/dynamic_link.hpp> // test subject

#include <boost/function.hpp> // function
#include <boost/system/system_error.hpp> // system_error

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(dynamic_link_tests)

/**
 * load_library<char> returns hinstance
 */
BOOST_AUTO_TEST_CASE( load_library )
{
    washer::hmodule hinst = washer::load_library("kernel32.dll");
    BOOST_CHECK(hinst);
}

/**
 * load_library<wchar_t> returns hinstance
 */
BOOST_AUTO_TEST_CASE( load_library_w )
{
    washer::hmodule hinst = washer::load_library(L"kernel32.dll");
    BOOST_CHECK(hinst);
}

/**
 * load_library on unknown DLL should throw.
 */
BOOST_AUTO_TEST_CASE( load_library_fail )
{
    BOOST_CHECK_THROW(
        washer::load_library("idontexist.dll"),
        boost::system::system_error);
}

/**
 * load_library on unknown DLL should throw.
 */
BOOST_AUTO_TEST_CASE( load_library_fail_w )
{
    BOOST_CHECK_THROW(
        washer::load_library(L"idontexist.dll"),
        boost::system::system_error);
}

/**
 * module_handle<char> returns hinstance
 */
BOOST_AUTO_TEST_CASE( module_handle )
{
    HMODULE hinst = washer::module_handle("kernel32.dll");
    BOOST_CHECK(hinst);
}

/**
 * module_handle<wchar_t> returns hinstance
 */
BOOST_AUTO_TEST_CASE( module_handle_w )
{
    HMODULE hinst = washer::module_handle(L"kernel32.dll");
    BOOST_CHECK(hinst);
}

/**
 * module_handle on unknown DLL should throw.
 */
BOOST_AUTO_TEST_CASE( module_handle_fail )
{
    BOOST_CHECK_THROW(
        washer::module_handle("idontexist.dll"),
        boost::system::system_error);
}

/**
 * module_handle on unknown DLL should throw.
 */
BOOST_AUTO_TEST_CASE( module_handle_fail_w )
{
    BOOST_CHECK_THROW(
        washer::module_handle(L"idontexist.dll"),
        boost::system::system_error);
}

/**
 * Current module handle must always succeed.
 */
BOOST_AUTO_TEST_CASE( current_module_handle )
{
    HMODULE hinst = washer::module_handle();
    BOOST_CHECK(hinst);
}

//
// The following tests use a custom DLL to test loading functions from a DLL
// by name.  We use our own DLL, rather than a system one, so that we can
// detect if the DLL handle is released too early.  System DLLs are often
// loaded all over the place so the function calls would succeed even if we
// release our handle too soon.
//
// IMPORTANT: For this to work, we must not keep the DLL loaded for longer
// than a single test case.
//

/**
 * Call known function using dynamic binding.
 */
BOOST_AUTO_TEST_CASE( load_function )
{
    boost::function<char*()> func;

    func = washer::load_function<char*()>("load_test_dll.dll", "test_function");
    BOOST_CHECK_EQUAL(func(), "Ran DLL function successfully");
}

/**
 * Call known function using dynamic binding.
 */
BOOST_AUTO_TEST_CASE( load_function_w )
{
    boost::function<char*()> func;

    func = washer::load_function<char*()>(
        L"load_test_dll.dll", "test_function");
    BOOST_CHECK_EQUAL(func(), "Ran DLL function successfully");
}

/**
 * Call known function using dynamic binding, passing an argument.
 *
 * Tests that our signature template handles an argument correctly.
 */
BOOST_AUTO_TEST_CASE( load_unary_function )
{
    boost::function<int(int)> func;

    func = washer::load_function<int(int)>(
        L"load_test_dll.dll", "unary_test_function");
    BOOST_CHECK_EQUAL(func(10), 20);
}

/**
 * Call known function using dynamic binding, passing two arguments.
 *
 * Tests that our signature template handles different number of arguments
 * (not straightforward with C++).
 */
BOOST_AUTO_TEST_CASE( load_binary_function )
{
    boost::function<int(int,int)> func;

    func = washer::load_function<int(int,int)>(
        L"load_test_dll.dll", "binary_test_function");
    BOOST_CHECK_EQUAL(func(7,3), 21);
}

/**
 * Tests that our signature template handles cdecl calling convention.
 */
BOOST_AUTO_TEST_CASE( load_cdecl_function )
{
    boost::function<int(int)> func;

    func = washer::load_function<int __cdecl (int)>(
        L"load_test_dll.dll", "cdecl_test_function");
    BOOST_CHECK_EQUAL(func(10), 30);
}

/**
 * Tests that our signature template handles stdcall calling convention.
 */
BOOST_AUTO_TEST_CASE( load_stdcall_function )
{
    boost::function<int(int)> func;

    func = washer::load_function<int __stdcall (int)>(
        L"load_test_dll.dll", "stdcall_test_function");
    BOOST_CHECK_EQUAL(func(10), 30);
}

/**
 * Tests that our signature template handles fastcall calling convention.
 */
BOOST_AUTO_TEST_CASE( load_fastcall_function )
{
    boost::function<int(int)> func;

    func = washer::load_function<int __fastcall (int)>(
        L"load_test_dll.dll", "fastcall_test_function");
    BOOST_CHECK_EQUAL(func(10), 30);
}

BOOST_AUTO_TEST_SUITE_END();
