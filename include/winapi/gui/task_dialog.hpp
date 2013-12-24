/**
    @file

    TaskDialog C++ wrapper.

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

#ifndef WINAPI_GUI_TASK_DIALOG_HPP
#define WINAPI_GUI_TASK_DIALOG_HPP
#pragma once

#include <winapi/com/catch.hpp> // WINAPI_COM_CATCH
#include <winapi/dynamic_link.hpp> // proc_address
#include <winapi/message.hpp> // send_message
#include <winapi/window/window.hpp>

#include <boost/bind/bind.hpp>
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/function.hpp>
#include <boost/integer_traits.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/numeric/conversion/cast.hpp> // numeric_cast
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <comet/error.h>

#include <algorithm> // transform
#include <cassert> // assert
#include <map>
#include <stdexcept> // invalid_argument
#include <string>
#include <utility> // pair
#include <vector>

#include <commctrl.h> // TASKDIALOG values

#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#if _WIN32_WINNT < 0x0600 && !defined(TD_WARNING_ICON)

//
// Our task dialog class is designed to fail gracefully even on versions of
// Windows without TaskDialog support so we don't require
// NTDDI_VERSION >= NTDDI_VISTA.  However, we must provide our own definitions
// of the task dialog structs/enums as the Windows SDK excludes the when
// building for pre-Vista Windows.
//

#ifdef _WIN32
#include <pshpack1.h>
#endif

typedef HRESULT (CALLBACK *PFTASKDIALOGCALLBACK)(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData);

enum _TASKDIALOG_FLAGS
{
    TDF_ENABLE_HYPERLINKS               = 0x0001,
    TDF_USE_HICON_MAIN                  = 0x0002,
    TDF_USE_HICON_FOOTER                = 0x0004,
    TDF_ALLOW_DIALOG_CANCELLATION       = 0x0008,
    TDF_USE_COMMAND_LINKS               = 0x0010,
    TDF_USE_COMMAND_LINKS_NO_ICON       = 0x0020,
    TDF_EXPAND_FOOTER_AREA              = 0x0040,
    TDF_EXPANDED_BY_DEFAULT             = 0x0080,
    TDF_VERIFICATION_FLAG_CHECKED       = 0x0100,
    TDF_SHOW_PROGRESS_BAR               = 0x0200,
    TDF_SHOW_MARQUEE_PROGRESS_BAR       = 0x0400,
    TDF_CALLBACK_TIMER                  = 0x0800,
    TDF_POSITION_RELATIVE_TO_WINDOW     = 0x1000,
    TDF_RTL_LAYOUT                      = 0x2000,
    TDF_NO_DEFAULT_RADIO_BUTTON         = 0x4000,
    TDF_CAN_BE_MINIMIZED                = 0x8000
};
typedef int TASKDIALOG_FLAGS;

typedef enum _TASKDIALOG_MESSAGES
{
    TDM_NAVIGATE_PAGE                       = WM_USER + 101,
    TDM_CLICK_BUTTON                        = WM_USER + 102,
    TDM_SET_MARQUEE_PROGRESS_BAR            = WM_USER + 103,
    TDM_SET_PROGRESS_BAR_STATE              = WM_USER + 104,
    TDM_SET_PROGRESS_BAR_RANGE              = WM_USER + 105,
    TDM_SET_PROGRESS_BAR_POS                = WM_USER + 106,
    TDM_SET_PROGRESS_BAR_MARQUEE            = WM_USER + 107,
    TDM_SET_ELEMENT_TEXT                    = WM_USER + 108,
    TDM_CLICK_RADIO_BUTTON                  = WM_USER + 110,
    TDM_ENABLE_BUTTON                       = WM_USER + 111,
    TDM_ENABLE_RADIO_BUTTON                 = WM_USER + 112,
    TDM_CLICK_VERIFICATION                  = WM_USER + 113,
    TDM_UPDATE_ELEMENT_TEXT                 = WM_USER + 114,
    TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE = WM_USER + 115,
    TDM_UPDATE_ICON                         = WM_USER + 116
} TASKDIALOG_MESSAGES;

typedef enum _TASKDIALOG_NOTIFICATIONS
{
    TDN_CREATED                         = 0,
    TDN_NAVIGATED                       = 1,
    TDN_BUTTON_CLICKED                  = 2,
    TDN_HYPERLINK_CLICKED               = 3,
    TDN_TIMER                           = 4,
    TDN_DESTROYED                       = 5,
    TDN_RADIO_BUTTON_CLICKED            = 6,
    TDN_DIALOG_CONSTRUCTED              = 7,
    TDN_VERIFICATION_CLICKED            = 8,
    TDN_HELP                            = 9,
    TDN_EXPANDO_BUTTON_CLICKED          = 10
} TASKDIALOG_NOTIFICATIONS;

struct TASKDIALOG_BUTTON
{
    int     nButtonID;
    PCWSTR  pszButtonText;
};

typedef enum _TASKDIALOG_ELEMENTS
{
    TDE_CONTENT,
    TDE_EXPANDED_INFORMATION,
    TDE_FOOTER,
    TDE_MAIN_INSTRUCTION
} TASKDIALOG_ELEMENTS;

typedef enum _TASKDIALOG_ICON_ELEMENTS
{
    TDIE_ICON_MAIN,
    TDIE_ICON_FOOTER
} TASKDIALOG_ICON_ELEMENTS;

#define TD_WARNING_ICON         MAKEINTRESOURCEW(-1)
#define TD_ERROR_ICON           MAKEINTRESOURCEW(-2)
#define TD_INFORMATION_ICON     MAKEINTRESOURCEW(-3)
#define TD_SHIELD_ICON          MAKEINTRESOURCEW(-4)

enum _TASKDIALOG_COMMON_BUTTON_FLAGS
{
    TDCBF_OK_BUTTON            = 0x0001,
    TDCBF_YES_BUTTON           = 0x0002,
    TDCBF_NO_BUTTON            = 0x0004,
    TDCBF_CANCEL_BUTTON        = 0x0008,
    TDCBF_RETRY_BUTTON         = 0x0010,
    TDCBF_CLOSE_BUTTON         = 0x0020
};
typedef int TASKDIALOG_COMMON_BUTTON_FLAGS;

struct TASKDIALOGCONFIG
{
    UINT                           cbSize;
    HWND                           hwndParent;
    HINSTANCE                      hInstance;
    TASKDIALOG_FLAGS               dwFlags;
    TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons;
    PCWSTR                         pszWindowTitle;
    union
    {
        HICON  hMainIcon;
        PCWSTR pszMainIcon;
    };
    PCWSTR                         pszMainInstruction;
    PCWSTR                         pszContent;
    UINT                           cButtons;
    const TASKDIALOG_BUTTON        *pButtons;
    int                            nDefaultButton;
    UINT                           cRadioButtons;
    const TASKDIALOG_BUTTON        *pRadioButtons;
    int                            nDefaultRadioButton;
    PCWSTR                         pszVerificationText;
    PCWSTR                         pszExpandedInformation;
    PCWSTR                         pszExpandedControlText;
    PCWSTR                         pszCollapsedControlText;
    union
    {
        HICON  hFooterIcon;
        PCWSTR pszFooterIcon;
    };
    PCWSTR                         pszFooter;
    PFTASKDIALOGCALLBACK           pfCallback;
    LONG_PTR                       lpCallbackData;
    UINT                           cxWidth;
};

#ifdef _WIN32
#include <poppack.h>
#endif

#define PBST_NORMAL 0x0001
#define PBST_ERROR 0x0002
#define PBST_PAUSED 0x0003

#endif


namespace winapi {
namespace gui {
namespace task_dialog {

typedef boost::function<
    HRESULT (const TASKDIALOGCONFIG*, int*, int*, BOOL*)> tdi_function;

class tdi_implementation
{
public:
    tdi_implementation(const tdi_function& tdi) : m_tdi(tdi) {}

    HRESULT operator()(
        const TASKDIALOGCONFIG* config, int* button, int* radio_button,
        BOOL* verification_flag_checked)
    {
        return m_tdi(
            config, button, radio_button, verification_flag_checked);
    }

    virtual ~tdi_implementation() {}

private:
    tdi_function m_tdi;
};

namespace detail {

    class bind_task_dialog_indirect : public tdi_implementation
    {
    public:
        bind_task_dialog_indirect()
            :
        tdi_implementation(
            winapi::load_function<
                HRESULT (const TASKDIALOGCONFIG*, int*, int*, BOOL*)>(
                "comctl32.dll", "TaskDialogIndirect")) {}
    };

}

namespace button_type
{
    //
    // IMPORTANT: These have to match the Windows IDs for the common buttons
    // because that's what TaskDialogIndirect returns as the value of
    // `which_button`.  The callbacks are registered by the values of this
    // enum, so to be able to match `which_button` to the corresponding
    // callback, we either needed a conversion function or the values have
    // to be the same.  We went for the latter.
    //

    enum type
    {
        ok = IDOK,
        cancel = IDCANCEL,
        yes = IDYES,
        no = IDNO,
        retry = IDRETRY,
        close = IDCLOSE
    };
    
}

namespace icon_type
{
    enum type
    {
        none,
        warning,
        error,
        information,
        shield
    };
}

namespace initial_expansion_state
{
    enum state
    {
        collapsed,
        expanded,
        default = collapsed
    };
}

namespace expansion_position
{
    enum position
    {
        above,
        below,
        default = above
    };
}

namespace detail {

    inline TASKDIALOG_COMMON_BUTTON_FLAGS button_to_tdcbf(button_type::type id)
    {
        switch (id)
        {
        case button_type::ok:
            return TDCBF_OK_BUTTON;
        case button_type::cancel:
            return TDCBF_CANCEL_BUTTON;
        case button_type::yes:
            return TDCBF_YES_BUTTON;
        case button_type::no:
            return TDCBF_NO_BUTTON;
        case button_type::retry:
            return TDCBF_RETRY_BUTTON;
        case button_type::close:
            return TDCBF_CLOSE_BUTTON;
        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown button type"));
        }
    }

    inline const wchar_t* icon_to_tdicon(icon_type::type type)
    {
        switch (type)
        {
        case icon_type::none:
            return NULL;
        case icon_type::warning:
            return TD_WARNING_ICON;
        case icon_type::error:
            return TD_ERROR_ICON;
        case icon_type::information:
            return TD_INFORMATION_ICON;
        case icon_type::shield:
            return TD_SHIELD_ICON;
        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown icon type"));
        }
    }

    inline TASKDIALOG_BUTTON convert_td_button(
        const std::pair<int, std::wstring>& button)
    {
        TASKDIALOG_BUTTON tdb;
        tdb.nButtonID = button.first;
        tdb.pszButtonText = button.second.c_str();
        return tdb;
    }

}

/**
 * A running task dialog.
 */
