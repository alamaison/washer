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

#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/menu.hpp> // test subject
#include <winapi/gui/menu/menu_items.hpp> // test subject

#include <winapi/dynamic_link.hpp> // module_handle
#include <winapi/gui/windows/window.hpp>
#include <winapi/error.hpp> // last_error

#include <boost/cast.hpp> // polymorphic_cast
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/mpl/apply.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <string>

using winapi::gui::hwnd_t;
using namespace winapi::gui::menu;
using winapi::gui::window;
using winapi::module_handle;

using boost::shared_ptr;

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
        w.text(boost::unit_test::framework::current_test_case().p_name);
        //w.visible(true);
        //pump_thread_messages();
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

    void do_insertion(
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
        detail::win32::insert_menu_item(handle, 0, MF_BYPOSITION, &info);
    }
}

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
        b.append(sub_menu(string_menu_button(L"Menu"), m));
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
        b.append(sub_menu(string_menu_button(L"Menu"), m));
        BOOST_CHECK(b.begin() != b.end());

        BOOST_CHECK(m.valid());
        BOOST_CHECK(b.valid());

        {
            window<> w = create_test_window();
            w.menu(b);
            show_window(w);
        }

        BOOST_CHECK(!m.valid());
        BOOST_CHECK(!b.valid());
    }
};

typedef boost::mpl::vector<
    lonely_fixture, no_window_fixture, normal_usage_fixture> fixtures;

template<typename Outer>
struct fixtures_loop
{
    template<typename T>
    struct use : boost::mpl::apply<Outer, T> {};

    typedef typename boost::mpl::fold<
        fixtures, boost::mpl::vector<>,
        boost::mpl::push_back< boost::mpl::_1, use<boost::mpl::_2> >
    >::type type;
};

/**
 * Allows fixtures to be combined with the windows ownership fixtures
 */
template<typename T>
struct fixture_combinator
{
    typedef typename boost::mpl::fold<
        T, boost::mpl::vector<>,
        boost::mpl::copy<
            fixtures_loop<boost::mpl::_2>,
            boost::mpl::back_inserter<boost::mpl::_1>
        >
    >::type type;
};

/**
 * Tests where the menu was created externally and passed to the wrapper as a
 * raw menu.
 *
 * Thest test make sure that, given a menu, we can extra items from it
 * correctly.
 */
BOOST_AUTO_TEST_SUITE(raw_menu_tests)

/**
 * Wrap an empty menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( empty_menu, F, fixtures )
{
    menu_handle handle = menu_handle::adopt_handle(
        detail::win32::create_popup_menu());
    menu m(handle);

    BOOST_CHECK(m.begin() == m.end());

    F::do_test(m);
}

/**
 * Does insertion via the old insertion API.
 * The menu items are always retrieved using the new API so this tests that
 * the translation goes smoothly.
 */
template<typename F>
struct old_style_string_command : public F
{
    static void do_insert(HMENU handle, wstring caption, UINT command_id)
    {
        detail::win32::insert_menu(
            handle, 0, MF_BYPOSITION | MF_STRING, command_id, caption.c_str());
    }
};

/**
 * Inserts string command using the new API.
 *
 * This version specifies the MIIM_FTYPE flag which may be redundant.  MSDN
 * implies that only MIIM_STRING is needed.  Testing this combination to make
 * sure its presence doesn't affect our extraction.
 */
template<typename F>
struct new_style_string_command : public F
{
    static void do_insert(HMENU handle, wstring caption, UINT command_id)
    {
        do_insertion(
            handle, caption.c_str(), command_id, NULL,
            MIIM_FTYPE | MIIM_ID | MIIM_STRING,
            MFT_STRING);
    }
};

/**
 * Inserts string command using the new API.
 *
 * This version does *not* specifiy the MIIM_FTYPE flag that MSDN implies
 * may be redundant.  Testing this combination to make sure its absence
 * doesn't affect our extraction.
 */
