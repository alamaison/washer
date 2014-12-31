/**
    @file

    General HWND wrapper class.

    @if license

    Copyright (C) 2010, 2011, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WASHER_WINDOW_WINDOW_HPP
#define WASHER_WINDOW_WINDOW_HPP
#pragma once

#include <washer/gui/hwnd.hpp> // HWND-manipulating functions
#include <washer/gui/menu/menu.hpp> // menu_bar
#include <washer/window/window_handle.hpp>
#include <washer/window/detail/window_win32.hpp> // set_menu, get_window_rect

#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert
#include <stdexcept> // logic_error
#include <string> // basic_string

namespace washer {
namespace window {

class invalid_window_error : public std::logic_error
{
public:
    invalid_window_error() : std::logic_error(
        "Can't perform operation with a NULL window handle (HWND)") {}
};

class rectangle
{
public:
    rectangle() : m_rect(RECT()) {}
    rectangle(const RECT& rect) : m_rect(rect) {}

    /// @name Coordinates
    // @{
    long left() const { return m_rect.left; }
    long top() const { return m_rect.top; }
    long bottom() const { return m_rect.bottom; }
    long right() const { return m_rect.right; }
    // @}

    /// @name Dimensions
    // @{
    long width() const { return right() - left(); }
    long height() const { return bottom() - top(); }
    // @}

    RECT* out() { return &m_rect; }

private:
    RECT m_rect;
};

/**
 * C++ interface to the Win32 window object.
 *
 * Purpose: to safely wrap the HWND and associated functions.
 *
 * @tparam T  The character width of the window.  Windows has both 'ANSI' and
 *            Unicode window objects; strange but true.
 */
template<typename T=wchar_t>
class window
{
public:

    explicit window(const window_handle& handle) : m_handle(handle) {}

    /**
     * Is the window theoretically visible?
     *
     * It may be obscured but not hidden.
     */
    bool is_visible() const
    {
        throw_if_invalid();
        return ::IsWindowVisible(m_handle.get()) != 0;
    }

    /**
     * Show or hide window.
     *
     * @returns  Previous visibility.
     */
    bool visible(bool state)
    {
        throw_if_invalid();
        return ::ShowWindow(m_handle.get(), (state) ? SW_SHOW : SW_HIDE) != 0;
    }

    /**
     * Is the window able to respond to they keyboard and mouse?
     *
     * Some types of window, e.g., buttons, may have a visible difference when
     * they are disabled usch as being greyed out.
     */
    bool is_enabled() const
    {
        throw_if_invalid();
        return ::IsWindowEnabled(m_handle.get()) != 0;
    }

    /**
     * Enable or disable a window.
     *
     * @returns  Previous enablement state.
     */
    bool enable(bool state)
    {
        throw_if_invalid();
        return ::EnableWindow(m_handle.get(), state) == 0;
    }

    /**
     * The window's text.
     *
     * All Win32 windows have a text field.  Even icons and images.
     *
     * @todo  Add version that gets the text in the 'other' character width.
     *        Regardless of the HWND type, we should be able to get the text
     *        as both.
     */
    template<typename U>
    std::basic_string<U> text() const
    {
        throw_if_invalid();
        return detail::window_text<U>(m_handle.get());
    }

    /// Change window text.
    void text(const std::string& new_text) { generic_text(new_text); }

    /// Change window text. @overload text(const std::string& new_text)
    void text(const std::wstring& new_text) { generic_text(new_text); }

    rectangle position() const
    {
        rectangle rect;
        detail::win32::get_window_rect(m_handle.get(), rect.out());
        return rect;
    }

    /**
     * Set the window menu.
     *
     * @todo  Move to a specialised app window class.
     * @todo  Deal with destroying the old window.
     */
    void menu(const washer::gui::menu::menu_bar& menu)
    {
        detail::win32::set_menu(hwnd(), menu.handle().get());
    }

    /**
     * Window message handling procedure.
     */
    WNDPROC window_procedure()
    {
        return washer::gui::window_field<T, WNDPROC>(
            m_handle.get(), GWLP_WNDPROC);
    }

    /**
     * Change the function that handles window messages.
     *
     * This method is used to 'subclass' a window by passing any messages,
     * not handled by the new wndproc, through to the previous window message
     * procedure (returned by this method).
     *
     * @returns  Pointer to previous window message procedure.
     */
    WNDPROC change_window_procedure(WNDPROC new_wndproc)
    {
        return washer::gui::set_window_field<T>(
            m_handle.get(), GWLP_WNDPROC, new_wndproc);
    }

    bool operator==(window other) const
    {
        return m_handle.get() == other.m_handle.get();
    }

    bool operator==(HWND handle) const
    {
        return handle == m_handle.get();
    }

    HWND hwnd() const { return m_handle.get(); }

private:

    /**
     * Change window text using a wide or a narrow string.
     *
     * This method exists to facilitate generic programming.  The public text()
     * methods use concrete string types to allow their parameters to be
     * given as a char* or a wchar_t* pointer (this isn't possible using
     * a basic_string).  Those methods delegate to this one.
     */
    template<typename U>
    void generic_text(const std::basic_string<U>& new_text)
    {
        throw_if_invalid();
        return detail::window_text(m_handle.get(), new_text);
    }

    void throw_if_invalid() const
    {
        if (!m_handle.get())
            BOOST_THROW_EXCEPTION(invalid_window_error());

        // This additional check is not reliable enough to put in a release
        // build because I don't understand the MSDN remarks regarding
        // threading
        assert(::IsWindow(m_handle.get()));
    }

    window_handle m_handle;
};

template<typename T>
bool operator==(HWND handle, const window<T>& other)
{
    return other == handle;
}

}} // namespace washer::window

#endif