class task_dialog
{
public:

    void instruction(const std::wstring& new_instruction_text)
    {
        ::winapi::send_message<wchar_t>(
            m_dialog_window.hwnd(), TDM_SET_ELEMENT_TEXT, TDE_MAIN_INSTRUCTION,
            new_instruction_text.c_str());
    }

    void content(const std::wstring& new_content_text)
    {
        ::winapi::send_message<wchar_t>(
            m_dialog_window.hwnd(), TDM_SET_ELEMENT_TEXT, TDE_CONTENT,
            new_content_text.c_str());
    }

    /// @cond INTERNAL
    /// Limits access to private constructor
    class access_attorney
    {
        template<typename T, typename Impl>
        friend class task_dialog_builder;

    public:

        static task_dialog create(const winapi::window::window<>& dialog_window)
        {
            return task_dialog(dialog_window);
        }
    };
    /// @endcond

private:

    friend class access_attorney;

    task_dialog(const winapi::window::window<>& dialog_window) :
        m_dialog_window(dialog_window) {}

    winapi::window::window<> m_dialog_window;
};

namespace detail {

    /**
     * Auto-joining RAII thread wrapper to avoid problems of non-RAII thread;
     *
     * See:
     *  - http://www.open-std.org/Jtc1/sc22/wg21/docs/papers/2008/n2802.html
     *  - http://akrzemi1.wordpress.com/2012/11/14/not-using-stdthread/
     */
    class raii_join_thread : private boost::noncopyable
    {
    public:
        raii_join_thread(boost::function<void()> thread_executee)
            :
        m_thread(thread_executee) {}

