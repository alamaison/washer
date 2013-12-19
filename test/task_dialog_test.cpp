/**
    @file

    Tests for TaskDialog.

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

#include <winapi/gui/task_dialog.hpp> // test subject

#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp> // boost::this_thread

BOOST_AUTO_TEST_SUITE(task_dialog_tests)

/**
 * Create a TaskDialog without showing it.
 */
BOOST_AUTO_TEST_CASE( create )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    //td.show();
}

int throw_something() { throw std::exception("I throw if invoked"); }

/**
 * Create a TaskDialog with buttons.
 *
 * Use a callback that throws to ensure the callback isn't called unless
 * a button is clicked.
 */
BOOST_AUTO_TEST_CASE( create_with_buttons )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.add_button(L"Uncommon button 1", throw_something);
    td.add_button(
        winapi::gui::task_dialog::button_type::close, throw_something);
    td.add_button(
        L"Uncommon button\nWith another string underneath", throw_something);
    //td.show();
}

/**
 * Create a TaskDialog with radio buttons.
 */
BOOST_AUTO_TEST_CASE( create_with_radio_buttons )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.add_radio_button(27, L"Option 1");
    td.add_button(
        L"Uncommon button\nWith another string underneath", throw_something);
    td.add_radio_button(27, L"Option 2");
    //td.show();
}

/**
 * Create a TaskDialog with collapsed extended text area above fold.
 */
BOOST_AUTO_TEST_CASE( create_with_collapsed_extended_text_above )
{
    winapi::gui::task_dialog::task_dialog_builder<> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.extended_text(L"Detailed explanation");
    //td.show();
}

/**
 * Create a TaskDialog with expanded extended text area above fold.
 */
BOOST_AUTO_TEST_CASE( create_with_expanded_extended_text_above )
{
    winapi::gui::task_dialog::task_dialog_builder<> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.extended_text(
        L"Detailed explanation",
        winapi::gui::task_dialog::expansion_position::above,
        winapi::gui::task_dialog::initial_expansion_state::expanded);
    //td.show();
}

/**
 * Create a TaskDialog with collapsed extended text area below the fold.
 */
BOOST_AUTO_TEST_CASE( create_with_collapsed_extended_text_below )
{
    winapi::gui::task_dialog::task_dialog_builder<> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.extended_text(
        L"Detailed explanation",
        winapi::gui::task_dialog::expansion_position::below);
    //td.show();
}

/**
 * Create a TaskDialog with expanded extended text area below the fold.
 */
BOOST_AUTO_TEST_CASE( create_with_expanded_extended_text_below )
{
    winapi::gui::task_dialog::task_dialog_builder<> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.extended_text(
        L"Detailed explanation",
        winapi::gui::task_dialog::expansion_position::below,
        winapi::gui::task_dialog::initial_expansion_state::expanded);
    //td.show();
}

/**
 * Create a TaskDialog with custom collapsed expander text.
 */
BOOST_AUTO_TEST_CASE( create_with_custom_collapsed_expander )
{
    winapi::gui::task_dialog::task_dialog_builder<> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.extended_text(
        L"Detailed explanation",
        winapi::gui::task_dialog::expansion_position::default,
        winapi::gui::task_dialog::initial_expansion_state::default,
        L"Here be there &dragons");
    //td.show();
}

/**
 * Create a TaskDialog with custom expanded expander text.
 */
BOOST_AUTO_TEST_CASE( create_with_custom_expanded_expander )
{
    winapi::gui::task_dialog::task_dialog_builder<> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.extended_text(
        L"Detailed explanation",
        winapi::gui::task_dialog::expansion_position::default,
        winapi::gui::task_dialog::initial_expansion_state::default,
        L"", L"See! &Dragons");
    //td.show();
}

/**
 * Create a TaskDialog with both expander labels customised.
 */
BOOST_AUTO_TEST_CASE( create_with_custom_expander )
{
    winapi::gui::task_dialog::task_dialog_builder<> td(
        NULL, L"Test instruction", L"Some content text\nAnd some more",
        L"Test TaskDialog");
    td.extended_text(
        L"Detailed explanation",
        winapi::gui::task_dialog::expansion_position::default,
        winapi::gui::task_dialog::initial_expansion_state::default,
        L"Here be there &dragons with really really really really really long tails", L"See! &Dragons");
    //td.show();
}


namespace {

    void start_marquee(winapi::gui::task_dialog::progress_bar bar)
    {
        bar(winapi::gui::task_dialog::marquee_progress());
    }

}

/**
 * Create a TaskDialog with marquee progress.
 *
 * Default update frequency.
 */
BOOST_AUTO_TEST_CASE( create_with_marquee )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Marquee (default update)",
        L"We tell it to start and update with a default (unspecified) "
        L"frequency, which should mean every 30ms.\n"
        L"Will sit and Cylon until dialog is closed.",
        L"create_with_marquee");

    td.include_progress_bar(start_marquee);

    //td.show();
}

namespace {

    void start_marquee_custom(
        winapi::gui::task_dialog::progress_bar bar)
    {
        bar(winapi::gui::task_dialog::marquee_progress(300));
    }

}

/**
 * Create a TaskDialog with marquee progress.
 *
 * Custom update frequency.
 */
BOOST_AUTO_TEST_CASE( create_with_marquee_custom_update )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Marquee (custom update)",
        L"Created dialog and set marquee progress bar in callback.\n"
        L"We tell it to start and update with a custom 0.3s "
        L"frequency.\n"
        L"Will sit and Cylon (jumpily) until dialog is closed.",
        L"create_with_marquee_custom_update");

    td.include_progress_bar(start_marquee_custom);
    //td.show();
}

