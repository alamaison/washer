/**
    @file

    Tests for menu button visitor.

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

#include "item_test_visitors.hpp" // button_test
#include "menu_fixtures.hpp" // test_bitmap
#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/button/bitmap_button.hpp>
#include <winapi/gui/menu/button/bitmap_button_description.hpp>
#include <winapi/gui/menu/button/owner_drawn_button.hpp>
#include <winapi/gui/menu/button/owner_drawn_button_description.hpp>
#include <winapi/gui/menu/button/string_button.hpp>
#include <winapi/gui/menu/button/string_button_description.hpp>
#include <winapi/gui/menu/item/command_item_description.hpp>
#include <winapi/gui/menu/item/separator_item.hpp>
#include <winapi/gui/menu/visitor.hpp> // test subject

#include <boost/detail/scoped_enum_emulation.hpp> // BOOST_SCOPED_ENUM
#include <boost/test/unit_test.hpp>

#include <string>

using namespace winapi::gui::menu;
using namespace winapi::test;

using std::string;

/**
 * These tests only exist to ensure the correct visitor overload gets fired.
 */
BOOST_AUTO_TEST_SUITE( menu_button_visitor_tests )

BOOST_SCOPED_ENUM_START(expected_case)
{
    bitmap,
    owner,
    string
};
BOOST_SCOPED_ENUM_END;

class test_visitor : public menu_visitor<string>
{
public:

    test_visitor(BOOST_SCOPED_ENUM(expected_case) c) : m_case(c) {}

    string operator()(bitmap_button&)
    {
        BOOST_REQUIRE(m_case == expected_case::bitmap);
        return "bitmap";
    }

    string operator()(owner_drawn_button&)
    {
        BOOST_REQUIRE(m_case == expected_case::owner);
        return "owner";
    }

    string operator()(string_button& item)
    {
        BOOST_REQUIRE(m_case == expected_case::string);
        return "string";
    }

private:

    BOOST_SCOPED_ENUM(expected_case) m_case;
};

class void_test_visitor : public menu_visitor<>
{
public:

    void_test_visitor(BOOST_SCOPED_ENUM(expected_case) c) : m_case(c) {}

    void operator()(bitmap_button&)
    {
        BOOST_REQUIRE(m_case == expected_case::bitmap);
    }

    void operator()(owner_drawn_button&)
    {
        BOOST_REQUIRE(m_case == expected_case::owner);
    }

    void operator()(string_button& item)
    {
        BOOST_REQUIRE(m_case == expected_case::string);
    }

private:

    BOOST_SCOPED_ENUM(expected_case) m_case;
};

class templated_test_visitor : public menu_visitor<string>
{
public:

    template<typename T>
    string operator()(T&)
    {
        return "catch-all";
    }
};

/**
 * Tests that we can catch one type specifically but others generically.
 */
class lopsided_test_visitor : public menu_visitor<string>
{
public:

    lopsided_test_visitor(BOOST_SCOPED_ENUM(expected_case) c)
        : m_case(c) {}

    string operator()(bitmap_button&)
    {
        BOOST_REQUIRE(m_case == expected_case::bitmap);
        return "bitmap";
    }

    template<typename T>
    string operator()(T&)
    {
        BOOST_REQUIRE(
            m_case == expected_case::owner || m_case == expected_case::string);
        return "owner-or-string";
    }

private:

    BOOST_SCOPED_ENUM(expected_case) m_case;
};

BOOST_AUTO_TEST_CASE( visit_bitmap )
{
    menu m;
    m.insert(
        command_item_description(bitmap_button_description(test_bitmap()), 99));

    BOOST_CHECK_EQUAL(
        m[0].accept(make_button_test(test_visitor(expected_case::bitmap))),
        "bitmap");

    m[0].accept(make_button_test(void_test_visitor(expected_case::bitmap)));

    BOOST_CHECK_EQUAL(
        m[0].accept(make_button_test(templated_test_visitor())), "catch-all");

    BOOST_CHECK_EQUAL(
        m[0].accept(
            make_button_test(lopsided_test_visitor(expected_case::bitmap))),
        "bitmap");
}

BOOST_AUTO_TEST_CASE( visit_owner_drawn )
{
    menu m;
    m.insert(command_item_description(owner_drawn_button_description(), 42));

    BOOST_CHECK_EQUAL(
        m[0].accept(make_button_test(test_visitor(expected_case::owner))),
        "owner");

    m[0].accept(make_button_test(void_test_visitor(expected_case::owner)));

    BOOST_CHECK_EQUAL(m[0].accept(
        make_button_test(templated_test_visitor())), "catch-all");

    BOOST_CHECK_EQUAL(
        m[0].accept(make_button_test(lopsided_test_visitor(expected_case::owner))),
        "owner-or-string");
}

BOOST_AUTO_TEST_CASE( visit_string )
{
    menu m;
    m.insert(command_item_description(string_button_description(L"Bob"), 42));

    BOOST_CHECK_EQUAL(
        m[0].accept(make_button_test(test_visitor(expected_case::string))),
        "string");

    m[0].accept(make_button_test(void_test_visitor(expected_case::string)));

    BOOST_CHECK_EQUAL(
        m[0].accept(make_button_test(templated_test_visitor())), "catch-all");

    BOOST_CHECK_EQUAL(
        m[0].accept(make_button_test(lopsided_test_visitor(expected_case::string))),
        "owner-or-string");
}

BOOST_AUTO_TEST_SUITE_END()