        ~raii_join_thread() throw()
        {
            m_thread.join();
        }

    private:
        boost::thread m_thread;
    };

}

/**
 * Executes the given callable on a new thread to update the task dialog while
 * it's running.
 *
 * Will start executing the callable once the task dialog starts.
 *
 * The purpose of this class is to do long-running updates to the task dialog.
 * These cannot be done in the dialog creation callback, as that would prevent
 * the dialog from being displayed while the updates executed.  This class
 * solves the problem by spawning a new thread to run the updates, once the
 * creation callback gives us the task dialog object to run them on.
 */
class async_dialog_updater
{
public:

    async_dialog_updater(
        const boost::function<void(const task_dialog&)> updater) :
    m_updater(updater) {}

    void operator()(const task_dialog& dialog)
    {
        // raii thread will join existing thread here if this is the
        // last reference

        m_thread = boost::make_shared<detail::raii_join_thread>(
            boost::bind(m_updater, dialog));
    }

private:

    boost::function<void(const task_dialog&)> m_updater;
    boost::shared_ptr<detail::raii_join_thread> m_thread;
};

/**
 * Show progress as a marquee bar.
 */
class marquee_progress
{
public:
    
    /**
     * Marquee updates animation at default intervals.
     */
    marquee_progress() : m_update_interval(0) {}

    /**
     * Marquee updates animation at intervals of `update_interval` milliseconds.
     */
    marquee_progress(UINT update_interval) 
        : m_update_interval(update_interval) {}

    /// @cond INTERNAL
    // Limits library-internal access to selected privates
    // See http://stackoverflow.com/q/3217390/67013.
    class access_attorney
    {
        friend class progress_bar;
    public:

