/**
    @file

    Tests for menu wrappers.

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

#include "menu_fixtures.hpp"
#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/item_descriptions.hpp> // test subject
#include <winapi/gui/menu/items.hpp> // test subject
#include <winapi/gui/menu/menu.hpp> // test subject

#include <boost/foreach.hpp> // BOOST_FOREACH
#include <boost/test/unit_test.hpp>

using namespace winapi::gui::menu;
using winapi::test::menu_fixtures;
using winapi::test::menu_ownership_fixtures;

/**
 * These tests are not especially interested in the details of the items in
 * the menu (except where it may affect the menu's behaviour).  Instead they
 * just care about testing the menu and menu_bar classes.  The details of the
 * items are tested by the menu_item* test suites.
 */
BOOST_AUTO_TEST_SUITE(menu_tests)

/**
 * Create an empty menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( create_empty_menu, F, menu_ownership_fixtures )
{
    menu m;

    BOOST_CHECK(m.begin() == m.end());
    BOOST_CHECK_EQUAL(m.size(), 0);
    BOOST_CHECK_THROW(m[0], std::runtime_error);
    BOOST_FOREACH(menu_item& item, m)
    {
        BOOST_FAIL("Empty menu should not iterate");
    }

    F::do_test(m);
}


/**
 * Test menu copying.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( menu_copy, F, menu_ownership_fixtures )
{
    menu m;

    // Copy constructor
    menu n = m;
    BOOST_CHECK(m == n);

    // Copy assignment
    menu p;
    n = p;
    BOOST_CHECK(n == p);
    BOOST_CHECK(n != m);
}

/**
 * Create an empty menu bar.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( empty_menu_bar, F, menu_ownership_fixtures )
{
    menu_bar m;

    BOOST_CHECK(m.begin() == m.end());
    BOOST_CHECK_EQUAL(m.size(), 0);
    BOOST_CHECK_THROW(m[0], std::runtime_error);
    BOOST_FOREACH(menu_item& item, m)
    {
        BOOST_FAIL("Empty menu should not iterate");
    }

    F::do_test(m);
}

/**
 * Wrap an empty menu(bar).
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( existing_empty_menu, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    BOOST_CHECK(m.begin() == m.end());
    BOOST_CHECK_EQUAL(m.size(), 0);
    BOOST_CHECK_THROW(m[0], std::runtime_error);
    BOOST_FOREACH(menu_item& item, m)
    {
        BOOST_FAIL("Empty menu should not iterate");
    }

    F::do_test(m);
}

/**
 * Insert a non-popup (command) item in a menu(bar).
 *
 * Tests that the insert() method creates an item in the menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( create_command, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    m.insert(command_item_description(string_menu_button(L"Command"), 42));

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK_EQUAL(m.size(), 1);
    BOOST_CHECK(m[0]);
    BOOST_CHECK_THROW(m[1], std::runtime_error);

    F::do_test(m);
}

/**
 * Wrap a menu(bar) with an existing non-popup (command) item.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( existing_command, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    winapi::test::detail::do_insertion(
        t.handle().get(), L"C", 42, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK_EQUAL(m.size(), 1);
    BOOST_CHECK(m[0]);
    BOOST_CHECK_THROW(m[1], std::runtime_error);

    F::do_test(m);
}

/**
 * Create a popup item in a menu(bar).
 *
 * Tests that the insert() method creates an item in the menu and that the
 * submenu doesn't affect iteration in the parent menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( create_submenu, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    menu sub;
    sub.insert(command_item_description(string_menu_button(L"Boo"), 1));

    m.insert(sub_menu_description(string_menu_button(L"Pop"), sub));

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK_EQUAL(m.size(), 1);
    BOOST_CHECK(m[0]);
    BOOST_CHECK_THROW(m[1], std::runtime_error);

    F::do_test(m);
}

/**
 * Test iterator copying.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( iterator_copy, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    // Copy constructor
    F::menu_type::iterator start = m.begin();

    // Copy conversion constructor
    F::menu_type::const_iterator const_start = m.begin();

    BOOST_CHECK(start == const_start);

    // Copy assignment
    start = m.begin();

    // Copy conversion assignment
    const_start = m.begin();

    BOOST_CHECK(start == const_start);
}

/**
 * Test iterator forward-traversal.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( iterator_increment, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    winapi::test::detail::do_insertion(
        t.handle().get(), L"C", 42, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);
    winapi::test::detail::do_insertion(
        t.handle().get(), L"D", 43, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);

    F::menu_type::iterator it = m.begin();

    BOOST_CHECK(it != m.end());

    it++;

    BOOST_CHECK(it != m.begin());
    BOOST_CHECK(it != m.end());

    it++;

    BOOST_CHECK(it == m.end());

    BOOST_CHECK_THROW(++it, std::runtime_error);
}

/**
 * Test iterator backward-traversal.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( iterator_decrement, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    winapi::test::detail::do_insertion(
        t.handle().get(), L"C", 42, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);
    winapi::test::detail::do_insertion(
        t.handle().get(), L"D", 43, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);

    F::menu_type::iterator it = m.end();

    BOOST_CHECK(it != m.begin());

    it--;

    BOOST_CHECK(it != m.begin());
    BOOST_CHECK(it != m.end());

    it--;

    BOOST_CHECK(it == m.begin());

    BOOST_CHECK_THROW(--it, std::runtime_error);
}

/**
 * Test iterator forward skip.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( iterator_advance, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    winapi::test::detail::do_insertion(
        t.handle().get(), L"C", 42, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);
    winapi::test::detail::do_insertion(
        t.handle().get(), L"D", 43, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);

    F::menu_type::iterator it = m.begin();

    BOOST_CHECK(it != m.end());

    it = it + 2;

    BOOST_CHECK(it == m.end());

    BOOST_CHECK_THROW(it + 1, std::runtime_error);
    BOOST_CHECK_THROW(it - 3, std::runtime_error);
}

/**
 * Test iterator reverse skip.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( iterator_reverse, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    winapi::test::detail::do_insertion(
        t.handle().get(), L"C", 42, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);
    winapi::test::detail::do_insertion(
        t.handle().get(), L"D", 43, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);

    F::menu_type::iterator it = m.end();

    BOOST_CHECK(it != m.begin());

    it = it - 2;

    BOOST_CHECK(it == m.begin());

    BOOST_CHECK_THROW(it - 1, std::runtime_error);
    BOOST_CHECK_THROW(it + 3, std::runtime_error);
}

/**
 * Test iterator forward skip.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( iterator_distance, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    winapi::test::detail::do_insertion(
        t.handle().get(), L"C", 42, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);
    winapi::test::detail::do_insertion(
        t.handle().get(), L"D", 43, NULL, MIIM_ID | MIIM_STRING, MFT_STRING);

    F::menu_type::iterator it = m.begin();

    BOOST_CHECK_EQUAL(m.begin() - m.begin(), 0);
    BOOST_CHECK_EQUAL(m.begin() - m.end(), -2);
    BOOST_CHECK_EQUAL(m.end() - m.begin(), 2);
}

BOOST_AUTO_TEST_SUITE_END()

