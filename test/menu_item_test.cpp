/**
    @file

    Tests for menu item extraction.

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

#include "fixture_permutator.hpp"
#include "menu_fixtures.hpp"
#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/items.hpp> // test subject
#include <winapi/gui/menu/menu.hpp> // test subject

#include <winapi/gui/windows/window.hpp>

#include <boost/cast.hpp> // polymorphic_cast
#include <boost/mpl/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

using namespace winapi::gui::menu;
using winapi::gui::hwnd_t;
using winapi::gui::window;
using winapi::test::detail::do_insertion;
using winapi::test::detail::test_bitmap;
using winapi::test::fixture_permutator;
using winapi::test::menu_fixtures;
using winapi::test::menu_ownership_fixtures;

using boost::dynamic_pointer_cast;
using boost::shared_ptr;
using boost::shared_polymorphic_cast;

using std::string;
using std::wstring;

namespace {

    class selectable_state_test : public menu_item_visitor<>
    {
    public:

        void operator()(separator_menu_item&)
        {
            BOOST_FAIL("Separator unexpected");
        }

        void operator()(selectable_menu_item& item)
        {
            BOOST_CHECK(item.is_enabled());
            BOOST_CHECK(!item.is_default());
            BOOST_CHECK(!item.is_highlighted());
            BOOST_CHECK(!item.is_checked());
        }
    };

    class is_separator_test : public menu_item_visitor<>
    {
    public:

        void operator()(separator_menu_item&)
        {
        }

        void operator()(command_menu_item&)
        {
            BOOST_FAIL("Command item unexpected");
        }

        void operator()(sub_menu&)
        {
            BOOST_FAIL("Sub-menu unexpected");
        }
    };

    class is_command_test : public menu_item_visitor<>
    {
    public:

        void operator()(separator_menu_item&)
        {
            BOOST_FAIL("Separator unexpected");
        }

        void operator()(command_menu_item&)
        {
        }

        void operator()(sub_menu&)
        {
            BOOST_FAIL("Sub-menu unexpected");
        }
    };

    class is_sub_menu_test : public menu_item_visitor<>
    {
    public:

        void operator()(separator_menu_item&)
        {
            BOOST_FAIL("Separator unexpected");
        }

        void operator()(command_menu_item&)
        {
            BOOST_FAIL("Command item unexpected");
        }

        void operator()(sub_menu&)
        {
        }
    };

    class command_id_test : public menu_item_visitor<>
    {
    public:
        command_id_test(UINT command_id) : m_command_id(command_id) {}

        void operator()(command_menu_item& item)
        {
            BOOST_CHECK_EQUAL(item.command_id(), m_command_id);
        }

        template<typename T>
        void operator()(T&)
        {
            BOOST_FAIL("Unexpected");
        }

    private:
        UINT m_command_id;
    };

    class sub_menu_test : public menu_item_visitor<>
    {
    public:

        template<typename T>
        void operator()(T&)
        {
            BOOST_FAIL("Unexpected");
        }

        void operator()(sub_menu& item)
        {
            menu& submenu = item.menu();
            BOOST_CHECK(submenu.valid());

            /*
             * TODO: Add something here that tests the menu is the same one
             * as one taken in the constructor.
             */
        }
    };

    class string_button_test : public menu_item_visitor<>
    {
    public:
        string_button_test(const wstring& caption) : m_caption(caption) {}

        void operator()(selectable_menu_item& item)
        {
            boost::shared_ptr<string_menu_button> button =
                dynamic_pointer_cast<string_menu_button>(item.button());

            BOOST_CHECK_EQUAL(button->caption(), m_caption);
        }

        void operator()(separator_menu_item&)
        {
            BOOST_FAIL("Unexpected separator");
        }

    private:
        wstring m_caption;
    };

    class bitmap_button_test : public menu_item_visitor<>
    {
    public:
        bitmap_button_test(HBITMAP bitmap) : m_bitmap(bitmap) {}

        void operator()(selectable_menu_item& item)
        {
            boost::shared_ptr<bitmap_menu_button> button =
                dynamic_pointer_cast<bitmap_menu_button>(item.button());

            BOOST_CHECK_EQUAL(button->bitmap(), m_bitmap);
        }

        void operator()(separator_menu_item&)
        {
            BOOST_FAIL("Unexpected separator");
        }

    private:
        HBITMAP m_bitmap;
    };
}

/**
 * Tests where the menu was created externally and passed to the wrapper as a
 * raw menu.
 *
 * These test make sure that, given a menu, we can extra items from it
 * correctly.
 */
