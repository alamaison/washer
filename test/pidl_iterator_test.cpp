/**
    @file

    Unit tests for PIDL iterators.

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

#include "wchar_output.hpp" // wstring output

#include <winapi/shell/pidl_iterator.hpp> // test subject

#include <winapi/shell/shell.hpp> // special_folder_pidl

#include <boost/test/unit_test.hpp>

#include <algorithm> // copy
#include <cstring> // strlen
#include <vector>

using namespace winapi::shell::pidl;
using winapi::shell::special_folder_pidl;

using boost::test_tools::predicate_result;

using std::copy;
using std::strlen;
using std::vector;

namespace {

    /**
     * Create a dummy PIDL using some predefined text that we can later
     * use to compare with.
     */
    cpidl_t child_pidl_from_text(const std::string& text)
    {
        size_t pidl_length =
            // size field + string length + terminator
            sizeof(USHORT) + strlen(text.c_str()) + sizeof(SHITEMID);
        vector<char> buffer(pidl_length, '\0');
        copy(
            text.c_str(), text.c_str() + strlen(text.c_str()),
            &buffer[sizeof(USHORT)]);

        PUITEMID_CHILD pidl = reinterpret_cast<PUITEMID_CHILD>(&buffer[0]);
        pidl->mkid.cb = pidl_length - sizeof(SHITEMID);
        return cpidl_t(pidl);
    }
    
    /**
     * Check that the PIDL contains the text we expect.
     */
    predicate_result pidl_matches_text(
        PCUIDLIST_RELATIVE pidl, const std::string& text)
    {
        size_t data_length =
            (pidl->mkid.cb < sizeof(USHORT)) ?
                0 : pidl->mkid.cb - sizeof(USHORT);
        const char* data = reinterpret_cast<const char*>(pidl) + sizeof(USHORT);
        return boost::test_tools::tt_detail::equal_coll_impl(
            data, data + data_length, text.c_str(),
            text.c_str() + strlen(text.c_str()));
    }
}

BOOST_AUTO_TEST_SUITE(raw_pidl_iterator_tests)

/**
 * Default constructor should result in wrapped PIDL being NULL.
 */
BOOST_AUTO_TEST_CASE( default_construct )
{
    raw_pidl_iterator it;
    BOOST_CHECK(it == raw_pidl_iterator());
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 0);
}

/**
 * Default constructor should result in wrapped PIDL being NULL.
 */
BOOST_AUTO_TEST_CASE( desktop_root )
{
    apidl_t desktop = special_folder_pidl(CSIDL_DESKTOP);
    raw_pidl_iterator it(desktop.get());
    BOOST_CHECK(it == raw_pidl_iterator());
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 0);
}

/**
 * Constructing an iterator with a child PIDL returns that item.

 * In addition, incrementing the iterator should work once and should cause
 * the iterator to be equal to the default-constructed version, i.e., should
 * signify the end of the iteration.
 */
BOOST_AUTO_TEST_CASE( iterate_over_child_pidl )
{
    cpidl_t p = child_pidl_from_text("Elizabeth");

    raw_pidl_iterator it(p.get());
    BOOST_CHECK(it != raw_pidl_iterator());
    BOOST_CHECK(pidl_matches_text(*it, "Elizabeth"));

    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 1);

    ++it;
    BOOST_CHECK(it == raw_pidl_iterator());

    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 0);

    BOOST_CHECK_THROW(++it, std::range_error);
}

/**
 * Constructing an iterator with a multi-item PIDL points to each item of the
 * PIDL in turn.
 */
BOOST_AUTO_TEST_CASE( iterate_over_multi_item_pidl )
{
    apidl_t p =
        apidl_t() + child_pidl_from_text("Mary") + child_pidl_from_text("had")
        + child_pidl_from_text("a") + child_pidl_from_text("little")
        + child_pidl_from_text("lamb");

    raw_pidl_iterator it(p.get());
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 5);
    BOOST_CHECK(pidl_matches_text(*it++, "Mary"));
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 4);
    BOOST_CHECK(pidl_matches_text(*it++, "had"));
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 3);
    BOOST_CHECK(pidl_matches_text(*it++, "a"));
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 2);
    BOOST_CHECK(pidl_matches_text(*it++, "little"));
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 1);
    BOOST_CHECK(pidl_matches_text(*it++, "lamb"));
    BOOST_CHECK_EQUAL(std::distance(it, raw_pidl_iterator()), 0);
    BOOST_CHECK(it == raw_pidl_iterator());
}

BOOST_AUTO_TEST_SUITE_END()