template<typename F>
struct new_style_string_command_no_ftype_set : public F
{
    static void do_insert(HMENU handle, wstring caption, UINT command_id)
    {
        do_insertion(
            handle, caption.c_str(), command_id, NULL,
            MIIM_ID | MIIM_STRING, // No FTYPE flag
            MFT_STRING); // Zero so makes no difference
    }
};

typedef fixture_combinator<
    boost::mpl::vector<
        old_style_string_command<boost::mpl::_>,
        new_style_string_command<boost::mpl::_>,
        new_style_string_command_no_ftype_set<boost::mpl::_>
    >
>::type string_command_fixtures;

/**
 * Wrap a simple string command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    extract_command_item_with_string_button, F, string_command_fixtures )
{
    HMENU handle = detail::win32::create_popup_menu();

    F::do_insert(handle, L"Bob", 42);

    menu m(menu_handle::adopt_handle(handle));

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK(m[0]);

    shared_ptr<command_menu_item> item =
        boost::shared_polymorphic_cast<command_menu_item>(m[0]);

    BOOST_CHECK_EQUAL(item->command_id(), 42);

    const string_menu_button& button =
        dynamic_cast<const string_menu_button&>(item->button());

    BOOST_CHECK_EQUAL(button.caption(), L"Bob");

    F::do_test(m);
}

/**
 * Does insertion via the old insertion API.
 * The menu items are always retrieved using the new API so this tests that
 * the translation goes smoothly.
 */
template<typename F>
struct old_style_bitmap_command : public F
{
    static void do_insert(HMENU handle, HBITMAP bitmap, UINT command_id)
    {
        detail::win32::insert_menu(
            handle, 0, MF_BITMAP, command_id, (const wchar_t*)bitmap);
    }
};

/**
 * Inserts string command using the new API.
 *
 * This specifies the MIIM_TYPE flag because it is using the (legacy) 
 * whole-button bitmap.
 */
template<typename F>
struct new_style_bitmap_command : public F
{
    static void do_insert(HMENU handle, HBITMAP bitmap, UINT command_id)
    {
        do_insertion(
            handle, (const wchar_t*)bitmap, command_id, NULL,
            MIIM_TYPE | MIIM_ID, MFT_BITMAP);
    }
};

typedef fixture_combinator<
    boost::mpl::vector<
        old_style_bitmap_command<boost::mpl::_>,
        new_style_bitmap_command<boost::mpl::_>
    >
>::type bitmap_command_fixtures;

/**
 * Wrap a simple bitmap command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    extract_command_item_with_bitmap_button, F, bitmap_command_fixtures )
{
    HMENU handle = detail::win32::create_popup_menu();
    HBITMAP bitmap = test_bitmap();

    F::do_insert(handle, bitmap, 42);

    menu m(menu_handle::adopt_handle(handle));

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK(m[0]);

    shared_ptr<command_menu_item> item =
        boost::shared_polymorphic_cast<command_menu_item>(m[0]);

    BOOST_CHECK_EQUAL(item->command_id(), 42);

    const bitmap_menu_button& button =
        dynamic_cast<const bitmap_menu_button&>(item->button());

    BOOST_CHECK(button.bitmap() == bitmap);

    F::do_test(m);
}

/**
 * Does string popup insertion via the old insertion API.
 * The menu items are always retrieved using the new API so this tests that
 * the translation goes smoothly.
 */
template<typename F>
struct old_style_string_popup : public F
{
    static void do_insert(HMENU handle, wstring caption, HMENU sub_menu)
    {
        detail::win32::insert_menu(
            handle, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)sub_menu,
            caption.c_str());
    }
};

/**
 * Inserts string popup using the new API.
 */
template<typename F>
struct new_style_string_popup : public F
{
    static void do_insert(HMENU handle, wstring caption, HMENU sub_menu)
    {
        do_insertion(
            handle, caption.c_str(), 0, sub_menu,
            MIIM_SUBMENU | MIIM_STRING,
            MFT_STRING);
    }
};


typedef fixture_combinator<
    boost::mpl::vector<
        old_style_string_popup<boost::mpl::_>,
        new_style_string_popup<boost::mpl::_>
    >
>::type string_popup_fixtures;

