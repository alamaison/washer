/**
    @file

    Command item extracted from underlying HMENU.

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

#ifndef WINAPI_GUI_MENU_ITEM_COMMAND_ITEM_HPP
#define WINAPI_GUI_MENU_ITEM_COMMAND_ITEM_HPP
#pragma once

#include <winapi/gui/menu/button/button.hpp>
#include <winapi/gui/menu/detail/item_common.hpp> // selectable_item_core
#include <winapi/gui/menu/detail/item_position.hpp>
#include <winapi/gui/menu/menu.hpp>
#include <winapi/gui/menu/item/selectable_item.hpp>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace winapi {
namespace gui {
namespace menu {

/**
 * Simple button menu entry with an associated command ID.
 *
 * The ID is used when informing the program that the user clicked a menu item.
 * The type of button is passed as an argument to the constructor.
 */
class command_item : public selectable_item, private boost::noncopyable
{
public:

    /**
     * The ID sent to the owner windows when this command on the menu is
     * selected.
     */
    UINT id() const
    {
        return m_core.id();
    }

    virtual command_item& selectability(BOOST_SCOPED_ENUM(selectability) state)
    {
        m_core.button_state(state);
        return *this;
    }

    virtual command_item& check_mark_visibility(
        BOOST_SCOPED_ENUM(check_mark) state)
    {
        m_core.check_mark(state);
        return *this;
    }

    virtual ::winapi::gui::menu::button button() const
    {
        return m_core.button();
    }

    virtual bool is_selectable() const
    {
        return m_core.is_selectable();
    }

    virtual bool check_mark_is_visible() const
    {
        return m_core.check_mark_is_visible();
    }

    virtual bool is_highlighted() const
    {
        return m_core.is_highlighted();
    }

    explicit command_item(const detail::item_position& item) : m_core(item) {}

private:

    detail::selectable_item_core m_core;
};

namespace detail {

    template<typename Visitor>
    inline typename Visitor::result_type do_command_item_accept(
        const item_position& item, Visitor& visitor)
    {
        command_item item_view(item);
        return visitor(item_view);
    }

    template<typename Visitor>
    inline typename Visitor::result_type do_const_command_item_accept(
        const item_position& item, Visitor& visitor)
    {
        const command_item item_view(item);
        return visitor(item_view);
    }

}

}}} // namespace winapi::gui::menu

#endif
