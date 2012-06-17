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

#ifndef WINAPI_GUI_MENU_DETAIL_MENU_COMMON_CORE_HPP
#define WINAPI_GUI_MENU_DETAIL_MENU_COMMON_CORE_HPP
#pragma once

#include <winapi/gui/menu/detail/item_proxy.hpp>
#include <winapi/gui/menu/detail/menu_item_iterator.hpp>
#include <winapi/gui/menu/menu_handle.hpp> // menu_handle

#include <boost/shared_ptr.hpp>

#include <memory> // auto_ptr

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {
namespace detail {

template<typename ItemType>
inline boost::shared_ptr<ItemType> menu_item_from_position(
    const item_proxy& item);

/**
 * Object implementing the common aspects of wrapping a menu and a menu bar.
 *
 * @todo  Add a method that can insert at an iterator position.
 */
template<typename ItemType, typename DescriptionType>
class menu_common_core
{
    typedef ItemType item_type;
    typedef DescriptionType description_type;

public:

    typedef detail::menu_item_iterator<menu_common_core, item_type>
        iterator;
    typedef detail::menu_item_iterator<menu_common_core, const item_type>
        const_iterator;

    explicit menu_common_core(menu_handle menu) : m_menu(menu) {}

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

    boost::shared_ptr<item_type> operator[](size_t position) const
    {
        return detail::menu_item_from_position<item_type>(
            item_proxy(handle(), position));
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
    bool operator==(const menu_common_core& other) const
    {
        return m_menu.get() == other.m_menu.get();
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

protected:

    const menu_handle handle() const
    {
        return m_menu;
    }

private:

    void insert_at_position(const description_type& item, UINT position)
    {
        MENUITEMINFOW info = item.as_menuiteminfo();
        // BUG: this function takes ownership of the submenu HMENU but the
        // passed-in menu will still try to destroy it
        detail::win32::insert_menu_item(m_menu.get(), position, TRUE, &info);
    }

    menu_handle m_menu;
};

}}}} // namespace winapi::gui::menu::detail

#endif
