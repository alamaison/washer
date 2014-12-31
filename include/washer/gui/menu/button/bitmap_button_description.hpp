/**
    @file

    Whole-button menu button builder.

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

#ifndef WASHER_GUI_MENU_BUTTON_BITMAP_BUTTON_DESCRIPTION_HPP
#define WASHER_GUI_MENU_BUTTON_BITMAP_BUTTON_DESCRIPTION_HPP
#pragma once

#include <washer/gui/menu/button/button_description.hpp>

#include <cassert> // assert
#include <stdexcept> // invalid_argument

#include <Windows.h> // MENUITEMINFO, HBITMAP

namespace washer {
namespace gui {
namespace menu {

/**
 * Creates a button whose entire clickable target area is a picture.
 */
class bitmap_button_description : public button_description
{
public:

    /**
     * @param bitmap  the bitmap which must not be null
     */
    explicit bitmap_button_description(HBITMAP bitmap) : m_bitmap(bitmap)
    {
        // We cannot allow NULL bitmaps as, bizzarrely, Windows converts bitmap
        // items with a NULL bitmap to separators when they are inserted!
        if (!bitmap)
            BOOST_THROW_EXCEPTION(std::invalid_argument("Bitmap required"));
    }

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        // Old-style whole-button bitmap specified through dwTypeData and
        // MFT_BITMAP (MF_BITMAP) requires MIIM_TYPE rather than MIIM_FTYPE
        info.fMask = MIIM_TYPE;

        assert (m_bitmap != NULL);
        info.fType = MFT_BITMAP;
        info.dwTypeData =
            reinterpret_cast<wchar_t*>(const_cast<HBITMAP>(m_bitmap));

        return info;
    }

    virtual bitmap_button_description* do_clone() const
    {
        return new bitmap_button_description(*this);
    }

    HBITMAP m_bitmap;
};

}}} // namespace washer::gui::menu

#endif
