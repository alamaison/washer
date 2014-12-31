/**
    @file

    Whole-button bitmap button.

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

#ifndef WASHER_GUI_MENU_BUTTON_BITMAP_BUTTON_HPP
#define WASHER_GUI_MENU_BUTTON_BITMAP_BUTTON_HPP
#pragma once

#include <washer/gui/menu/detail/item_position.hpp>

#include <boost/noncopyable.hpp>

#include <Windows.h> // MENUITEMINFO, HBITMAP

namespace washer {
namespace gui {
namespace menu {

/**
 * Menu button whose entire clickable target area is a picture.
 */
class bitmap_button : private boost::noncopyable
{
    friend class button;

public:

    HBITMAP bitmap() const
    {
        // Old-style whole-button bitmap specified through dwTypeData and
        // MFT_BITMAP (MF_BITMAP) requires MIIM_TYPE rather than MIIM_FTYPE
        MENUITEMINFOW info = m_item.get_menuiteminfo(MIIM_TYPE);

        return reinterpret_cast<HBITMAP>(info.dwTypeData);
    }

    void bitmap(HBITMAP bitmap)
    {
        MENUITEMINFOW info = m_item.get_menuiteminfo(MIIM_TYPE);
        info.dwTypeData = reinterpret_cast<wchar_t*>(bitmap);
        m_item.set_menuiteminfo(info);
    }

private:

    explicit bitmap_button(const detail::item_position& item) : m_item(item) {}

    detail::item_position m_item;
};

}}} // namespace washer::gui::menu

#endif