BOOST_AUTO_TEST_SUITE(menu_item_extraction_tests)

/**
 * Wrap an empty menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE( empty_menu, F, menu_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

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
    static void do_insert(menu_handle handle, wstring caption, UINT command_id)
    {
        detail::win32::insert_menu(
            handle.get(), 0, MF_BYPOSITION | MF_STRING, command_id,
            caption.c_str());
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
    static void do_insert(menu_handle handle, wstring caption, UINT command_id)
    {
        do_insertion(
            handle.get(), caption.c_str(), command_id, NULL,
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
    static void do_insert(menu_handle handle, wstring caption, UINT command_id)
    {
        do_insertion(
            handle.get(), caption.c_str(), command_id, NULL,
            MIIM_ID | MIIM_STRING, // No FTYPE flag
            MFT_STRING); // Zero so makes no difference
    }
};

typedef fixture_permutator<
    boost::mpl::vector<
        old_style_string_command<boost::mpl::_>,
        new_style_string_command<boost::mpl::_>,
        new_style_string_command_no_ftype_set<boost::mpl::_>
    >,
    menu_fixtures
>::type string_command_fixtures;

/**
 * Wrap a simple string command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    extract_command_item_with_string_button, F, string_command_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    F::do_insert(t.handle(), L"Bob", 42);

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK_NO_THROW(m[0]);

    m[0].accept(is_command_test());
    m[0].accept(selectable_state_test());
    m[0].accept(command_id_test(42));
    m[0].accept(string_button_test(L"Bob"));

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
    static void do_insert(menu_handle handle, HBITMAP bitmap, UINT command_id)
    {
        detail::win32::insert_menu(
            handle.get(), 0, MF_BITMAP, command_id, (const wchar_t*)bitmap);
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
    static void do_insert(menu_handle handle, HBITMAP bitmap, UINT command_id)
    {
        do_insertion(
            handle.get(), (const wchar_t*)bitmap, command_id, NULL,
            MIIM_TYPE | MIIM_ID, MFT_BITMAP);
    }
};

typedef fixture_permutator<
    boost::mpl::vector<
        old_style_bitmap_command<boost::mpl::_>,
        new_style_bitmap_command<boost::mpl::_>
    >,
    menu_fixtures
>::type bitmap_command_fixtures;

/**
 * Wrap a simple bitmap command.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    extract_command_item_with_bitmap_button, F, bitmap_command_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    HBITMAP bitmap = test_bitmap();

    F::do_insert(t.handle(), bitmap, 42);

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK_NO_THROW(m[0]);

    m[0].accept(is_command_test());
    m[0].accept(selectable_state_test());
    m[0].accept(command_id_test(42));
    m[0].accept(bitmap_button_test(bitmap));

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


typedef fixture_permutator<
    boost::mpl::vector<
        old_style_string_popup<boost::mpl::_>,
        new_style_string_popup<boost::mpl::_>
    >,
    menu_fixtures
>::type string_popup_fixtures;

/**
 * Wrap a string item that pops open a menu.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    extract_popup_item_with_string_button, F, string_popup_fixtures )
{
    F::test_menu t = F::create_menu_to_test();
    F::menu_type m = t.menu();

    menu_handle submenu_handle = menu_handle::adopt_handle(
        detail::win32::create_popup_menu());
    do_insertion(
        submenu_handle.get(), L"Pop", 7, NULL, MIIM_STRING | MIIM_ID,
        MFT_STRING);

    F::do_insert(t.handle().get(), L"Bob", submenu_handle.get());

    BOOST_CHECK(m.begin() != m.end());
    BOOST_CHECK_NO_THROW(m[0]);

    m[0].accept(is_sub_menu_test());
    m[0].accept(selectable_state_test());
    m[0].accept(string_button_test(L"Bob"));
    m[0].accept(sub_menu_test());

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

typedef fixture_permutator<
    boost::mpl::vector<
        old_style_separator<boost::mpl::_>,
        new_style_separator<boost::mpl::_>,
        weird_old_style_separator1<boost::mpl::_>,
        weird_new_style_separator1<boost::mpl::_>,
        weird_old_style_separator2<boost::mpl::_>,
        weird_new_style_separator2<boost::mpl::_>
    >,
    menu_ownership_fixtures
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
    BOOST_CHECK_NO_THROW(m[0]);

    m[0].accept(is_separator_test());

    F::do_test(m);
}

BOOST_AUTO_TEST_SUITE_END()
