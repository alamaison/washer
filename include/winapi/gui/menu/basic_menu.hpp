/**
    @file

    Code common to both menus and menu bars.

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

#ifndef WINAPI_GUI_MENU_BASIC_MENU_HPP
#define WINAPI_GUI_MENU_BASIC_MENU_HPP
#pragma once

#include <winapi/gui/menu/detail/item_position.hpp>
#include <winapi/gui/menu/detail/menu_item_iterator.hpp>
#include <winapi/gui/menu/menu_handle.hpp> // menu_handle
#include <winapi/gui/menu/menu_item.hpp>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * A C++ view of a Windows menu or menu bar.
 *
 * Purpose: to implement the common aspects of wrapping a menu and a menu bar.
 */
template<typename DescriptionType, typename HandleCreator, typename Friend>
class basic_menu
{
    typedef DescriptionType description_type;

    friend Friend;

public:

    typedef detail::menu_item_iterator<basic_menu, menu_item>
        iterator;
    typedef detail::menu_item_iterator<basic_menu, const menu_item>
        const_iterator;

    /**
    * Create a new menu.
    */
    explicit basic_menu()
        : m_menu(menu_handle::adopt_handle(HandleCreator()())) {}

    /**
     * Accept an existing menu or menu bar.
     *
     * Purpose: to allow this menu wrapper to be used with menus created by
     *          Windows or by other code that gives us a raw HMENU.
     *
     * @warning  Only pass the a menu handle to this constructor of the same
     *           type as would be created by the handle creator type.
     *           Ideally this would check that the handle passed in is of the
     *           correct type, but there is no way to tell them apart once they
     *           have been created.  
     */
    basic_menu(const menu_handle& handle) : m_menu(handle) {}

    /**
     * Returns the number of items in the menu.
     */
    size_t size() const
    {
        return detail::win32::get_menu_item_count(m_menu.get());
    }

    /**
     * Create an item based on the given description and insert it into the
     * menu at the given iterator position.
     *
     * Shuffles existing items along.
     */
    void insert(const description_type& item, const iterator& position)
    {
        insert_at_position(item, position);
    }

    /**
     * Insert an item at the end of the menu.
     */
    void insert(const description_type& item)
    {
        insert_at_position(item, -1);
    }

    menu_item operator[](size_t position) const
    {
        if (position >= size())
            BOOST_THROW_EXCEPTION(
                std::range_error("Menu item index out of range"));

        return menu_item(detail::item_position(handle(), position));
    }

    iterator begin()
    {
        return iterator(*this);
    }

    iterator end()
    {
        return iterator::end(*this);
    }

    /**
     * Test if objects wrap the same Win32 menu.
     */
    bool operator==(const basic_menu& other) const
    {
        return m_menu.get() == other.m_menu.get();
    }

    /**
     * Test if objects wrap different Win32 menus.
     */
    bool operator!=(const basic_menu& other) const
    {
        return !(*this == other);
    }

    /**
     * Tests if the underlying Win32 menu still exists.
     *
     * Windows take over the lifetime of a menu and destroy the menu when they
     * themselves are destroyed.  Therefore it is possible for this menu to
     * become invalid outside the control of this wrapper.
     */
    bool valid() const
    {
        return detail::win32::is_menu(m_menu.get());
    }

private:

    const menu_handle handle() const
    {
        return m_menu;
    }

    void insert_at_position(const description_type& item, UINT position)
    {
        MENUITEMINFOW info = item.as_menuiteminfo();
        // BUG: this function takes ownership of the submenu HMENU but the
        // passed-in menu will still try to destroy it
        detail::win32::insert_menu_item(m_menu.get(), position, TRUE, &info);
    }

    menu_handle m_menu;
};

}}} // namespace winapi::gui::menu

#endif