        static void call(
            const marquee_progress& progress, ::winapi::window::window<> dialog)
        {
            progress(dialog);
        }

    };
    /// @endcond

private:

    friend class access_attorney;

    void operator()(::winapi::window::window<> dialog) const
    {
        ::winapi::send_message<wchar_t, BOOL>(
            dialog.hwnd(), TDM_SET_MARQUEE_PROGRESS_BAR, TRUE, 0);

        // In case the bar is currently in one of the coloured states.
        // These states don't work with the marquee, they just freeze it
        ::winapi::send_message<wchar_t>(
            dialog.hwnd(), TDM_SET_PROGRESS_BAR_STATE, PBST_NORMAL, 0);

        ::winapi::send_message<wchar_t, BOOL>(
            dialog.hwnd(), TDM_SET_PROGRESS_BAR_MARQUEE, TRUE,
            m_update_interval);
    }

    UINT m_update_interval;
};

/**
 * Show progress as a position along a range.
 */
class range_progress
{
public:
    
    /**
     * Progress between 0 and 100.
     */
    range_progress() : m_initial(0U), m_end(0U) {}

    /**
     * Progress at a position between `initial` and `end`.
     */
    range_progress(UINT initial, UINT end)
        : m_initial(initial), m_end(end) {}

    struct bar_state
    {
        enum enum_t
        {
            normal,
            paused,
            errored
        };
    };

    /**
     * Updates range-based progress bar position and state.
     */
    class progress_updater
    {
    public:

        void update_position(UINT new_position)
        {
            // First set the progress bar one position too far forward
            // to work around Vista+ progress bar bug:
            // http://news.jrsoftware.org/news/innosetup/msg62356.html
            //
            // If the progress bar is still completing its animation
            // when TDM_SET_PROGRESS_BAR_STATE is sent to it, it freezes the
            // bar at its green state instead of redrawing it in yellow
            // or read.  The advance-retreat trick seems to make it redraw
            // properly.
            ::winapi::send_message<wchar_t, BOOL>(
                m_dialog_window.hwnd(), TDM_SET_PROGRESS_BAR_POS,
                new_position + 1, 0);
            ::winapi::send_message<wchar_t, BOOL>(
                m_dialog_window.hwnd(), TDM_SET_PROGRESS_BAR_POS,
                new_position, 0);
        }

        void bar_state(bar_state::enum_t new_state)
        {
            int state_flag;

            switch (new_state)
            {
            case bar_state::normal:
                state_flag = PBST_NORMAL;
                break;

            case bar_state::paused:
                state_flag = PBST_PAUSED;
                break;

            default:
                assert(!"Invalid bar state");
            case bar_state::errored:
                state_flag = PBST_ERROR;
                break;
            }

            ::winapi::send_message<wchar_t>(
                m_dialog_window.hwnd(), TDM_SET_PROGRESS_BAR_STATE,
                state_flag, 0);
        }

    private:
        friend class range_progress;

        progress_updater(const ::winapi::window::window<>& dialog)
            : m_dialog_window(dialog) {}

        ::winapi::window::window<> m_dialog_window;
    };

    /// @cond INTERNAL
    // Limits library-internal access to selected privates
    // See http://stackoverflow.com/q/3217390/67013.
    class access_attorney
    {
        friend class progress_bar;
    public:

        static progress_updater call(
            const range_progress& progress, ::winapi::window::window<> dialog)
        {
            return progress(dialog);
        }

    };
    /// @endcond

private:

    friend class access_attorney;

    progress_updater operator()(::winapi::window::window<> dialog) const
    {
        ::winapi::send_message<wchar_t, BOOL>(
            dialog.hwnd(), TDM_SET_MARQUEE_PROGRESS_BAR, FALSE, 0);

        ::winapi::send_message<wchar_t>(
            dialog.hwnd(), TDM_SET_PROGRESS_BAR_STATE,
            PBST_NORMAL, 0);

        ::winapi::send_message<wchar_t, BOOL>(
            dialog.hwnd(), TDM_SET_PROGRESS_BAR_RANGE, 0,
            MAKELPARAM(m_initial, m_end));

        return progress_updater(dialog);
    }

    UINT m_initial;
    UINT m_end;
};

/**
 * Progress-updating mechanism of a task dialog.
 */
class progress_bar
{
public:


