/**
    @file

    Builder for caption-based menu button.

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

#ifndef WINAPI_GUI_MENU_BUTTON_BUTTON_STRING_BUTTON_DESCRIPTION_HPP
#define WINAPI_GUI_MENU_BUTTON_BUTTON_STRING_BUTTON_DESCRIPTION_HPP
#pragma once

#include <winapi/gui/menu/button/button_description.hpp>

#include <boost/numeric/conversion/cast.hpp> // numeric_cast

#include <string>

#include <windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Creates a standard menu button whose clickable target area is a text string.
 *
 * @todo  Separate accelerator combo (after tab character).
 */
class string_button_description : public button_description
{
public:

    explicit string_button_description(const std::wstring& caption)
        : m_caption(caption) {}

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        // Using MIIM_TYPE instead of MIIM_STRING for backward compatibility
        // with Win 98.  When we allow side-bitmaps on string items,
        // this will need to change back again.
        //
        // Old comment:
        // MIIM_STRING might be all that's needed but specifying MIIM_FTYPE as
        // we do set fType and MFT_STRING just in case.
        // The MSDN docs are hopelessly rubbish.  No idea if this is the right
        // thing to do.
        //info.fMask = MIIM_FTYPE | MIIM_STRING;
        info.fMask = MIIM_TYPE;
        info.fType = MFT_STRING;
        info.dwTypeData = const_cast<wchar_t*>(m_caption.c_str());
        // doesn't seem to be necessary but better safe than sorry:
        info.cch = boost::numeric_cast<UINT>(m_caption.size());

        return info;
    }

    virtual string_button_description* do_clone() const
    {
        return new string_button_description(*this);
    }

    std::wstring m_caption;
};

}}} // namespace winapi::gui::menu

#endif
