/**
    @file

    Menu button with simple string caption.

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

#ifndef WINAPI_GUI_MENU_BUTTON_STRING_BUTTON_HPP
#define WINAPI_GUI_MENU_BUTTON_STRING_BUTTON_HPP
#pragma once

#include <winapi/gui/menu/detail/item_position.hpp>

#include <boost/noncopyable.hpp>

#include <string>

namespace winapi {
namespace gui {
namespace menu {

/**
 * Standard menu button whose clickable target area is a text string.
 *
 * @todo  Extract accelerator combo (after tab character).
 */
class string_button : private boost::noncopyable
{
    friend class button;

public:

    /**
     * The text displayed on the button.
     */
    std::wstring caption() const
    {
        return m_item.get_caption();
    }

    void caption(const std::wstring& new_caption)
    {
        // MIIM_STRING might be all that's needed but specifying MIIM_FTYPE as
        // we do set fType and MFT_STRING just in case.
        // The MSDN docs are hopelessly rubbish.  No idea if this is the right
        // thing to do.
        MENUITEMINFOW info = m_item.get_menuiteminfo(MIIM_FTYPE | MIIM_STRING);

        assert(info.fType == MFT_STRING);

        info.dwTypeData = const_cast<wchar_t*>(new_caption.c_str());
        // doesn't seem to be necessary but better safe than sorry:
        info.cch = new_caption.size();

        m_item.set_menuiteminfo(info);
    }

private:

    explicit string_button(const detail::item_position& item) : m_item(item) {}

    detail::item_position m_item;
};

}}} // namespace winapi::gui::menu

#endif