    /**
     * Set progress to a position bar with the given range.
     *
     * The bar begins with the position at the start of the range.  The object
     * returned can update the position and alter its state to a `paused` colour
     * (typically yellow) or an `errored` colour (typically red).
     * 
     * If called when the progress display is a marquee, this method changes
     * the type a range-based display.
     *
     * If it was already a range-based display, this adjusts the range and
     * resets the position to the beginning of that range.
     *
     * @warning  The `progress_updater` returned is only valid until the next
     *           call to the `progress_bar` instance that created it.  Use
     *           after the next call leads to undefined behaviour.
     */
    range_progress::progress_updater operator()(const range_progress& update)
    {
        // Instead of taking and range and position every time, we take the
        // range once and return an object to set the position.  This is
        // because setting the range also resets the bar to the beginning until
        // we reset the position, making the bar appear to flicker.  Doing the
        // range once make subsequent position updates smooth.
        //
        // Also, this is a tidier interface as the caller doesn't have to keep
        // the range bound around to keep passing in with the new position.

        return range_progress::access_attorney::call(update, m_dialog_window);
    }

    /**
     * Set the progress bar to a marquee.
     *
     * If called when the progress display is a normal progress bar, this changes
     * the display to a marquee. 
     *
     * If the display was already a marquee, this resets the marquee animation
     * to the beginning of the sweep.  Therefore, in the absence of intervening
     * calls that set the progress to a range-based display, the caller should
     * only call this method once, unless they really want the marquee sweep to
     * restart.
     */
    void operator()(const marquee_progress& update)
    {
        marquee_progress::access_attorney::call(update, m_dialog_window);
    }

    /// @cond INTERNAL
    /// Limits access to private constructor
    class access_attorney
    {
        template<typename T, typename Impl>
        friend class task_dialog_builder;

    public:
        static progress_bar call(const winapi::window::window<>& dialog_window)
        {
            return progress_bar(dialog_window);
        }
    };
    /// @endcond

private:

    friend class access_attorney;

    progress_bar(const winapi::window::window<>& dialog_window) :
       m_dialog_window(dialog_window) {}

    winapi::window::window<> m_dialog_window;
};

/**
 * Executes the given callable on a new thread to update the task dialog
 * progress bar.
 *
 * Will start executing the callable once the task dialog starts and hands this
 * object a progress bar update object.
 */
class async_progress_updater
{
public:

    async_progress_updater(
        const boost::function<void(const progress_bar&)> updater) :
    m_updater(updater) {}

    void operator()(const progress_bar& bar)
    {
        // raii thread will join existing thread here if this is the
        // last reference

        m_thread = boost::make_shared<detail::raii_join_thread>(
            boost::bind(m_updater, bar));
    }

private:

    boost::function<void(const progress_bar&)> m_updater;
    boost::shared_ptr<detail::raii_join_thread> m_thread;
};

/**
 * Represents the extended text area of a running task dialog.
 */
class extended_text_area
{
public:

    void update_text(const std::wstring& new_text)
    {
        ::winapi::send_message<wchar_t>(
            m_dialog_window.hwnd(), TDM_SET_ELEMENT_TEXT,
            TDE_EXPANDED_INFORMATION, new_text.c_str());
    }

    /// @cond INTERNAL
    /// Limits access to private constructor
    class access_attorney
    {
        template<typename T, typename Impl>
        friend class task_dialog_builder;

    public:
        static extended_text_area create(
            const winapi::window::window<>& dialog_window)
        {
            return extended_text_area(dialog_window);
        }
    };
    /// @endcond

private:

    friend class access_attorney;

    extended_text_area(const winapi::window::window<>& dialog_window) :
       m_dialog_window(dialog_window) {}

    winapi::window::window<> m_dialog_window;
};

/**
 * Executes the given callable on a new thread to update the task dialog's
 * extended text area while the dialog is running.
 *
 * Will start executing the callable once the task dialog starts.
 *
 * The purpose of this class is to do long-running updates to the task dialog
 * text area.  These cannot be done in the dialog creation callback, as that
 * would prevent the dialog being displayed while the updates executed.  This
 * class solves the problem by spawning a new thread to run the updates, once
 * the creation callback gives us the text area object to run them on.
 */
class async_extended_text_updater
{
public:

    async_extended_text_updater(
        const boost::function<void(const extended_text_area&)> updater) :
    m_updater(updater) {}

    void operator()(const extended_text_area& text_area)
    {
        // raii thread will join existing thread here if this is the
        // last reference

        m_thread = boost::make_shared<detail::raii_join_thread>(
            boost::bind(m_updater, text_area));
    }

private:

    boost::function<void(const extended_text_area&)> m_updater;
    boost::shared_ptr<detail::raii_join_thread> m_thread;
};

namespace detail {

    // For some reason these don't work as default arguments if we template
    // them
    inline void task_dialog_noop(const task_dialog&) {}
    inline void extended_text_area_noop(const extended_text_area&) {}

}

/**
 * Creator of running Windows Task Dialogs.
 *
 * It calls TaskDialogIndirect by binding to it dynamically so will fail
 * gracefully by throwing an exception on versions of Windows prior to Vista.
 *
 * @param T     Type of value returned by the button callbacks and @c show().
 * @param Impl  Functor returning a TaskDialogIndirect implementation. By
 *              default this is the stock implementation from comctl32.dll
 *              but can be changed to a custom implementation (e.g. a
 *              TaskDialog emulator for earlier versions of Windows).
 */
