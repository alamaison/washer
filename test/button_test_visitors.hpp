/**
    @file

    Visitors used to make assertions about menu item buttons.

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

#ifndef WINAPI_TEST_MENU_BUTTON_TEST_VISITORS_HPP
#define WINAPI_TEST_MENU_BUTTON_TEST_VISITORS_HPP
#pragma once

#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/button/bitmap_button.hpp>
#include <winapi/gui/menu/button/string_button.hpp>
#include <winapi/gui/menu/visitor.hpp>

#include <boost/test/unit_test.hpp>

#include <string>

namespace winapi {
namespace test {

class string_button_test : public winapi::gui::menu::menu_visitor<>
{
public:
    string_button_test(const std::wstring& caption) : m_caption(caption) {}

    void operator()(winapi::gui::menu::string_button& button)
    {
        BOOST_CHECK_EQUAL(button.caption(), m_caption);
    }

    template<typename T>
    void operator()(T&)
    {
        BOOST_FAIL("Unexpected button type");
    }

private:
    std::wstring m_caption;
};

class bitmap_button_test : public winapi::gui::menu::menu_visitor<>
{
public:
    bitmap_button_test(HBITMAP bitmap) : m_bitmap(bitmap) {}

    void operator()(winapi::gui::menu::bitmap_button& button)
    {
        BOOST_CHECK_EQUAL(button.bitmap(), m_bitmap);
    }

    template<typename T>
    void operator()(T&)
    {
        BOOST_FAIL("Unexpected button type");
    }

private:
    HBITMAP m_bitmap;
};

class string_button_mutator : public winapi::gui::menu::menu_visitor<>
{
public:
    string_button_mutator(const std::wstring& caption) : m_caption(caption) {}

    void operator()(winapi::gui::menu::string_button& button)
    {
        button.caption(m_caption);
    }

    template<typename T>
    void operator()(T&)
    {
        BOOST_FAIL("Unexpected button type");
    }

private:
    std::wstring m_caption;
};

class bitmap_button_mutator : public winapi::gui::menu::menu_visitor<>
{
public:
    bitmap_button_mutator(HBITMAP bitmap) : m_bitmap(bitmap) {}

    void operator()(winapi::gui::menu::bitmap_button& button)
    {
        button.bitmap(m_bitmap);
    }

    template<typename T>
    void operator()(T&)
    {
        BOOST_FAIL("Unexpected button type");
    }

private:
    HBITMAP m_bitmap;
};

}}

#endif