/**
 * Wrap a string item that pops open a menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    extract_popup_item_with_string_button, F, string_popup_fixtures )
{
    HMENU handle = detail::win32::create_popup_menu();
    menu_handle submenu_handle = menu_handle::adopt_handle(
        detail::win32::create_popup_menu());
    do_insertion(
        submenu_handle.get(), L"Pop", 7, NULL, MIIM_STRING | MIIM_ID,
        MFT_STRING);

    F::do_insert(handle, L"Bob", submenu_handle.get());

    menu m(menu_handle::adopt_handle(handle));

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK(m[0]);

    shared_ptr<sub_menu> item =
        boost::shared_polymorphic_cast<sub_menu>(m[0]);

    const string_menu_button& button =
        dynamic_cast<const string_menu_button&>(item->button());

    BOOST_CHECK_EQUAL(button.caption(), L"Bob");

    menu& submenu = item->menu();
    BOOST_CHECK(submenu.valid());
    BOOST_CHECK_EQUAL(submenu.size(), 1);
    BOOST_CHECK(submenu.begin() != submenu.end());
    BOOST_CHECK(submenu[0]);

    shared_ptr<command_menu_item> sub_item =
        boost::shared_polymorphic_cast<command_menu_item>(submenu[0]);

    BOOST_CHECK_EQUAL(sub_item->command_id(), 7);
    BOOST_CHECK_EQUAL(
        dynamic_cast<const string_menu_button&>(sub_item->button()).caption(),
        L"Pop");

    F::do_test(m);
}

/** @todo  Could do tests for bitmap with popup menu here. */

/** @todo  Not tested owner-drawn buttons yet. */

template<typename F>
struct old_style_separator : public F
{
    static void do_insert(HMENU handle)
    {
        detail::win32::insert_menu(
            handle, 0, MF_BYPOSITION | MF_SEPARATOR, 0, (wchar_t*)(NULL));
    }
};

template<typename F>
struct new_style_separator : public F
{
    static void do_insert(HMENU handle)
    {
        do_insertion(handle, NULL, 0, NULL, MIIM_FTYPE, MFT_SEPARATOR);
    }
};

template<typename F>
struct weird_old_style_separator1 : public F
{
    static void do_insert(HMENU handle)
    {
        detail::win32::insert_menu(
            handle, 42, MF_BYPOSITION | MF_STRING | MF_SEPARATOR, 42,
            L"I'm an odd separator with a caption");
    }
};

template<typename F>
struct weird_new_style_separator1 : public F
{
    static void do_insert(HMENU handle)
    {
        do_insertion(
            handle, L"I'm an odd separator with a caption", 43, NULL,
            MIIM_FTYPE | MIIM_ID | MIIM_STRING,
            MFT_STRING | MFT_SEPARATOR);
    }
};

template<typename F>
struct weird_old_style_separator2 : public F
{
    static void do_insert(HMENU handle)
    {
        HMENU submenu = detail::win32::create_popup_menu();
        do_insertion(
            submenu, L"Supposedly, I'm a submenu of a separator", 7, NULL,
            MIIM_FTYPE | MIIM_ID | MIIM_STRING, MFT_STRING);

        detail::win32::insert_menu(
            handle, 42, MF_BYPOSITION | MF_SEPARATOR, (UINT_PTR)submenu,
            (const wchar_t*)NULL);
        detail::win32::enable_menu_item(handle, 0, MF_BYPOSITION);
    }
};

template<typename F>
struct weird_new_style_separator2 : public F
{
    static void do_insert(HMENU handle)
    {
        HMENU submenu = detail::win32::create_popup_menu();
        do_insertion(
            submenu, L"Supposedly, I'm a submenu of a separator", 7, NULL,
            MIIM_FTYPE | MIIM_ID | MIIM_STRING, MFT_STRING);

        do_insertion(
            handle, NULL, 101, submenu, MIIM_FTYPE | MIIM_SUBMENU,
            MFT_SEPARATOR);
        detail::win32::enable_menu_item(handle, 0, MF_BYPOSITION);
    }
};

