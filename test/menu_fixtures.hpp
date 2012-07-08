/**
    @file

    Fixtures used in menu tests.

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

#ifndef WINAPI_TEST_MENU_FIXTURES_HPP
#define WINAPI_TEST_MENU_FIXTURES_HPP
#pragma once

#include "fixture_permutator.hpp"
#include "wchar_output.hpp" // wchar_t test output

// These are the tests subjects created by the fixtures
#include <winapi/gui/menu/button/string_button_description.hpp>
#include <winapi/gui/menu/item/sub_menu_item_description.hpp>
#include <winapi/gui/menu/menu.hpp>
#include <winapi/gui/menu/menu_handle.hpp>

#include <winapi/dynamic_link.hpp> // module_handle
#include <winapi/gui/windows/window.hpp>
#include <winapi/error.hpp> // last_error

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/mpl/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <string>

namespace winapi {
namespace test {

namespace detail {

    inline LRESULT CALLBACK wnd_proc(
        HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_CLOSE:
            ::DestroyWindow(hwnd);
            return 1;

        case WM_DESTROY:
            if (!::GetParent(hwnd))
            {
                ::PostQuitMessage(0);
            }
            return 1;

        default:
            ::SetForegroundWindow(hwnd);
            return ::DefWindowProcW(hwnd, msg, wparam, lparam);
        }
    }

    inline void pump_thread_messages()
    {
        MSG msg = MSG();
        BOOL rc;
        while ((rc = ::GetMessageW(&msg, NULL, 0, 0)) > 0)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        if (rc < 0)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("GetMessage"));
    }

    inline gui::window<> create_test_window()
    {
        WNDCLASSEXW window_class = WNDCLASSEXW();
        window_class.cbSize = sizeof(WNDCLASSEXW);
        window_class.lpszClassName = L"testclass";
        window_class.lpfnWndProc = &wnd_proc;

        ATOM klass = ::RegisterClassExW(&window_class);
        //if (klass == 0)
        //    BOOST_THROW_EXCEPTION(
        //        boost::enable_error_info(winapi::last_error()) <<
        //        boost::errinfo_api_function("RegisterClassEx"));

        HWND hwnd = ::CreateWindowExW(
            0L, L"testclass", L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
        // BUG: CreateWindowExW doesn't set the error when passed
        // invalid arguments
        if (hwnd == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("CreateWindowEx"));

        return gui::window<>(gui::hwnd_t(hwnd, ::DestroyWindow));
    }

    template<typename AorW>
    inline void show_window(gui::window<AorW>& w)
    {
        w.text(boost::unit_test::framework::current_test_case().p_name);
        //w.visible(true);
        //pump_thread_messages();
    }
}

inline void do_insertion(
    HMENU handle, const wchar_t* caption, UINT command_id,
    HMENU sub_menu, UINT fMask, UINT fType)
{
    MENUITEMINFOW info = MENUITEMINFOW();
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = fMask;
    info.fType = fType;
    info.wID = command_id;
    info.hSubMenu = sub_menu;
    info.dwTypeData = const_cast<wchar_t*>(caption);
    gui::menu::detail::win32::insert_menu_item(
        handle, 0, MF_BYPOSITION, &info);
}

inline HBITMAP test_bitmap()
{
    HBITMAP bitmap = (HBITMAP)::LoadImageW(
        module_handle("shell32.dll"), L"#145", IMAGE_BITMAP,
        LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_SHARED);
    if (bitmap == NULL)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("LoadImage"));

    return bitmap;
}

/**
 * Fixture that tests the behaviour of a menu that isn't given to a menu bar or
 * a menu bar in a window.
 *
 * This is the scenario where the menu is being used as a context menu rather
 * than rooted in the window.  Or where, bizarrely, a menu bar is created but
 * then never used.
 *
 * The other two scenarios take ownership of the menu's lifetime so this tests
 * how the menu copes with managing its own lifetime.
 */
template<typename F>
class lonely_fixture : public F
{
public:
    static void do_ownership_test(gui::menu::menu& m)
    {
        // do nothing

        BOOST_CHECK(m.valid());
    }

    static void do_ownership_test(gui::menu::menu_bar& m)
    {
        // do nothing

        BOOST_CHECK(m.valid());
    }
};

