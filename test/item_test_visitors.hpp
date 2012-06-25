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

#include "menu_fixtures.hpp" // test_bitmap
#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/buttons.hpp>
#include <winapi/gui/menu/items.hpp>
#include <winapi/gui/menu/menu.hpp> // menu
#include <winapi/gui/menu/menu_item.hpp> // menu_item_visitor

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

namespace winapi {
namespace test {

class selectable_state_test : public winapi::gui::menu::menu_item_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_menu_item& item)
    {
        BOOST_CHECK(item.is_enabled());
        BOOST_CHECK(!item.is_default());
        BOOST_CHECK(!item.is_highlighted());
        BOOST_CHECK(!item.is_checked());
    }
};

class selectability_test : public winapi::gui::menu::menu_item_visitor<>
{
public:

    selectability_test(bool expected) : m_expected(expected) {}

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_menu_item& item)
    {
        BOOST_CHECK_EQUAL(item.is_enabled(), m_expected);
    }

private:
    bool m_expected;
};

class checkedness_test : public winapi::gui::menu::menu_item_visitor<>
{
public:

    checkedness_test(bool expected) : m_expected(expected) {}

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::selectable_menu_item& item)
    {
        BOOST_CHECK_EQUAL(item.is_checked(), m_expected);
    }

private:
    bool m_expected;
};

class is_separator_test : public winapi::gui::menu::menu_item_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
    }

    void operator()(winapi::gui::menu::command_menu_item&)
    {
        BOOST_FAIL("Command item unexpected");
    }

    void operator()(winapi::gui::menu::sub_menu_item&)
    {
        BOOST_FAIL("Sub-menu unexpected");
    }
};

class is_command_test : public winapi::gui::menu::menu_item_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::command_menu_item&)
    {
    }

    void operator()(winapi::gui::menu::sub_menu_item&)
    {
        BOOST_FAIL("Sub-menu unexpected");
    }
};

class is_sub_menu_test : public winapi::gui::menu::menu_item_visitor<>
{
public:

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
        BOOST_FAIL("Separator unexpected");
    }

    void operator()(winapi::gui::menu::command_menu_item&)
    {
        BOOST_FAIL("Command item unexpected");
    }

    void operator()(winapi::gui::menu::sub_menu_item&)
    {
    }
};

class command_id_test : public winapi::gui::menu::menu_item_visitor<>
{
public:
    command_id_test(UINT command_id) : m_command_id(command_id) {}

    void operator()(winapi::gui::menu::command_menu_item& item)
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

class sub_menu_test : public winapi::gui::menu::menu_item_visitor<>
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

class string_button_test : public winapi::gui::menu::menu_item_visitor<>
{
public:
    string_button_test(const std::wstring& caption) : m_caption(caption) {}

    void operator()(winapi::gui::menu::selectable_menu_item& item)
    {
        boost::shared_ptr<winapi::gui::menu::string_menu_button> button =
            boost::dynamic_pointer_cast<winapi::gui::menu::string_menu_button>(
                item.button());

        BOOST_CHECK_EQUAL(button->caption(), m_caption);
    }

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
        BOOST_FAIL("Unexpected separator");
    }

private:
    std::wstring m_caption;
};

class bitmap_button_test : public winapi::gui::menu::menu_item_visitor<>
{
public:
    bitmap_button_test(HBITMAP bitmap) : m_bitmap(bitmap) {}

    void operator()(winapi::gui::menu::selectable_menu_item& item)
    {
        boost::shared_ptr<winapi::gui::menu::bitmap_menu_button> button =
            boost::dynamic_pointer_cast<winapi::gui::menu::bitmap_menu_button>(
                item.button());

        BOOST_CHECK_EQUAL(button->bitmap(), m_bitmap);
    }

    void operator()(winapi::gui::menu::separator_menu_item&)
    {
        BOOST_FAIL("Unexpected separator");
    }

private:
    HBITMAP m_bitmap;
};

}}

#endif
