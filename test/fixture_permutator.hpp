/**
    @file

    Create permutations of test fixtures.

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

#ifndef WINAPI_TEST_FIXTURE_COMBINATOR_HPP
#define WINAPI_TEST_FIXTURE_COMBINATOR_HPP
#pragma once

#include <boost/mpl/apply.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>

namespace winapi {
namespace test {

namespace detail {

template<typename Outer, typename Inner>
struct fixtures_loop
{
    BOOST_MPL_ASSERT(( boost::mpl::is_sequence<Inner> ));

    template<typename T>
    struct use : boost::mpl::apply<Outer, T> {};

    typedef typename boost::mpl::fold<
        Inner, boost::mpl::vector<>,
        boost::mpl::push_back< boost::mpl::_1, use<boost::mpl::_2> >
    >::type type;

    BOOST_MPL_ASSERT(( boost::mpl::is_sequence<type> ));
};

}

/**
 * Produces all possible permutations of two lists of fixtures.
 *
 * The lists are combined by templated inheritance so the `Outer` list fixtures
 * but take a template parameter and inherit from it.  This is instantiated
 * with the items from the `Inner` list.
 */
template<typename Outer, typename Inner>
struct fixture_permutator
{
    BOOST_MPL_ASSERT(( boost::mpl::is_sequence<Outer> ));
    BOOST_MPL_ASSERT(( boost::mpl::is_sequence<Inner> ));

    typedef typename boost::mpl::fold<
        Outer, boost::mpl::vector<>,
        boost::mpl::copy<
            detail::fixtures_loop<boost::mpl::_2, Inner>,
            boost::mpl::back_inserter<boost::mpl::_1>
        >
    >::type type;

    BOOST_MPL_ASSERT(( boost::mpl::is_sequence<type> ));
};

}}

#endif
