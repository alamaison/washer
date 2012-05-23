/**
    @file

    Menu iterator.

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

#ifndef WINAPI_GUI_DETAIL_MENU_ITEM_ITERATOR_HPP
#define WINAPI_GUI_DETAIL_MENU_ITEM_ITERATOR_HPP
#pragma once

#include <winapi/gui/menu/detail/menu.hpp> // menu_handle
#include <winapi/gui/menu/detail/menu_win32.hpp> // get_menu_item_info
#include <winapi/gui/menu/menu_items.hpp>
             // make_command_menu_item, bitmap_menu_button, owner_drawn_menu_button,
             // separator_menu_item, string_menu_button

#include <boost/iterator/iterator_facade.hpp>
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert
#include <memory> // auto_ptr
#include <string>
#include <vector>

namespace winapi {
namespace gui {
namespace menu {
namespace detail {

inline std::auto_ptr<menu_item> menu_item_from_position(
    const menu_handle& menu, UINT pos)
{
    MENUITEMINFOW info = MENUITEMINFOW();
    info.cbSize = sizeof(MENUITEMINFOW);
    detail::win32::get_menu_item_info(menu.get(), pos, TRUE, &info);

    // MSDN doesn't say that MFT_OWNERDRAWN is part of the
    // mutually exclusive set of flags but I don't see how it couldn't
    // be.  These flags specify the meaning of dwTypeData and cch which
    // MFT_OWNERDRAWN gives a user-defined meaning.
    if (info.fType & MFT_BITMAP)
    {
        HBITMAP bitmap = reinterpret_cast<HBITMAP>(info.dwTypeData);
        return make_command_menu_item(
            bitmap_menu_button(bitmap), info.wID).clone();
    }
    else if (info.fType & MFT_OWNERDRAW)
    {
        return make_command_menu_item(
            owner_drawn_menu_button(), info.wID).clone();
    }
    else if (info.fType & MFT_SEPARATOR)
    {
        assert(info.wID == 0);
        return new separator_menu_item();
    }
    else
    {
        // MFT_STRING which is zero so can only be detected by elimination
        std::vector<wchar_t> buffer(info.cch);
        info.cch = buffer.size();
        detail::win32::get_menu_item_info(menu.get(), pos, TRUE, &info);

        return make_command_menu_item(
            string_menu_button(std::wstring(info.dwTypeData, info.cch)),
            info.wID).clone();
    }
}

template<typename T>
class menu_item_iterator :
    public boost::iterator_facade<
        menu_item_iterator<T>, menu_item, std::random_access_iterator_tag>
{
    friend boost::iterator_core_access;

public:

    explicit menu_item_iterator(const T& menu)
        :
    m_menu(menu), m_pos(0),
    m_current_item(menu_item_from_position(m_menu, m_pos)) {}

private:

    bool equal(menu_item_iterator const& other) const
    {
        return this->m_menu == other.m_menu && this->m_pos == other.m_pos;
    }

    reference dereference() const
    {
        if (m_pos > m_menu.size() - 1)
            BOOST_THROW_EXCEPTION(
                std::logic_error(
                    "Dereferencing past the end of the menu"));

        return *m_current_item;
    }

    void increment()
    {
        if (m_pos > m_menu.size())
            BOOST_THROW_EXCEPTION(
                std::range_error("Cannot increment past end of the menu"));

        ++m_pos;
        m_current_item = menu_item_from_position(m_menu, m_pos);
    }

    const T m_menu;
    size_t m_pos;
    mutable std::auto_ptr<menu_item> m_current_item;
};

}}}} // namespace winapi::gui::menu::detail

#endif
