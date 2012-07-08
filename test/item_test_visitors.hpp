/**
    @file

    Visitors used to make assertions about menu items.

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

#ifndef WINAPI_TEST_MENU_ITEM_TEST_VISITORS_HPP
#define WINAPI_TEST_MENU_ITEM_TEST_VISITORS_HPP
#pragma once

#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/item/command_item.hpp>
#include <winapi/gui/menu/item/separator_item.hpp>
#include <winapi/gui/menu/item/sub_menu_item.hpp>
#include <winapi/gui/menu/menu.hpp> // menu
#include <winapi/gui/menu/visitor.hpp> // menu_visitor

#include <boost/test/unit_test.hpp>

#include <string>

namespace winapi {
namespace test {

class selectable_state_test : public winapi::gui::menu::menu_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_item& item)
    {
        BOOST_CHECK(item.is_selectable());
        BOOST_CHECK(!item.is_highlighted());
        BOOST_CHECK(!item.check_mark_is_visible());
    }
};

class selectability_test : public winapi::gui::menu::menu_visitor<>
{
public:

    selectability_test(bool expected) : m_expected(expected) {}

    void operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_item& item)
    {
        BOOST_CHECK_EQUAL(item.is_selectable(), m_expected);
    }

private:
    bool m_expected;
};

class selectability_mutator : public winapi::gui::menu::menu_visitor<>
{
public:

    selectability_mutator(
        BOOST_SCOPED_ENUM(winapi::gui::menu::selectability) state)
        : m_selectability(state) {}

    void operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_item& item)
    {
        item.selectability(m_selectability);
    }

private:
    BOOST_SCOPED_ENUM(winapi::gui::menu::selectability) m_selectability;
};

class checkedness_test : public winapi::gui::menu::menu_visitor<>
{
public:

    checkedness_test(bool expected) : m_expected(expected) {}

    void operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_item& item)
    {
        BOOST_CHECK_EQUAL(item.check_mark_is_visible(), m_expected);
    }

private:
    bool m_expected;
};

class check_mutator : public winapi::gui::menu::menu_visitor<>
{
public:

    check_mutator(BOOST_SCOPED_ENUM(winapi::gui::menu::check_mark) state)
        : m_checkedness(state) {}

    void operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_item& item)
    {
        item.check_mark_visibility(m_checkedness);
    }

private:
    BOOST_SCOPED_ENUM(winapi::gui::menu::check_mark) m_checkedness;
};

class is_separator_test : public winapi::gui::menu::menu_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_item&)
    {
    }

    void operator()(winapi::gui::menu::command_item&)
    {
        BOOST_FAIL("Command item unexpected");
    }

    void operator()(winapi::gui::menu::sub_menu_item&)
    {
        BOOST_FAIL("Sub-menu unexpected");
    }
};

class is_command_test : public winapi::gui::menu::menu_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::command_item&)
    {
    }

    void operator()(winapi::gui::menu::sub_menu_item&)
    {
        BOOST_FAIL("Sub-menu unexpected");
    }
};

class is_sub_menu_test : public winapi::gui::menu::menu_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::command_item&)
    {
        BOOST_FAIL("Command item unexpected");
    }

    void operator()(winapi::gui::menu::sub_menu_item&)
    {
    }
};

class command_id_test : public winapi::gui::menu::menu_visitor<>
{
public:
    command_id_test(UINT command_id) : m_command_id(command_id) {}

    void operator()(winapi::gui::menu::command_item& item)
    {
        BOOST_CHECK_EQUAL(item.command_id(), m_command_id);
    }

    template<typename T>
    void operator()(T&)
    {
        BOOST_FAIL("Unexpected");
    }

private:
    UINT m_command_id;
};

class sub_menu_test : public winapi::gui::menu::menu_visitor<>
{
public:

    sub_menu_test(const winapi::gui::menu::menu& menu) : m_menu(menu) {}

    template<typename T>
    void operator()(T&)
    {
        BOOST_FAIL("Unexpected");
    }

    void operator()(winapi::gui::menu::sub_menu_item& item)
    {
        winapi::gui::menu::menu& submenu = item.menu();
        BOOST_CHECK(submenu.valid());

        BOOST_CHECK(submenu == m_menu);
    }

private:
    winapi::gui::menu::menu m_menu;
};

template<typename ButtonVisitor>
class button_test :
    public winapi::gui::menu::menu_visitor<typename ButtonVisitor::result_type>
{
public:

    button_test(ButtonVisitor test) : m_test(test) {}

    result_type operator()(winapi::gui::menu::separator_item&)
    {
        BOOST_FAIL("Separator unexpected");
        return result_type();
    }

    result_type operator()(winapi::gui::menu::selectable_item& item)
    {
        return item.button().accept(m_test);
    }

private:
    ButtonVisitor m_test;
};

template<typename ButtonVisitor>
inline button_test<ButtonVisitor> make_button_test(ButtonVisitor test)
{
    return button_test<ButtonVisitor>(test);
}

}}

#endif
