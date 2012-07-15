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

#include "button_test_visitors.hpp"
#include "item_test_visitors.hpp"
#include "menu_fixtures.hpp"
#include "wchar_output.hpp" // wchar_t test output

#include <winapi/gui/menu/button/bitmap_button_description.hpp>
#include <winapi/gui/menu/button/string_button_description.hpp>
#include <winapi/gui/menu/item/command_item.hpp> // test subject
#include <winapi/gui/menu/item/command_item_description.hpp> // test subject
#include <winapi/gui/menu/item/item.hpp> // test subject
#include <winapi/gui/menu/item/separator_item.hpp> // test subject
#include <winapi/gui/menu/item/separator_item_description.hpp> // test subject
#include <winapi/gui/menu/item/sub_menu_item.hpp> // test subject
#include <winapi/gui/menu/item/sub_menu_item_description.hpp> // test subject
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
        sub_menu_item_description(
            string_button_description(L"Menu being tested is under here"), m));
    use_in_context(b);
}

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
        m[0].accept(id_test(96));
    }
};

template<typename F>
class popup : public F
{
public:

    typedef sub_menu_item_description description_type;

    template<typename B>
    description_type create_description(const B& button)
    {
        m_sub = menu();
        m_sub.insert(
            command_item_description(string_button_description(L"Boo"), 987987));

        sub_menu_item_description description =
            sub_menu_item_description(button, m_sub);
        description.id(6565);

        return description;
    }

    template<typename M>
    void do_item_type_test(const M& m)
    {
        m[0].accept(sub_menu_test(m_sub));
        m[0].accept(id_test(6565));
    }
    
private:

    menu m_sub;
};

typedef boost::mpl::vector< command<boost::mpl::_>, popup<boost::mpl::_> >
    item_description_type_fixtures;

template<typename F>
struct string_b : public F
{
    typedef string_button_description button_type;

    button_type create_button()
    {
        return string_button_description(L"String button");
    }

    template<typename M>
    void do_button_test(const M& m)
    {
        m[0].accept(make_button_test(string_button_test(L"String button")));
    }

    template<typename M>
    void mutate_button(M m)
    {
        m[0].accept(
            make_button_test(string_button_mutator(L"Mutated button text")));
    }

    template<typename M>
    void do_mutated_button_test(const M& m)
    {
        m[0].accept(
            make_button_test(string_button_test(L"Mutated button text")));
    }
};

template<typename F>
class bitmap_b : public F
{
public:
    typedef bitmap_button_description button_type;

    bitmap_b() : m_hbitmap1(test_bitmap()), m_hbitmap2(test_bitmap()) {}

    button_type create_button()
    {
        return bitmap_button_description(m_hbitmap1);
    }

    template<typename M>
    void do_button_test(const M& m)
    {
        m[0].accept(make_button_test(bitmap_button_test(m_hbitmap1)));
    }

    template<typename M>
    void mutate_button(M m)
    {
        m[0].accept(make_button_test(bitmap_button_mutator(m_hbitmap2)));
    }

    template<typename M>
    void do_mutated_button_test(const M& m)
    {
        m[0].accept(make_button_test(bitmap_button_test(m_hbitmap2)));
    }

    HBITMAP m_hbitmap1;
    HBITMAP m_hbitmap2;
};

typedef boost::mpl::vector<
    string_b<boost::mpl::_>, bitmap_b<boost::mpl::_> > button_fixtures;

