/**
    @file

    Menu item button drawn by window.

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

#ifndef WINAPI_GUI_MENU_BUTTON_OWNER_DRAWN_BUTTON_HPP
#define WINAPI_GUI_MENU_BUTTON_OWNER_DRAWN_BUTTON_HPP
#pragma once

#include <winapi/gui/menu/detail/item_position.hpp>

#include <boost/noncopyable.hpp>

#include <Windows.h> // MIIM_TYPE

namespace winapi {
namespace gui {
namespace menu {

/**
 * Menu button whose clickable area is rendered by the window that owns the
 * menu.
 *
 * The constructor optionally takes a pointer to user-defined data which the
 * window can use to draw the button.
 */
class owner_drawn_button : private boost::noncopyable
{
    friend class button;

public:

    void* user_defined_data() const
    {
        return reinterpret_cast<void*>(
            m_item.get_menuiteminfo(MIIM_TYPE).dwTypeData);
    }

    void user_defined_data(void* new_data)
    {
        MENUITEMINFOW info = m_item.get_menuiteminfo(MIIM_TYPE);

        assert(info.fType == MFT_OWNERDRAW);

        info.dwTypeData = static_cast<wchar_t*>(new_data);
    }

private:

    explicit owner_drawn_button(const detail::item_position& item)
        : m_item(item) {}

    detail::item_position m_item;
};

}}} // namespace winapi::gui::menu

#endif
