/**
    @file

    Tests for description classes building new menu items.

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

/**
 * @file
 *
 * This file generates tests using the awesomeness of Boost.MPL such that
 * every orthogonal aspect of menu item descriptions is tested in combination
 * with every other orthogonal aspect.
 *
 * Each orthogonal aspect is represented by a boost::mpl::vector containing
 * its individual possibilities, each a fragment of a test fixtures that
 * sets/tests that possibility.  If two things are orthogonal, they should
 * not share one of these boost::mpl::vectors but, instead, be held in another
 * vector and permutated with the other orthoganal aspects.
 */

#include "item_test_visitors.hpp"
#include "menu_fixtures.hpp"
#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/item_descriptions.hpp> // test subject
#include <winapi/gui/menu/items.hpp> // test subject
#include <winapi/gui/menu/menu.hpp> // test subject

#include <boost/mpl/aux_/config/ctps.hpp>
#include <boost/mpl/vector/vector50.hpp> // max vector to extend
#include <boost/preprocessor/iterate.hpp> // BOOST_PP_ITER*
#include <boost/test/unit_test.hpp>

// This extends the maximum Boost.MPL vector from 20 to 200 elements.
// Obviously this puts a huge strain on the compiler but as this is only used
// for testing and the benefit (being able to test every permutation of
// menu item description state) is huge, the tradeoff seems acceptable.
// Especially if you consider that a fairly old compiler MSVC8 (VS2005) can
// handle this gracefully and compilers are only going to be getting more
// robust and efficient.
namespace boost {
namespace mpl {
#define BOOST_PP_ITERATION_PARAMS_1 \
    (3,(51, 200, "boost/mpl/vector/aux_/numbered.hpp"))
#include BOOST_PP_ITERATE()
}}

using namespace winapi::gui::menu;
using namespace winapi::test;

// All these fixtures are in the global namespace in order to keep the
// displayed name of the generated fixture permutations as short as possible

template<typename F>
struct command : public F
{
    typedef command_item_description description_type;

    template<typename B>
    description_type create_description(const B& button)
    {
        return command_item_description(button, 96);
    }

    template<typename M>
    void do_item_type_test(const M& m)
    {
        m[0].accept(command_id_test(96));
    }
};

template<typename F>
class popup : public F
{
public:

    typedef sub_menu_description description_type;

    template<typename B>
    description_type create_description(const B& button)
    {
        m_sub = menu();
        m_sub.insert(
            command_item_description(string_menu_button(L"Boo"), 987987));

        return sub_menu_description(button, m_sub);
    }

    template<typename M>
    void do_item_type_test(const M& m)
    {
        m[0].accept(sub_menu_test(m_sub));
    }
    
private:

    menu m_sub;
};

typedef boost::mpl::vector< command<boost::mpl::_>, popup<boost::mpl::_> >
    item_description_type_fixtures;

template<typename F>
struct string_button : public F
{
    typedef string_menu_button button_type;

    button_type create_button()
    {
        return string_menu_button(L"String button");
    }

    template<typename M>
    void do_button_test(const M& m)
    {
        m[0].accept(string_button_test(L"String button"));
    }
};

template<typename F>
class bitmap_button : public F
{
public:
    typedef bitmap_menu_button button_type;

    bitmap_button() : m_hbitmap(test_bitmap()) {}

    button_type create_button()
    {
        return bitmap_menu_button(m_hbitmap);
    }

    template<typename M>
    void do_button_test(const M& m)
    {
        m[0].accept(bitmap_button_test(m_hbitmap));
    }

    HBITMAP m_hbitmap;
};

typedef boost::mpl::vector<
    string_button<boost::mpl::_>, bitmap_button<boost::mpl::_>
> button_fixtures;

template<typename F>
struct enabled : public F
{
    template<typename D>
    void set_selectability(D& d)
    {
        d.button_state(selectability::enabled);
    }

    template<typename M>
    void do_selectability_test(const M& m)
    {
        m[0].accept(selectability_test(true));
    }
};

template<typename F>
struct disabled : public F
{
    template<typename D>
    void set_selectability(D& d)
    {
        d.button_state(selectability::disabled);
    }

    template<typename M>
    void do_selectability_test(const M& m)
    {
        m[0].accept(selectability_test(false));
    }
};

template<typename F>
struct default_enabled : public F
{
    template<typename D>
    void set_selectability(D& d)
    {
        d.button_state(selectability::default);
    }

    template<typename M>
    void do_selectability_test(const M& m)
    {
        m[0].accept(selectability_test(true));
    }
};

