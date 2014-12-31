/**
    @file

    Clickable menu buttons types.

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

#ifndef WASHER_GUI_MENU_BUTTON_OWNER_DRAWN_BUTTON_DESCRIPTION_HPP
#define WASHER_GUI_MENU_BUTTON_OWNER_DRAWN_BUTTON_DESCRIPTION_HPP
#pragma once

#include <cassert> // assert

#include <Windows.h> // MENUITEMINFO

namespace washer {
namespace gui {
namespace menu {

/**
 * Creates a menu button whose clickable area is rendered by the window that
 * owns the menu.
 *
 * The constructor optionally takes a pointer to user-defined data which the
 * window can use to draw the button.
 */
class owner_drawn_button_description : public button_description
{
public:

    explicit owner_drawn_button_description(void* user_defined_data = NULL)
        : m_user_defined_data(user_defined_data) {}

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        // TYPE rather than FTYPE as we use dwTypeData
        // The MSDN docs are hopelessly rubbish.  Once again, no idea if this
        // is the right thing to do.
        info.fMask = MIIM_TYPE;
        info.fType = MFT_OWNERDRAW;
        info.dwTypeData = static_cast<wchar_t*>(m_user_defined_data);

        return info;
    }

    virtual owner_drawn_button_description* do_clone() const
    {
        return new owner_drawn_button_description(*this);
    }

    void* m_user_defined_data;
};

}}} // namespace washer::gui::menu

#endif
