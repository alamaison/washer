/**
    @file

    Tests for IProgressDialog wrapper.

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

#include <winapi/gui/progress.hpp> // test subject

#include <comet/util.h> // auto_coinit

#include <boost/test/unit_test.hpp>

using winapi::gui::progress;

using comet::auto_coinit;

namespace {

    class com_fixture
    {
    private:
        auto_coinit com;
    };

    //const int SLEEP_TIME = 5000;
    const int SLEEP_TIME = 5;

}

BOOST_FIXTURE_TEST_SUITE(progress_tests, com_fixture)

/**
 * Progress dialogue text display.
 */
BOOST_AUTO_TEST_CASE( text )
{
    progress dialogue(
        NULL, L"Test title", progress::modality::modal, 
        progress::time_estimation::none, progress::bar_type::progress,
        progress::minimisable::yes, progress::cancellability::cancellable);
    dialogue.line(1, L"Text 1");
    dialogue.line(2, L"Text 2");
    dialogue.line(3, L"Text 3");
    for (int i = 0; i <= SLEEP_TIME; ++i)
    {
        dialogue.update(i, SLEEP_TIME);
        ::Sleep(1);
    }
}

/**
 * Progress dialogue long path display.
 */
BOOST_AUTO_TEST_CASE( path )
{
    progress dialogue(
        NULL, L"Test title", progress::modality::modal, 
        progress::time_estimation::none, progress::bar_type::progress,
        progress::minimisable::yes, progress::cancellability::cancellable);
    dialogue.line(1, L"Text 1");
    dialogue.line_as_compressable_path(
        2, L"C:\\a\\very\\extremely\\indubitably\\"
        L"supercalifragilistically\\expialidociously\\long\\path");
    dialogue.line_as_compressable_path(
        3, L"/a/very/very/extremely/indubitably/"
        L"supercalifragilistically/expialidociously/long/unix/path");
    for (int i = 0; i <= SLEEP_TIME; ++i)
    {
        dialogue.update(i, SLEEP_TIME);
        ::Sleep(1);
    }
}

/**
 * Progress dialogue with automatic time estimation.
 */
BOOST_AUTO_TEST_CASE( text_with_time )
{
    progress dialogue(
        NULL, L"Test title", progress::modality::modal, 
        progress::time_estimation::automatic_time_estimate,
        progress::bar_type::progress,
        progress::minimisable::yes, progress::cancellability::cancellable);
    dialogue.line(1, L"Text 1");
    dialogue.line(2, L"Text 2");
    for (int i = 0; i <= SLEEP_TIME; ++i)
    {
        dialogue.update(i, SLEEP_TIME);
        ::Sleep(3);
    }
}

/**
 * Progress dialogue with time and marquee.
 */
BOOST_AUTO_TEST_CASE( text_with_time_and_marquee )
{
    progress dialogue(
        NULL, L"Test title", progress::modality::modal, 
        progress::time_estimation::automatic_time_estimate,
        progress::bar_type::marquee,
        progress::minimisable::yes, progress::cancellability::cancellable);
    dialogue.line(1, L"Text 1");
    dialogue.line(2, L"Text 2");
    for (int i = 0; i <= SLEEP_TIME; ++i)
    {
        dialogue.update(i, SLEEP_TIME);
        ::Sleep(3);
    }
}

/**
 * Progress dialogue with time but no progress bar.
 */
BOOST_AUTO_TEST_CASE( text_with_time_only )
{
    progress dialogue(
        NULL, L"Test title", progress::modality::modal, 
        progress::time_estimation::automatic_time_estimate,
        progress::bar_type::none,
        progress::minimisable::yes, progress::cancellability::cancellable);
    dialogue.line(1, L"Text 1");
    dialogue.line(2, L"Text 2");
    for (int i = 0; i <= SLEEP_TIME; ++i)
    {
        dialogue.update(i, SLEEP_TIME);
        ::Sleep(3);
    }
}

BOOST_AUTO_TEST_SUITE_END();