namespace {

    void run_progress_update(winapi::gui::task_dialog::progress_bar bar)
    {
        winapi::gui::task_dialog::range_progress::progress_updater updater =
            bar(winapi::gui::task_dialog::range_progress(0, 9));

        for (int i=0; i < 10; ++i)
        {
            updater.update_position(i);

            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }
    }

}

/**
 * Create a TaskDialog with steady range progress.
 */
BOOST_AUTO_TEST_CASE( with_range_progress )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Range progress",
        L"When dialog created we start updating progress with "
        L"range_progress updates.\n"
        L"Is incremented steadily to completion.",
        L"with_range_progress");

    td.include_progress_bar(
        winapi::gui::task_dialog::async_progress_updater(
            run_progress_update));

    //td.show();
}

namespace {

    void run_changing_progress_update(
        winapi::gui::task_dialog::progress_bar bar)
    {
        winapi::gui::task_dialog::range_progress our_range(0, 9);

        winapi::gui::task_dialog::range_progress::progress_updater updater =
            bar(our_range);

        for (int i=0; i < 6; ++i)
        {
            updater.update_position(i);

            boost::this_thread::sleep(boost::posix_time::milliseconds(300));
        }

        bar(winapi::gui::task_dialog::marquee_progress());

        boost::this_thread::sleep(boost::posix_time::milliseconds(800));

        updater = bar(our_range);

        for (int i=6; i < 10; ++i)
        {
            updater.update_position(i);

            boost::this_thread::sleep(boost::posix_time::milliseconds(300));
        }

    }
}

/**
 * Create a TaskDialog with range progress but change to marquee part way
 * through and then change back to finish.
 */
BOOST_AUTO_TEST_CASE( create_with_range_progress_change_type )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Range-marquee-range",
        L"Start with range, and run half way.\n"
        L"Switch to marquee Cylon for a bit.\n"
        L"Show the range again, starting from where we left off.",
        L"create_with_range_progress_change_type");

    td.include_progress_bar(
        winapi::gui::task_dialog::async_progress_updater(
            run_changing_progress_update));

    //td.show();
}

namespace {

    void run_pause_run(
        winapi::gui::task_dialog::progress_bar bar)
    {
        winapi::gui::task_dialog::range_progress::progress_updater updater =
            bar(winapi::gui::task_dialog::range_progress(0, 9));

        for (int i=0; i < 6; ++i)
        {
            updater.update_position(i);

            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        }

        updater.bar_state(
            winapi::gui::task_dialog::range_progress::bar_state::paused);

        // It can take a long time for the bar to be redrawn with the new
        // state
        boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

        updater.bar_state(
            winapi::gui::task_dialog::range_progress::bar_state::normal);

        for (int i=6; i < 10; ++i)
        {
            updater.update_position(i);

            boost::this_thread::sleep(boost::posix_time::milliseconds(300));
        }
    }
}

/**
 * Create a TaskDialog with marquee progress but pause it and restart it.
 */
BOOST_AUTO_TEST_CASE( range_pause )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Range interrupted with pause",
        L"Start incrementing progress.\n"
        L"Pause after a bit, then continue as normal.",
        L"range_pause");

    td.include_progress_bar(
        winapi::gui::task_dialog::async_progress_updater(run_pause_run));

    //td.show();
}

namespace {

    void run_error_run(
        winapi::gui::task_dialog::progress_bar bar)
    {
        winapi::gui::task_dialog::range_progress::progress_updater updater =
            bar(winapi::gui::task_dialog::range_progress(0, 9));

        for (int i=0; i < 6; ++i)
        {
            updater.update_position(i);

            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        }

        updater.bar_state(
            winapi::gui::task_dialog::range_progress::bar_state::errored);

        // It can take a long time for the bar to be redrawn with the new
        // state
        boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

        updater.bar_state(
            winapi::gui::task_dialog::range_progress::bar_state::normal);

        for (int i=6; i < 10; ++i)
        {
            updater.update_position(i);

            boost::this_thread::sleep(boost::posix_time::milliseconds(300));
        }
    }
}

/**
 * Create a TaskDialog with marquee progress but display error before
 * continuing.
 */
BOOST_AUTO_TEST_CASE( range_error )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Range interrupted with error",
        L"Start incrementing progress.\n"
        L"Change to error after a bit, then continue as normal.",
        L"range_error");

    td.include_progress_bar(
        winapi::gui::task_dialog::async_progress_updater(run_error_run));

    //td.show();
}

namespace {

    void change_instruction(winapi::gui::task_dialog::task_dialog dialog)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        dialog.instruction(L"And then changes to this");
    }
}

/**
 * Change instruction text of dialog while running.
 */
BOOST_AUTO_TEST_CASE( dynamic_change_instruction )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Instruction starts off like this",
        L"Notice how the instruction text changes.",
        L"dynamic_change_instruction");

    if (false)
    {
        td.show(
            winapi::gui::task_dialog::async_dialog_updater(
                change_instruction));
    }
}

namespace {

    void change_content(winapi::gui::task_dialog::task_dialog dialog)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        dialog.content(L"And\nnow\nfor\nsomething\nelse");
    }
}

/**
 * Change content text of dialog while running.
 */
BOOST_AUTO_TEST_CASE( dynamic_change_content )
{
    winapi::gui::task_dialog::task_dialog_builder<void> td(
        NULL, L"Notice how the content changes.",
        L"Here's the initial content.",
        L"dynamic_change_content");

    if (false)
    {
        td.show(
            winapi::gui::task_dialog::async_dialog_updater(
                change_content));
    }
}

BOOST_AUTO_TEST_SUITE_END();