template<typename T=void, typename Impl=detail::bind_task_dialog_indirect>
class task_dialog_builder
{
public:
    typedef boost::function<T ()> button_callback;
    typedef std::pair<int, std::wstring> button;

    /**
     * Create a TaskDialog builder.
     *
     * The builder take arguments that are compulsory for any task dialog.
     *
     * Optional properties of the task dialog can be set using the builder's
     * methods.
     *
     * The builder creates the task dialog, from the compulsory arguments and
     * any optional properties set, when the `show` method is called.
     *
     * @param parent_hwnd            Handle to parent window, may be NULL.
     * @param main_instruction       Text of main instruction line.
     * @param content                Text of dialogue body.
     * @param window_title           Title of dialogue window.
     * @param icon                   Icon to display or none.  One of
     *                               @c icon_type.
     * @param cancellation_callback  Function to call if dialogue is cancelled.
     *                               Use this if you aren't going to add a
     *                               common cancel button to the dialogue but
     *                               still want it to respond to Alt+F4, Esc,
     *                               etc., as though a cancel button had been
     *                               clicked.  To disable this default
     *                               behaviour, use an empty button_callback.
     * @param use_command_links      If true (default) display custom buttons
     *                               as large panes arranged vertically in the
     *                               body of the dialog.  Otherwise, display
     *                               them with the common buttons arranged
     *                               horizontally at the bottom.
     */
    task_dialog_builder(
        HWND parent_hwnd, const std::wstring& main_instruction,
        const std::wstring& content, const std::wstring& window_title,
        icon_type::type icon=icon_type::none,
        bool use_command_links=true,
        button_callback cancellation_callback=button_noop)
        :
        m_hwnd(parent_hwnd),
        m_main_instruction(main_instruction),
        m_content(content),
        m_window_title(window_title),
        m_icon(icon),
        m_cancellation_callback(cancellation_callback),
        m_use_command_links(use_command_links),
        m_common_buttons(0),
        m_default_button(0),
        m_default_radio_button(0),
        m_expansion_state(initial_expansion_state::default),
        m_expansion_position(expansion_position::default),
        m_dialog_creation_callback(detail::task_dialog_noop)
        {}

    /**
     * Display the Task dialog and return when a button is clicked.
     *
     * @param dialog_creation_callback
     *     A callable that will be passed the task dialog once it is running,
     *     allowing it to be updated dynamically.  Do not do any long-running
     *     operation in this callback, as the dialog is not shown until it
     *     returns.  Instead use `async_dialog_updater` to spawn an operation
     *     on a new thread in the callback.
     *
     * @returns any value returned by the clicked button's callback.
     */
    T show(
        boost::function<void (const task_dialog&)>
            dialog_creation_callback=detail::task_dialog_noop)
    {
        // basic
        TASKDIALOGCONFIG tdc = {0};
        tdc.cbSize = sizeof(TASKDIALOGCONFIG);
        tdc.pfCallback = callback_dethunker;
        tdc.lpCallbackData = reinterpret_cast<LONG_PTR>(this);

        tdc.hwndParent = m_hwnd;

        // strings
        tdc.pszMainInstruction = m_main_instruction.c_str();
        tdc.pszContent = m_content.c_str();
        tdc.pszWindowTitle = m_window_title.c_str();
        tdc.pszMainIcon = detail::icon_to_tdicon(m_icon);

        // flags
        if (m_use_command_links && !m_buttons.empty())
            tdc.dwFlags |= TDF_USE_COMMAND_LINKS;
        if (!m_cancellation_callback.empty())
        {
            tdc.dwFlags |= TDF_ALLOW_DIALOG_CANCELLATION;
            m_callbacks[IDCANCEL] = m_cancellation_callback;
        }

        // common buttons
        tdc.dwCommonButtons = m_common_buttons;

        // custom buttons
        std::vector<TASKDIALOG_BUTTON> buttons;
        if (!m_buttons.empty())
        {
            std::transform(
                m_buttons.begin(), m_buttons.end(),
                back_inserter(buttons), detail::convert_td_button);

            tdc.cButtons = boost::numeric_cast<int>(buttons.size());
            tdc.pButtons = &buttons[0];
        }
        tdc.nDefaultButton = m_default_button;

        // When no buttons have been specified, the TaskDialog implementation
        // will add an OK button with an id of IDOK.  We add a no-op callback
        // here for that ID so that clicking the button doesn't cause us to
        // throw an exception.
        if (m_common_buttons == 0 && m_buttons.empty())
        {
            m_callbacks[IDOK] = button_noop;
        }

        // radio buttons
        std::vector<TASKDIALOG_BUTTON> radio_buttons;
        if (!m_radio_buttons.empty())
        {
            std::transform(
                m_radio_buttons.begin(), m_radio_buttons.end(),
                back_inserter(radio_buttons), detail::convert_td_button);
            tdc.cRadioButtons = boost::numeric_cast<int>(radio_buttons.size());
            tdc.pRadioButtons = &radio_buttons[0];
        }
        tdc.nDefaultRadioButton = m_default_radio_button;

        // extended text area
        if (!m_extended_text.empty())
        {
            tdc.pszExpandedInformation = m_extended_text.c_str();

            if (m_expansion_state == initial_expansion_state::collapsed)
                tdc.dwFlags &= ~TDF_EXPANDED_BY_DEFAULT;
            else if (m_expansion_state == initial_expansion_state::expanded)
                tdc.dwFlags |= TDF_EXPANDED_BY_DEFAULT;
            else
                BOOST_THROW_EXCEPTION(
                    std::invalid_argument("Unknown initial expansion state"));

            if (m_expansion_position == expansion_position::above)
                tdc.dwFlags &= ~TDF_EXPAND_FOOTER_AREA;
            else if (m_expansion_position == expansion_position::below)
                tdc.dwFlags |= TDF_EXPAND_FOOTER_AREA;
            else
                BOOST_THROW_EXCEPTION(
                    std::invalid_argument("Unknown expansion position"));

            if (!m_expander_label_collapsed.empty())
                tdc.pszCollapsedControlText =
                    m_expander_label_collapsed.c_str();
            if (!m_expander_label_expanded.empty())
                tdc.pszExpandedControlText =
                    m_expander_label_expanded.c_str();
        }

        if (m_bar_creation_callback)
        {
            // The callback being set means the user asked for a progress bar
            tdc.dwFlags |= TDF_SHOW_PROGRESS_BAR;

            // It doesn't matter that we don't say if it's a marquee here.
            // That will be set when the caller uses the `progress_bar` that
            // we hand them later
        }

        m_dialog_creation_callback = dialog_creation_callback;

        int which_button;
        HRESULT hr = Impl()(&tdc, &which_button, NULL, NULL);
        if (hr != S_OK)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(comet::com_error(hr)) <<
                boost::errinfo_api_function("TaskDialogIndirect"));