typedef fixture_combinator<
    boost::mpl::vector<
        old_style_separator<boost::mpl::_>,
        new_style_separator<boost::mpl::_>,
        weird_old_style_separator1<boost::mpl::_>,
        weird_new_style_separator1<boost::mpl::_>,
        weird_old_style_separator2<boost::mpl::_>,
        weird_new_style_separator2<boost::mpl::_>
    >
>::type separator_fixtures;

/**
 * Wrap a separator.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( extract_separator_item, F, separator_fixtures )
{
    HMENU handle = detail::win32::create_popup_menu();
    F::do_insert(handle);

    menu m(menu_handle::adopt_handle(handle));

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK(m[0]);

    typedef const separator_menu_item item_type;

    BOOST_CHECK_NO_THROW(boost::shared_polymorphic_cast<item_type>(m[0]));

    F::do_test(m);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(menu_bar_tests)

/**
 * An empty menu bar; not attached to window.
 */
BOOST_AUTO_TEST_CASE( empty_menu_bar )
{
    menu_bar m;
    BOOST_CHECK_EQUAL(m.size(), 0);
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

    BOOST_CHECK_EQUAL(m.size(), 0);
    BOOST_CHECK(m.begin() == m.end());

    show_window(w);
}

BOOST_AUTO_TEST_CASE( string_command_in_bar )
{
    menu_bar b;

    b.append(command_menu_item(
        string_menu_button(L"string command in bar"), 14));

    BOOST_CHECK_EQUAL(b.size(), 1);
    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    show_window(w);
}

BOOST_AUTO_TEST_CASE( bitmap_command_in_bar )
{
    menu_bar b;

    command_menu_item item =
        command_menu_item(bitmap_menu_button(test_bitmap()), 13);

    b.append(item);

    BOOST_CHECK_EQUAL(b.size(), 1);
    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    show_window(w);
}

BOOST_AUTO_TEST_CASE( string_popup_in_bar )
{
    menu_bar b;

    menu m;
    m.append(command_menu_item(string_menu_button(L"Pop"), 1));

    sub_menu item =
        sub_menu(string_menu_button(L"string popup in bar"), m);

    b.append(item);

    BOOST_CHECK_EQUAL(b.size(), 1);
    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    show_window(w);
}

BOOST_AUTO_TEST_CASE( bitmap_popup_in_bar )
{
    menu_bar b;

    menu m;
    m.append(command_menu_item(string_menu_button(L"Pop"), 1));

    b.append(sub_menu(bitmap_menu_button(test_bitmap()), m));

    BOOST_CHECK(b.begin() != b.end());

    window<> w = create_test_window();
    w.menu(b);
    show_window(w);
}

BOOST_AUTO_TEST_SUITE_END();


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
    m.append(command_menu_item(string_menu_button(L"Child command"), 1));

    BOOST_CHECK(m.begin() != m.end());

    F::do_test(m);
}

/**
 * A bitmap command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( bitmap_command, F, fixtures )
{
    menu m;
    m.append(command_menu_item(bitmap_menu_button(test_bitmap()), 1));

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

    sub.append(command_menu_item(string_menu_button(L"Boo"), 1));

    m.append(sub_menu(string_menu_button(L"Pop"), sub));

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

    sub.append(command_menu_item(string_menu_button(L"Boo"), 1));

    m.append(sub_menu(bitmap_menu_button(test_bitmap()), sub));

    BOOST_CHECK(m.begin() != m.end());

    F::do_test(m);
}

/**
 * A menu with multiple items.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( mixed_items, F, fixtures )
{
    menu m;

    m.append(command_menu_item(string_menu_button(L"String command"), 1));

    m.append(command_menu_item(bitmap_menu_button(test_bitmap()), 2));

    m.append(separator_menu_item());

    menu sub;

    sub.append(command_menu_item(string_menu_button(L"String sub"), 3));

    sub.append(separator_menu_item());

    sub.append(command_menu_item(bitmap_menu_button(test_bitmap()), 4));

    m.append(sub_menu(string_menu_button(L"Lalala"), sub));

    F::do_test(m);
}

BOOST_AUTO_TEST_SUITE_END();
