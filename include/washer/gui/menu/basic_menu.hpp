/**
    @file

    Code common to both menus and menu bars.

    @if license

    Copyright (C) 2012, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WASHER_GUI_MENU_BASIC_MENU_HPP
#define WASHER_GUI_MENU_BASIC_MENU_HPP
#pragma once

#include <washer/gui/menu/detail/item_position.hpp>
#include <washer/gui/menu/detail/item_iterator.hpp>
#include <washer/gui/menu/menu_handle.hpp> // menu_handle
#include <washer/gui/menu/item/item.hpp>

#include <boost/integer_traits.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/numeric/conversion/cast.hpp> // numeric_cast
#include <boost/utility/swap.hpp>

#include <algorithm> // find_if

#include <Windows.h> // MENUITEMINFO

namespace washer {
namespace window {

template<typename> class window;

}

namespace gui {
namespace menu {

/**
 * A C++ view of a Windows menu or menu bar.
 *
 * Purpose: to implement the common aspects of wrapping a menu and a menu bar.
 */
template<typename DescriptionType, typename HandleCreator>
class basic_menu
{
    typedef DescriptionType description_type;

#if defined (_MSC_VER) && (_MSC_VER > 1400)
    template<typename>
    friend class washer::window::window;
#else
    // HACK to workaround VC8 (2005) and presumably earlier that can't
    // befriend a template class in a parent namespace.
    // See: http://stackoverflow.com/q/10694416/67013
    friend class washer::window::window<char>;
    friend class washer::window::window<wchar_t>;
#endif

    friend class sub_menu_item_description;

public:

    typedef item value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef detail::item_iterator<basic_menu, value_type>
        iterator;
    typedef detail::item_iterator<basic_menu, const value_type>
        const_iterator;
    typedef boost::reverse_iterator<iterator> reverse_iterator;
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::ptrdiff_t difference_type;
    typedef std::size_t size_type;

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
    size_type size() const
    {
        return detail::win32::get_menu_item_count(m_menu.get());
    }

    size_type max_size() const
    {
        return boost::integer_traits<UINT>::const_max;
    }

    /**
     * Returns whether the menu is empty.
     */
    bool empty() const
    {
        return size() == 0;
    }

    /**
     * Create an item based on the given description and insert it into the
     * menu at the given iterator position.
     *
     * Shuffles existing items along.
     */
    void insert(const description_type& item, const iterator& position)
    {
        insert_at_position(item, boost::numeric_cast<UINT>(position - begin()));
    }

    /**
     * Insert an item at the end of the menu.
     */
    void insert(const description_type& item)
    {
        insert_at_position(item, (UINT)-1);
    }

    value_type operator[](size_type position) const
    {
        if (position >= size())
            BOOST_THROW_EXCEPTION(
                std::range_error("Menu item index out of range"));

        return value_type(
            detail::item_position(
                handle(), boost::numeric_cast<UINT>(position)));
    }

    iterator begin() const
    {
        return iterator(*this);
    }

    iterator end() const
    {
        return iterator::end(*this);
    }

    reverse_iterator rbegin() const
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const
    {
        return reverse_iterator(begin());
    }

    iterator default_item() const
    {
        UINT pos = detail::win32::get_menu_default_item(
            m_menu.get(), TRUE, GMDI_USEDISABLED);

        if (pos == -1)
            return end();
        else
            return begin() + pos;
    }

    void default_item(iterator position)
    {
        detail::win32::set_menu_default_item(
            m_menu.get(), boost::numeric_cast<UINT>(position - begin()), TRUE);
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

    bool swap(basic_menu& other)
    {
        boost::swap(m_menu, other.m_menu);
    }

private:

    const menu_handle handle() const
    {
        return m_menu;
    }

    class inserter
    {
    public:
        inserter(HMENU menu, UINT id, BOOL is_by_position)
            : m_menu(menu), m_id(id), m_is_by_position(is_by_position) {}

        inline void operator()(const MENUITEMINFOW& info)
        {
            detail::win32::insert_menu_item(
                m_menu, m_id, m_is_by_position, &info);
        }

    private:
        HMENU m_menu;
        UINT m_id;
        BOOL m_is_by_position;
    };

    void insert_at_position(const description_type& item, UINT position)
    {
        // Menu items (sub_menus only AFAIK) can have resources and the
        // menu takes ownership of these resources when the item is inserted.
        // We need to allow the item description to manage this transfer of
        // ownership

        item.do_insertion_and_relinquish_resources(
            inserter(m_menu.get(), position, TRUE));
    }

    // TODO: The menu handle is mutable so that, when being inserted into
    // another menu via a sub_menu_item_description, the description is
    // able to modify the ownership.
    // This is a bit of a hack allowing us to treat the menu item
    // description as const.  A better way round this would be to redesign
    // insert() to take the descriptions as a non-const template parameter.
    // Then most descriptions could be simple, copyable objects, but the
    // submenu_description, which has to manage its own resources, would
    // do something more complicated like being move-only.  This may
    // require an extra overload of insert().
    // The complication is that this would also mean ditching the
    // item_description interface, so we would have to find another
    // way to make sure some item can't be inserted into menu bars.
    mutable menu_handle m_menu;
};

namespace detail {

    class id_match
    {
    public:
        typedef bool result_type;

        id_match(UINT id) : m_id(id) {}

        template<typename T>
        bool operator()(const T& item) const
        {
            return item.id() == m_id;
        }

    private:
        UINT m_id;
    };

    class item_id_matches
    {
    public:
        item_id_matches(UINT id) : m_id(id) {}

        bool operator()(item& menu_item) const
        {
            return menu_item.accept(id_match(m_id));
        }

    private:
        UINT m_id;
    };

}

template<typename ItemIterator>
ItemIterator find_first_item_with_id(
    ItemIterator begin, ItemIterator end, UINT menu_id)
{
    return std::find_if(begin, end, detail::item_id_matches(menu_id));
}

}}} // namespace washer::gui::menu

#endif