template<typename F>
struct enabled : public F
{
    template<typename D>
    void set_selectability(D& d)
    {
        d.selectability(selectability::enabled);
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
        d.selectability(selectability::disabled);
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
        d.selectability(selectability::default);
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
    void set_selectability(D&)
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

// One day, with spiffy compilers, we can do away with the reduced version
typedef boost::mpl::vector< enabled<boost::mpl::_>, disabled<boost::mpl::_> >
    reduced_selectability_fixtures;

template<typename F>
struct checked : public F
{
    template<typename D>
    void set_check_state(D& d)
    {
        d.check_mark_visibility(check_mark::visible);
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
        d.check_mark_visibility(check_mark::invisible);
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
        d.check_mark_visibility(check_mark::default);
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
    void set_check_state(D&)
    {
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

// One day, with spiffy compilers, we can do away with the reduced version
typedef boost::mpl::vector< checked<boost::mpl::_>, unchecked<boost::mpl::_> >
    reduced_checkedness_fixtures;

typedef fixture_permutator<
    fixture_permutator<
        fixture_permutator<
            fixture_permutator<
                menu_type_fixtures, item_description_type_fixtures>::type,
            checkedness_fixtures>::type,
        selectability_fixtures>::type,
    button_fixtures>::type selectable_item_test_permutations;


template<typename F>
struct mutate_to_enabled : public F
{
    template<typename I>
    void mutate_selectability(I i)
    {
        i.accept(selectability_mutator(selectability::enabled));
    }

    template<typename M>
    void do_mutated_selectability_test(const M& m)
    {
        m[0].accept(selectability_test(true));
    }
};

template<typename F>
struct mutate_to_disabled : public F
{
    template<typename I>
    void mutate_selectability(I i)
    {
        i.accept(selectability_mutator(selectability::disabled));
    }

    template<typename M>
    void do_mutated_selectability_test(const M& m)
    {
        m[0].accept(selectability_test(false));
    }
};

typedef boost::mpl::vector<
    mutate_to_enabled<boost::mpl::_>, mutate_to_disabled<boost::mpl::_> >
    selectability_mutation_fixtures;

template<typename F>
struct mutate_to_checked : public F
{
    template<typename I>
    void mutate_check_state(I i)
    {
        i.accept(check_mutator(check_mark::visible));
    }

    template<typename M>
    void do_mutated_check_state_test(const M& m)
    {
        m[0].accept(checkedness_test(true));
    }
};

template<typename F>
struct mutate_to_unchecked : public F
{
    template<typename I>
    void mutate_check_state(I i)
    {
        i.accept(check_mutator(check_mark::invisible));
    }

    template<typename M>
    void do_mutated_check_state_test(const M& m)
    {
        m[0].accept(checkedness_test(false));
    }
};

typedef boost::mpl::vector<
    mutate_to_checked<boost::mpl::_>, mutate_to_unchecked<boost::mpl::_> >
    checkedness_mutation_fixtures;

BOOST_AUTO_TEST_SUITE(menu_item_tests)

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

// Using the reduced versions of the description creators as, for these tests,
// we aren't really interested in the details of all possible description
// uses.  We just want one that starts with the property set and another that
// starts with the property unset.
//
// Also we don't vary the button type though this would be a good thing to do
typedef fixture_permutator<
    fixture_permutator<
        fixture_permutator<
            fixture_permutator<
                fixture_permutator<
                    item_description_type_fixtures,
                    reduced_checkedness_fixtures>::type,
                reduced_selectability_fixtures>::type,
            checkedness_mutation_fixtures>::type,
        selectability_mutation_fixtures>::type,
    button_fixtures>::type
selectable_item_mutation_test_permutations;

/**
 * Test every permutation of mutation on the created menu items and their state
 * after the mutation.
 *
 * @todo If, one day, compilers can handle longer template lists, add menu type
 *       back into the list of permutations.
 *
 * THIS GENERATES HUNDREDS OF TEST CASES.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(
    item_mutation_test, F, selectable_item_mutation_test_permutations )
{
    menu m;

    F f;
    F::button_type b = f.create_button();
    F::description_type d = f.create_description(b);

    f.set_selectability(d);
    f.set_check_state(d);

    m.insert(d);

    f.do_item_type_test(m);
    f.do_selectability_test(m);
    f.do_check_state_test(m);
    f.do_button_test(m);

    f.mutate_selectability(m[0]);
    f.do_mutated_selectability_test(m);
    f.do_item_type_test(m);
    f.do_check_state_test(m);
    f.do_button_test(m);

    f.mutate_check_state(m[0]);
    f.do_mutated_check_state_test(m);
    f.do_item_type_test(m);
    f.do_mutated_selectability_test(m);
    f.do_button_test(m);

    f.mutate_button(m);
    f.do_mutated_button_test(m);
    f.do_item_type_test(m);
    f.do_mutated_selectability_test(m);
    f.do_mutated_check_state_test(m);

    use_in_context(m);
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_AUTO_TEST_SUITE(non_generated_item_description_tests)

/**
 * A separator.
 */
BOOST_AUTO_TEST_CASE( separator )
{
    menu m;
    separator_item_description d;
    d.id(56984950);
    m.insert(d);
    m[0].accept(is_separator_test());
    m[0].accept(id_test(56984950));
}

/**
 * A menu with multiple items.
 */
BOOST_AUTO_TEST_CASE( mixed_items )
{
    menu m;

    HBITMAP bitmap = test_bitmap();

    m.insert(
        command_item_description(
            string_button_description(L"String command"), 1));

    m.insert(command_item_description(bitmap_button_description(bitmap), 2));

    m.insert(separator_item_description());

    menu sub;

    sub.insert(
        command_item_description(string_button_description(L"String sub"), 3));

    sub.insert(separator_item_description());

    sub.insert(command_item_description(bitmap_button_description(bitmap), 4));

    m.insert(
        sub_menu_item_description(string_button_description(L"Lalala"), sub));

    m[0].accept(id_test(1));
    m[0].accept(make_button_test(string_button_test(L"String command")));

    m[1].accept(id_test(2));
    m[1].accept(make_button_test(bitmap_button_test(bitmap)));

    m[2].accept(is_separator_test());
    m[2].accept(id_test(0));

    m[3].accept(sub_menu_test(sub));
    m[2].accept(id_test(0));
    m[3].accept(make_button_test(string_button_test(L"Lalala")));
}

BOOST_AUTO_TEST_SUITE_END();