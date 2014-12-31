/**
    @file

    Menu separator.

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

#ifndef WASHER_GUI_MENU_ITEM_SEPARATOR_ITEM_HPP
#define WASHER_GUI_MENU_ITEM_SEPARATOR_ITEM_HPP
#pragma once

#include <boost/noncopyable.hpp>

namespace washer {
namespace gui {
namespace menu {

/**
 * Line dividing menu into logical sections.
 */
class separator_item : private boost::noncopyable
{
public:

    separator_item(const detail::item_position& item) : m_item(item) {}

    /**
     * The ID of the item.
     *
     * Used to find an item in a menu even if its position changes.
     */
    UINT id() const
    {
        return m_item.get_menuiteminfo(MIIM_ID).wID;
    }

private:
    detail::item_position m_item;
};

namespace detail {

    template<typename Visitor>
    inline typename Visitor::result_type do_separator_item_accept(
        const item_position& item, Visitor& visitor)
    {
        separator_item item_view(item);
        return visitor(item_view);
    }

    template<typename Visitor>
    inline typename Visitor::result_type do_const_separator_item_accept(
        const item_position& item, Visitor& visitor)
    {
        const separator_item item_view(item);
        return visitor(item_view);
    }

}

}}} // namespace washer::gui::menu

#endif