        m_dialog_creation_callback = detail::task_dialog_noop;

        assert(!m_callbacks.empty()); // windows will add a button if we didn't
        
        // Every button must have a callback
        assert(m_callbacks.find(which_button) != m_callbacks.end());

        return m_callbacks[which_button]();
    }


    /**
     * Add a common button such as OK or Cancel to the bottom of the dialogue.
     *
     * @param type      Button type.  One from @c button_type.
     * @param callback  Function to call if this button is clicked.  If the
     *                  function returns a value it will be returned from the
     *                  call to @c show().
     * @param default   If true, set this to be the default button.
     */
    void add_button(
        button_type::type type, button_callback callback, bool default=false)
    {
        assert(
            m_callbacks.find(detail::button_to_tdcbf(type)) ==
            m_callbacks.end());

        m_common_buttons |= detail::button_to_tdcbf(type);
        m_callbacks[type] = callback;
        if (default)
            m_default_button = type;
    }

    /**
     * Add a custom button to the dialogue.
     *
     * Buttons will be displayed in the order they are added.  If this
     * task_dialog was constructed with command links enabled then these
     * buttons will be displayed in the body of the dialogue arranged
     * vertically.  Otherwise, they will appear with the common buttons
     * at the bottom of the dialogue, arrange horizontally.
     *
     * @param caption   Text to show on the button.  If command links are
     *                  enabled then then any text after the first newline
     *                  will appear as secondary text on the link button.
     * @param callback  Function to call if this button is clicked.  If the
     *                  function returns a value it will be returned from the
     *                  call to @c show().
     * @param default   If true, set this to be the default button.
     */
    void add_button(
        const std::wstring& caption, button_callback callback,
        bool default=false)
    {
        // the common button IDs start at 1 so we generate IDs for the custom
        // buttons starting with maxint to make collisions as unlikely as
        // possible
        // the next available index in the table is the maximum int minus
        // the number of custom buttons we have already added
        // adding common buttons won't affect the next index because they're
        // stored separately.

        int next_id =
            boost::integer_traits<int>::const_max -
            boost::numeric_cast<int>(m_buttons.size());
        assert(m_callbacks.find(next_id) == m_callbacks.end());

        m_buttons.push_back(button(next_id, caption));
        m_callbacks[next_id] = callback;
        if (default)
            m_default_button = next_id;
    }

    /**
     * Add a radio button to the dialog.
     *
     * They are displayed in the order they are added.
     *
     * @todo  Find a way to actually get back which button was chosen.
     */
    void add_radio_button(
        int id, const std::wstring& caption, bool default=false)
    {
        m_radio_buttons.push_back(button(id, caption));
        if (default)
            m_default_radio_button = id;
    }

    /**
     * Set text for the optional extended text area of the dialogue.
     *
     * You can choose whether the text expands above or below the expander
     * button and whether it is collapsed or expanded when the dialogue first
     * appears.  The default is collapsed initially and expands above the button,
     * just below the main text.
     *
     * Also, you can customise the text that labels the expander button.  This
     * normally says "See details" when collapsed and "Hide details" when
     * expanded (or translated equivalent).  If you only customise one of these
     * labels, the same text is used for both.
     */
    void extended_text(
        const std::wstring& text,
        expansion_position::position position=expansion_position::default,
        initial_expansion_state::state state=initial_expansion_state::default,
        const std::wstring& expander_label_collapsed=std::wstring(),
        const std::wstring& expander_label_expanded=std::wstring(),
        boost::function<void (const extended_text_area&)>
            extended_text_creation_callback=detail::extended_text_area_noop)
    {
        m_extended_text = text;
        m_expansion_position = position;
        m_expansion_state = state;
        m_expander_label_collapsed = expander_label_collapsed;
        m_expander_label_expanded = expander_label_expanded;
        m_expando_creation_callback = extended_text_creation_callback;
    }

    /**
     * Includes a progress bar in the interface of the dialog being created.
     *
     * @param bar_creation_callback
     *     A callable which will receive interface to the progress bar once
     *     it is created.  This interface is how the caller can update
     *     the progress bar.
     */
    void include_progress_bar(
        const boost::function<void (const progress_bar&)>&
            bar_creation_callback)
    {
        // Setting this doubles as a boolean flag to tell dialog-creation
        // logic to set the progress flag
        m_bar_creation_callback = bar_creation_callback;
    }

