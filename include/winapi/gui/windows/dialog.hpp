/**
    @file

    Dialog HWND wrapper class.

    @if license

    Copyright (C) 2010, 2011  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_GUI_WINDOWS_DIALOG_HPP
#define WINAPI_GUI_WINDOWS_DIALOG_HPP
#pragma once

#include <winapi/gui/windows/window.hpp> // window base class

namespace winapi {
namespace gui {

/**
 * Wrapper around an icon (a STATIC window with SS_ICON style).
 *
 * @todo  Consider if it is worth checking class and style and throwing
 *        an exception if it fails.
 */
template<typename T>
class dialog_window : public window<T>
{
public:
    explicit dialog_window(HWND hwnd) : window(hwnd) {}
    explicit dialog_window(hwnd_t hwnd) : window(hwnd) {}

    /**
     * Dialog manager message handling procedure.
     */
    DLGPROC dialog_procedure()
    {
        return window_field<T, DLGPROC>(hwnd(), DWLP_DLGPROC);
    }

    /**
     * Change the function that handles dialog messages.
     *
     * This method is used to 'subclass' the dialog manager.
     *
     * @returns  Pointer to previous dialog message procedure.
     */
    DLGPROC change_dialog_procedure(DLGPROC new_dlgproc)
    {
        return set_window_field<T>(hwnd(), DWLP_DLGPROC, new_dlgproc);
    }

};

}} // namespace winapi::gui

#endif