template<typename F>
struct noop_enabled : public F
{
    template<typename D>
    void set_selectability(D& d)
    {
    }

    template<typename M>
    void do_selectability_test(const M& m)
    {
        m[0].accept(selectability_test(true));
    }
};

typedef boost::mpl::vector<
    enabled<boost::mpl::_>, disabled<boost::mpl::_>,
    default_enabled<boost::mpl::_>, noop_enabled<boost::mpl::_> >
    selectability_fixtures;

template<typename F>
struct checked : public F
{
    template<typename D>
    void set_check_state(D& d)
    {
        d.check_mark(checkedness::checked);
    }

    template<typename M>
    void do_check_state_test(const M& m)
    {
        m[0].accept(checkedness_test(true));
    }
};

template<typename F>
struct unchecked : public F
{
    template<typename D>
    void set_check_state(D& d)
    {
        d.check_mark(checkedness::unchecked);
    }

    template<typename M>
    void do_check_state_test(const M& m)
    {
        m[0].accept(checkedness_test(false));
    }
};

template<typename F>
struct default_unchecked : public F
{
    template<typename D>
    void set_check_state(D& d)
    {
        d.check_mark(checkedness::default);
    }

    template<typename M>
    void do_check_state_test(const M& m)
    {
        m[0].accept(checkedness_test(false));
    }
};

template<typename F>
struct noop_unchecked : public F
{
    template<typename D>
    void set_check_state(D& d)
    {
        d.check_mark(checkedness::default);
    }

    template<typename M>
    void do_check_state_test(const M& m)
    {
        m[0].accept(checkedness_test(false));
    }
};

typedef boost::mpl::vector<
    checked<boost::mpl::_>, unchecked<boost::mpl::_>,
    default_unchecked<boost::mpl::_>, noop_unchecked<boost::mpl::_> >
    checkedness_fixtures;


typedef fixture_permutator<
    fixture_permutator<
        fixture_permutator<
            fixture_permutator<
                menu_type_fixtures, item_description_type_fixtures>::type,
            checkedness_fixtures>::type,
        selectability_fixtures>::type,
    button_fixtures>::type selectable_item_test_permutations;

BOOST_AUTO_TEST_SUITE(menu_description_tests)

void use_in_context(const menu_bar& m)
{    
    winapi::gui::window<> w = winapi::test::detail::create_test_window();
    w.menu(m);
    winapi::test::detail::show_window(w);
}

void use_in_context(const menu& m)
{    
    menu_bar b;
    b.insert(
        sub_menu_description(
            string_menu_button(L"Menu being tested is under here"), m));
    use_in_context(b);
}

/**
 * Test every permutation of the menu item descriptions and all their state
 * variables in both menus and menu bars with every type of button.
 *
 * THIS GENERATES HUNDREDS OF TEST CASES.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    description_test, F, selectable_item_test_permutations )
{
    F f;
    F::menu_type m = F::create_menu_to_test();
    F::button_type b = f.create_button();
    F::description_type d = f.create_description(b);

    f.set_selectability(d);
    f.set_check_state(d);

    m.insert(d);

    f.do_item_type_test(m);
    f.do_button_test(m);
    f.do_selectability_test(m);
    f.do_check_state_test(m);

    use_in_context(m);
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_AUTO_TEST_SUITE(non_generated_menu_item_description_tests)

/**
 * A separator.
 */
BOOST_AUTO_TEST_CASE( separator )
{
    menu m;
    m.insert(separator_description());
    m[0].accept(is_separator_test());
}

/**
 * A menu with multiple items.
 */
BOOST_AUTO_TEST_CASE( mixed_items )
{
    menu m;

    HBITMAP bitmap = test_bitmap();

    m.insert(command_item_description(string_menu_button(L"String command"), 1));

    m.insert(command_item_description(bitmap_menu_button(bitmap), 2));

    m.insert(separator_description());

    menu sub;

    sub.insert(command_item_description(string_menu_button(L"String sub"), 3));

    sub.insert(separator_description());

    sub.insert(command_item_description(bitmap_menu_button(bitmap), 4));

    m.insert(sub_menu_description(string_menu_button(L"Lalala"), sub));

    m[0].accept(command_id_test(1));
    m[0].accept(string_button_test(L"String command"));

    m[1].accept(command_id_test(2));
    m[1].accept(bitmap_button_test(bitmap));

    m[2].accept(is_separator_test());

    m[3].accept(sub_menu_test(sub));
    m[3].accept(string_button_test(L"Lalala"));
}

BOOST_AUTO_TEST_SUITE_END();