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

#ifndef WINAPI_GUI_MENU_MENU_BUTTONS_HPP
#define WINAPI_GUI_MENU_MENU_BUTTONS_HPP
#pragma once

#include <cassert> // assert
#include <string>

#include <Windows.h> // MENUITEMINFO, HBITMAP

namespace winapi {
namespace gui {
namespace menu {

/**
 * Interface to menu item button description.
 *
 * Purpose: to separate the button-describing aspects of menus from other
 * orthogonal aspects which are muddled in with it in the Win32 API.
 *
 * Command items and submenu items have buttons and separator items do not.
 * The button type is independent of the other appearance properties such as
 * whether an item is enabled or checked.
 */
class menu_button_nature
{
    friend class command_menu_item;

    friend class sub_menu;

public:

    virtual ~menu_button_nature() {}

    menu_button_nature* clone() const
    {
        menu_button_nature* item = do_clone();
        assert(typeid(*this) == typeid(*item) && "do_clone() sliced object!");
        return item;
    }

private:

    /**
     * Win32 representation of the button as a partially-completed MENUITEMINFO.
     *
     * Classes that implement button natures expose themselves to the menu
     * classes that use them through this method.
     */
    virtual MENUITEMINFOW as_menuiteminfo() const = 0;
    virtual menu_button_nature* do_clone() const = 0;
};

/**
 * Menu button whose entire clickable target area is a picture.
 */
class bitmap_menu_button : public menu_button_nature
{
public:

    explicit bitmap_menu_button(HBITMAP bitmap) : m_bitmap(bitmap) {}

    HBITMAP bitmap() const
    {
        return m_bitmap;
    }

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        // Old-style whole-button bitmap specified through dwTypeData and
        // MFT_BITMAP (MF_BITMAP) requires MIIM_TYPE rather than MIIM_FTYPE
        info.fMask = MIIM_TYPE;
        info.fType = MFT_BITMAP;
        info.dwTypeData =
            reinterpret_cast<wchar_t*>(const_cast<HBITMAP>(m_bitmap));

        return info;
    }

    virtual bitmap_menu_button* do_clone() const
    {
        return new bitmap_menu_button(*this);
    }

    const HBITMAP m_bitmap;
};

/**
 * Standard menu button whose clickable target area is a text string.
 *
 * @todo  Separate accelerator combo (after tab character).
 */
class string_menu_button : public menu_button_nature
{
public:

    explicit string_menu_button(const std::wstring& caption)
        : m_caption(caption) {}

    /**
     * The text displayed on the button.
     */
    std::wstring caption() const
    {
        return m_caption;
    }

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        // MIIM_STRING might be all that's needed but specifying MIIM_FTYPE as
        // we do set fType and MFT_STRING just in case.
        // The MSDN docs are hopelessly rubbish.  No idea if this is the right
        // thing to do.
        info.fMask = MIIM_FTYPE | MIIM_STRING;
        info.fType = MFT_STRING;
        info.dwTypeData = const_cast<wchar_t*>(m_caption.c_str());
        // doesn't seem to be necessary but better safe than sorry:
        info.cch = m_caption.size();

        return info;
    }

    virtual string_menu_button* do_clone() const
    {
        return new string_menu_button(*this);
    }

    const std::wstring m_caption;
};

/**
 * Menu button whose clickable area is rendered by the window that owns the
 * menu.
 *
 * The constructor optionally takes a pointer to user-defined data which the
 * window can use to draw the button.
 */
class owner_drawn_menu_button : public menu_button_nature
{
public:

    explicit owner_drawn_menu_button(void* user_defined_data = NULL)
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

    virtual owner_drawn_menu_button* do_clone() const
    {
        return new owner_drawn_menu_button(*this);
    }

    void* const m_user_defined_data;
};

}}} // namespace winapi::gui::menu

#endif
