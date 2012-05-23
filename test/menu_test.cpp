/**
    @file

    Tests for menu wrappers.

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

#include <winapi/gui/menu/menu.hpp> // test subject
#include <winapi/gui/menu/menu_items.hpp> // test subject

#include <winapi/dynamic_link.hpp> // module_handle
#include <winapi/gui/windows/window.hpp>
#include <winapi/error.hpp> // last_error

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <string>

using winapi::gui::hwnd_t;
using namespace winapi::gui::menu;
using winapi::gui::window;
using winapi::module_handle;

using std::string;
using std::wstring;

namespace {

    LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }
    }

    void pump_thread_messages()
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

    window<> create_test_window()
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

        return window<>(hwnd_t(hwnd, ::DestroyWindow));
    }

    template<typename AorW>
    void show_window(window<AorW>& w)
    {
        w.visible(true);
        w.text(boost::unit_test::framework::current_test_case().p_name);
        pump_thread_messages();
    }

    HBITMAP test_bitmap()
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


}

BOOST_AUTO_TEST_SUITE(menu_bar_tests)

/**
 * An empty menu bar; not attached to window.
 */
BOOST_AUTO_TEST_CASE( empty_menu_bar )
{
    menu_bar m;
    BOOST_CHECK(m.begin() == m.end());
}

/**
 * An empty menu bar added to a window
 */
BOOST_AUTO_TEST_CASE( empty_menu_bar_assigned )
{
    menu_bar m;
    window<> w = create_test_window();
    w.menu(m);
    BOOST_CHECK(m.begin() == m.end());

    //show_window(w);
}

BOOST_AUTO_TEST_CASE( string_command_in_bar )
{
    menu_bar b;

    b.append(make_command_menu_item(
        string_menu_button(L"string command in bar"), 14));

    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    //show_window(w);
}

BOOST_AUTO_TEST_CASE( bitmap_command_in_bar )
{
    menu_bar b;

    b.append(make_command_menu_item(bitmap_menu_button(test_bitmap()), 13));

    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    //show_window(w);
}

BOOST_AUTO_TEST_CASE( string_popup_in_bar )
{
    menu_bar b;

    menu m;
    m.append(make_command_menu_item(string_menu_button(L"Pop"), 1));

    b.append(make_sub_menu(string_menu_button(L"string command in bar"), m));

    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    //show_window(w);
}

BOOST_AUTO_TEST_CASE( bitmap_popup_in_bar )
{
    menu_bar b;

    menu m;
    m.append(make_command_menu_item(string_menu_button(L"Pop"), 1));

    b.append(make_sub_menu(bitmap_menu_button(test_bitmap()), m));

    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    //show_window(w);
}

BOOST_AUTO_TEST_SUITE_END();

namespace {

/**
 * Fixture that tests the behaviour of a menu that isn't given to a menu bar or
 * a menu bar in a window.
 *
 * This is the scenario where the menu is being used as a context menu rather
 * than rooted in the window.
 *
 * The other two scenarios take ownership of the menu's lifetime so this tests
 * how the menu copes with managing its own lifetime.
 */
class lonely_fixture
{
public:
    static void do_test(menu& m)
    {
        // do nothing

        BOOST_CHECK(m.valid());
    }
};

/**
 * Fixture that test the behaviour of a menu that is given to a menu bar but
 * (strangely) the menu bar is not given to a window.
 *
 * The menu bar takes ownership of the menu so it has to cope with this
 * intrusion.
 */
class no_window_fixture
{
public:
    static void do_test(menu& m)
    {
        menu_bar b;
        b.append(make_sub_menu(string_menu_button(L"Menu"), m));
        BOOST_CHECK(b.begin() != b.end());

        BOOST_CHECK(m.valid());
        BOOST_CHECK(b.valid());
    }
};

/**
 * Fixture that tests the behaviour of a menu in the typical situation where
 * it has been given to a menu bar which, in turn, is given to a window.
 *
 * This window controls the lifetime of the menus.
 */
class normal_usage_fixture
{
public:
    static void do_test(menu& m)
    {
        menu_bar b;
        b.append(make_sub_menu(string_menu_button(L"Menu"), m));
        BOOST_CHECK(b.begin() != b.end());

        BOOST_CHECK(m.valid());
        BOOST_CHECK(b.valid());

        {
            window<> w = create_test_window();
            w.menu(b);
            //show_window(w);
        }

        BOOST_CHECK(!m.valid());
        BOOST_CHECK(!b.valid());
    }
};

typedef boost::mpl::list<
    lonely_fixture, no_window_fixture, normal_usage_fixture> fixtures;

}

BOOST_AUTO_TEST_SUITE(menu_tests)

/**
 * An empty menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( empty_menu, F, fixtures )
{
    menu m;

    BOOST_CHECK(m.begin() == m.end());

    F::do_test(m);
}

/**
 * A simple string command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( string_command, F, fixtures )
{
    menu m;
    m.append(make_command_menu_item(string_menu_button(L"Child command"), 1));

    BOOST_CHECK(m.begin() != m.end());

    F::do_test(m);
}

/**
 * A bitmap command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( bitmap_command, F, fixtures )
{
    menu m;
    m.append(make_command_menu_item(bitmap_menu_button(test_bitmap()), 1));

    BOOST_CHECK(m.begin() != m.end());

    F::do_test(m);
}

/**
 * A separator.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( separator, F, fixtures )
{
    menu m;
    m.append(separator_menu_item());

    BOOST_CHECK(m.begin() != m.end());

    F::do_test(m);
}

/**
 * A string popping out a submenu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( string_submenu, F, fixtures )
{
    menu m;
    menu sub;

    sub.append(make_command_menu_item(string_menu_button(L"Boo"), 1));

    m.append(make_sub_menu(string_menu_button(L"Pop"), sub));

    BOOST_CHECK(m.begin() != m.end());

    F::do_test(m);
}

/**
 * A bitmap popping out a submenu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( bitmap_submenu, F, fixtures )
{
    menu m;
    menu sub;

    BOOST_CHECK(m.begin() == m.end());
    BOOST_CHECK(sub.begin() == sub.end());

    sub.append(make_command_menu_item(string_menu_button(L"Boo"), 1));

    m.append(make_sub_menu(bitmap_menu_button(test_bitmap()), sub));

    BOOST_CHECK(m.begin() != m.end());

    F::do_test(m);
}

/**
 * A menu with multiple items.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( mixed_items, F, fixtures )
{
    menu m;

    m.append(make_command_menu_item(string_menu_button(L"String command"), 1));

    m.append(make_command_menu_item(bitmap_menu_button(test_bitmap()), 2));

    m.append(separator_menu_item());

    menu sub;

    sub.append(make_command_menu_item(string_menu_button(L"String sub"), 3));

    sub.append(separator_menu_item());

    sub.append(make_command_menu_item(bitmap_menu_button(test_bitmap()), 4));

    m.append(make_sub_menu(string_menu_button(L"Lalala"), sub));

    F::do_test(m);
}

BOOST_AUTO_TEST_SUITE_END();
