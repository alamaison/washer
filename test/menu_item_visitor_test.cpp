/**
    @file

    Tests for menu item visitor.

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

#include <winapi/gui/menu/items.hpp>
#include <winapi/gui/menu/menu_item.hpp> // test subject

#include <boost/detail/scoped_enum_emulation.hpp> // BOOST_SCOPED_ENUM
#include <boost/test/unit_test.hpp>

#include <string>

using namespace winapi::gui::menu;
using winapi::test::menu_creator_fixtures;
using winapi::test::menu_ownership_fixtures;

using std::string;

/**
 * These tests only exist to ensure the correct visitor overload gets fired.
 */
BOOST_AUTO_TEST_SUITE( menu_item_visitor_tests )

class test_visitor : public menu_item_visitor<boost::any>
{
public:

    BOOST_SCOPED_ENUM_START(expected_case)
    {
        separator,
        command,
        sub
    };
    BOOST_SCOPED_ENUM_END;

    test_visitor(BOOST_SCOPED_ENUM(expected_case) c) : m_case(c) {}

    boost::any visit(separator_menu_item&)
    {
        BOOST_REQUIRE(m_case == expected_case::separator);
        return string("separator");
    }

    boost::any visit(command_menu_item&)
    {
        BOOST_REQUIRE(m_case == expected_case::command);
        return string("command");
    }

    boost::any visit(sub_menu& item)
    {
        BOOST_REQUIRE(m_case == expected_case::sub);
        return string("sub_menu");
    }

private:

    BOOST_SCOPED_ENUM(expected_case) m_case;
};

BOOST_AUTO_TEST_CASE( visit_separator )
{
    menu m;
    m.insert(separator_description());

    string out = boost::any_cast<string>(
        m[0]->accept(test_visitor(test_visitor::expected_case::separator)));
    BOOST_CHECK_EQUAL(out, "separator");
}

BOOST_AUTO_TEST_CASE_TEMPLATE( visit_command, F, menu_creator_fixtures )
{
    F::menu_type m = F::create_menu_to_test().menu();
    m.insert(command_item_description(string_menu_button(L"Bob"), 42));

    string out = boost::any_cast<string>(
        m[0]->accept(test_visitor(test_visitor::expected_case::command)));
    BOOST_CHECK_EQUAL(out, "command");
}

BOOST_AUTO_TEST_CASE_TEMPLATE( visit_sub_menu, F, menu_creator_fixtures )
{
    F::menu_type m = F::create_menu_to_test().menu();
    menu s;
    s.insert(command_item_description(string_menu_button(L"Pop"), 7));
    m.insert(sub_menu_description(string_menu_button(L"Bob"), s));

    string out = boost::any_cast<string>(
        m[0]->accept(test_visitor(test_visitor::expected_case::sub)));
    BOOST_CHECK_EQUAL(out, "sub_menu");
}

BOOST_AUTO_TEST_SUITE_END()
