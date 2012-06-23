/**
    @file

    Tests for description classes building new menu items.

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

#include <boost/test/unit_test.hpp>

using namespace winapi::gui::menu;
using namespace winapi::test;


BOOST_AUTO_TEST_SUITE(menu_item_description_tests)

/**
 * A simple string command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( string_command, F, menu_creator_fixtures )
{
    F::menu_type m = F::create_menu_to_test();

    m.insert(command_item_description(string_menu_button(L"Child command"), 1));

    BOOST_CHECK_EQUAL(m.size(), 1);
    BOOST_CHECK(m.begin() != m.end());
}

/**
 * A bitmap command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( bitmap_command, F, menu_creator_fixtures )
{
    F::menu_type m = F::create_menu_to_test();

    m.insert(command_item_description(bitmap_menu_button(test_bitmap()), 1));

    BOOST_CHECK_EQUAL(m.size(), 1);
    BOOST_CHECK(m.begin() != m.end());
}

/**
 * A string popping out a submenu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( string_submenu, F, menu_creator_fixtures )
{
    F::menu_type m = F::create_menu_to_test();

    menu sub;
    sub.insert(command_item_description(string_menu_button(L"Boo"), 1));

    m.insert(sub_menu_description(string_menu_button(L"Pop"), sub));

    BOOST_CHECK_EQUAL(m.size(), 1);
    BOOST_CHECK(m.begin() != m.end());
}

/**
 * A bitmap popping out a submenu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( bitmap_submenu, F, menu_creator_fixtures )
{
    F::menu_type m = F::create_menu_to_test();

    menu sub;
    sub.insert(command_item_description(string_menu_button(L"Boo"), 1));

    m.insert(sub_menu_description(bitmap_menu_button(test_bitmap()), sub));

    BOOST_CHECK_EQUAL(m.size(), 1);
    BOOST_CHECK(m.begin() != m.end());
}

/**
 * A separator.
 */
BOOST_AUTO_TEST_CASE( separator )
{
    menu m;
    m.insert(separator_description());

    BOOST_CHECK(m.begin() != m.end());
}

/**
 * A menu with multiple items.
 */
BOOST_AUTO_TEST_CASE( mixed_items )
{
    menu m;

    m.insert(command_item_description(string_menu_button(L"String command"), 1));

    m.insert(command_item_description(bitmap_menu_button(test_bitmap()), 2));

    m.insert(separator_description());

    menu sub;

    sub.insert(command_item_description(string_menu_button(L"String sub"), 3));

    sub.insert(separator_description());

    sub.insert(command_item_description(bitmap_menu_button(test_bitmap()), 4));

    m.insert(sub_menu_description(string_menu_button(L"Lalala"), sub));
}

BOOST_AUTO_TEST_SUITE_END();
