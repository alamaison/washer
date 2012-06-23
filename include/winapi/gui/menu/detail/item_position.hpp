/**
    @file

    Internal representation of a position in a menu.

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

#ifndef WINAPI_GUI_MENU_DETAIL_ITEM_POSITION_HPP
#define WINAPI_GUI_MENU_DETAIL_ITEM_POSITION_HPP
#pragma once

#include <winapi/gui/menu/detail/menu_win32.hpp> // get_menu_item_info
#include <winapi/gui/menu/menu_handle.hpp>

#include <cassert> // assert
#include <string>
#include <vector>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {
namespace detail {

/**
 * Combines a menu and a menu position.
 *
 * Purpose: to abstract the notion of an item (whose type we do not know
 *          yet) at a particular point in a menu.
 *
 * Encapsulates the operations that only make sense when a menu and
 * a menu item position exist in unison.
 */
class item_position
{
public:

    item_position(const menu_handle& menu, UINT position)
        : m_handle(menu), m_position(position) {}

    MENUITEMINFOW get_menuiteminfo(UINT fMask) const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        info.fMask = fMask;
        detail::win32::get_menu_item_info(
            m_handle.get(), m_position, TRUE, &info);

        return info;
    }

    /**
     * Getting the string from a MENUITEMINFO gets its own special method
     * because of the double-call semantics and buffer ownership.
     */
    std::wstring get_caption() const
    {
        MENUITEMINFOW info = get_menuiteminfo(MIIM_FTYPE | MIIM_STRING);

        assert(
            (info.fType & (MFT_BITMAP | MFT_OWNERDRAW | MFT_SEPARATOR))
            == 0);

        std::vector<wchar_t> buffer(info.cch + 1);
        info.cch = buffer.size();
        info.dwTypeData = (!buffer.empty()) ? &buffer[0] : NULL;
        detail::win32::get_menu_item_info(
            m_handle.get(), m_position, TRUE, &info);

        if (info.cch > 0)
        {
            assert(info.dwTypeData);
            return std::wstring(info.dwTypeData, info.cch);
        }
        else
        {
            return std::wstring();
        }
    }

private:

    const menu_handle m_handle;
    const UINT m_position;
};

}}}} // namespace winapi::gui::menu::detail

#endif