private:
    HWND m_hwnd;
    std::wstring m_main_instruction;
    std::wstring m_content;
    std::wstring m_window_title;
    icon_type::type m_icon;
    bool m_use_command_links;
    button_callback m_cancellation_callback;

    /** @name  Button State */
    // @{
    TASKDIALOG_COMMON_BUTTON_FLAGS m_common_buttons; ///< Common dialog buttons
    std::vector<button> m_buttons; ///< Buttons with strings owned by us
    std::map<int, button_callback> m_callbacks; /// Map button IDs to callbacks
    std::vector<button> m_radio_buttons; ///< Radio buttons, strings owned by us
    int m_default_button;
    int m_default_radio_button;
    // @}

    /** @name  Extended text area */
    // @{
    std::wstring m_extended_text;
    initial_expansion_state::state m_expansion_state;
    expansion_position::position m_expansion_position;
    std::wstring m_expander_label_collapsed;
    std::wstring m_expander_label_expanded;
    // @}

    boost::function<void (const task_dialog&)> m_dialog_creation_callback;
    boost::optional<boost::function<void (const progress_bar&)>>
        m_bar_creation_callback;
    boost::optional<boost::function<void (const extended_text_area&)>>
        m_expando_creation_callback;

    boost::optional<winapi::window::window<>> m_running_dialog;

    static T button_noop() { return T(); }

    static HRESULT CALLBACK callback_dethunker(
        HWND dialog_window, UINT notification, WPARAM wparam, LPARAM lparam,
        LONG_PTR thunked_this) throw()
    {
        task_dialog_builder* this_td = reinterpret_cast<task_dialog_builder*>(thunked_this);

        try
        {
            return this_td->callback(
                dialog_window, notification, wparam, lparam);
        }
        WINAPI_COM_CATCH();
    }

    HRESULT callback(
        HWND dialog_window, UINT notification,
        WPARAM /*wparam*/, LPARAM /*lparam*/)
    {
        switch (notification)
        {
        case TDN_CREATED:
            m_running_dialog = winapi::window::window<>(
                winapi::window::window_handle::foster_handle(dialog_window));

            assert(m_running_dialog);

            m_dialog_creation_callback(task_dialog::access_attorney::create(
                *m_running_dialog));

            if (m_expando_creation_callback)
            {
                (*m_expando_creation_callback)(
                    extended_text_area::access_attorney::create(
                        *m_running_dialog));
            }

            if (m_bar_creation_callback)
            {

                (*m_bar_creation_callback)(
                    progress_bar::access_attorney::call(*m_running_dialog));
            }
            return S_OK;

        case TDN_DESTROYED:
            assert(dialog_window == *m_running_dialog);
            m_running_dialog = boost::optional<winapi::window::window<>>();
            return S_OK;

        default:
            return S_OK;
        }
    }
};

}}} // namespace winapi::gui::task_dialog

#endif