/**
 * Fixture that tests the behaviour of a menu that is given to a menu bar but
 * (strangely) the menu bar is not given to a window.
 *
 * The menu bar takes ownership of the menu so it has to cope with this
 * intrusion.
 */
template<typename F>
class no_window_fixture : public F
{
public:
    static void do_ownership_test(gui::menu::menu& m)
    {
        gui::menu::menu_bar b;
        b.insert(
            gui::menu::sub_menu_item_description(
                gui::menu::string_button_description(L"Menu"), m));
        BOOST_CHECK(b.begin() != b.end());

        BOOST_CHECK(m.valid());
        BOOST_CHECK(b.valid());
    }

    static void do_ownership_test(gui::menu::menu_bar& m)
    {
        // do nothing

        BOOST_CHECK(m.valid());
    }
};

/**
 * Fixture that tests the behaviour of a menu in the typical situation where
 * it has been given to a menu bar which, in turn, is given to a window.
 *
 * This window controls the lifetime of the menus.
 */
template<typename F>
class normal_usage_fixture : public F
{
public:

    static void do_ownership_test(gui::menu::menu& m)
    {
        gui::menu::menu_bar b;
        b.insert(
            gui::menu::sub_menu_item_description(
                gui::menu::string_button_description(L"Menu"), m));
        BOOST_CHECK(b.begin() != b.end());

        BOOST_CHECK(m.valid());
        BOOST_CHECK(b.valid());

        {
            gui::window<> w = detail::create_test_window();
            w.menu(b);
            detail::show_window(w);
        }

        BOOST_CHECK(!m.valid());
        BOOST_CHECK(!b.valid());
    }

    static void do_ownership_test(gui::menu::menu_bar& m)
    {
        BOOST_CHECK(m.valid());

        {
            gui::window<> w = detail::create_test_window();
            w.menu(m);
            detail::show_window(w);
        }

        BOOST_CHECK(!m.valid());
    }
};

typedef boost::mpl::vector<
    lonely_fixture<boost::mpl::_>, no_window_fixture<boost::mpl::_>,
    normal_usage_fixture<boost::mpl::_> >
    menu_ownership_fixtures;

template<typename T>
class test_menu_
{
public:
    test_menu_(const T& m, gui::menu::menu_handle handle)
        : m_menu(m), m_handle(handle) {}

    T& menu() { return m_menu; }
    gui::menu::menu_handle handle() { return m_handle; }

private:
    T m_menu;
    gui::menu::menu_handle m_handle;
};

template<typename F>
struct menu_creator : public F
{
    typedef gui::menu::menu menu_type;

    static menu_type create_menu_to_test()
    {
        return menu_type();
    }
};

template<typename F>
struct bar_creator : public F
{
    typedef gui::menu::menu_bar menu_type;

    static menu_type create_menu_to_test()
    {
        return menu_type();
    }
};

typedef boost::mpl::vector<
    menu_creator<boost::mpl::_>, bar_creator<boost::mpl::_> >
menu_type_fixtures;

typedef fixture_permutator<
    menu_type_fixtures,
    menu_ownership_fixtures
>::type menu_creator_fixtures;

template<typename F>
struct normal_menu_handle_creator : public F
{
    typedef gui::menu::menu menu_type;
    typedef test_menu_<menu_type> test_menu;

    static test_menu create_menu_to_test()
    {
        gui::menu::menu_handle handle = gui::menu::menu_handle::adopt_handle(
            gui::menu::detail::win32::create_popup_menu());
        return test_menu(menu_type(handle), handle);
    }
};

template<typename F>
struct menu_bar_handle_creator : public F
{
    typedef gui::menu::menu_bar menu_type;
    typedef test_menu_<menu_type> test_menu;

    static test_menu create_menu_to_test()
    {
        gui::menu::menu_handle handle = gui::menu::menu_handle::adopt_handle(
            gui::menu::detail::win32::create_menu());
        return test_menu(menu_type(handle), handle);
    }
};

typedef boost::mpl::vector<
    normal_menu_handle_creator<boost::mpl::_>,
    menu_bar_handle_creator<boost::mpl::_> >
menu_with_handle_creator_fixtures;

typedef fixture_permutator<
    boost::mpl::vector<
        normal_menu_handle_creator<boost::mpl::_>,
        menu_bar_handle_creator<boost::mpl::_>
    >,
    menu_ownership_fixtures
>::type menu_fixtures;

}}

#endif
