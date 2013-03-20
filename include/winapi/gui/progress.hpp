/**
    @file

    IProgressDialog C++ wrapper.

    @if license

    Copyright (C) 2012, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_GUI_PROGRESS_HPP
#define WINAPI_GUI_PROGRESS_HPP
#pragma once

#include <boost/detail/scoped_enum_emulation.hpp> // BOOST_SCOPED_ENUM
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/filesystem/path.hpp> // wpath
#include <boost/move/move.hpp>
#include <boost/noncopyable.hpp> // noncopyable
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <comet/error.h>
#include <comet/ptr.h> // com_ptr

#include <cassert> // assert
#include <stdexcept> // invalid_argument
#include <string>

#include <ShlObj.h> // IProgressDialog


template<> struct comet::comtype<IProgressDialog>
{
    static const IID& uuid() throw() { return IID_IProgressDialog; }
    typedef IUnknown base;
};

namespace winapi {
namespace gui {

/**
 * Progress dialogue window.
 *
 * This class displays the window when the object is created and hides it
 * when this object is destroyed.
 *
 * @todo  Can the caller change the title after creation (display)
 *
 * SetAnimation is not supported because Windows Vista and later don't make use
 * of it so it's not worth the effort.
 */
class progress : boost::noncopyable
{
public:

    BOOST_SCOPED_ENUM_START(modality)
    {
        modal,
        non_modal
    };
    BOOST_SCOPED_ENUM_END;

    BOOST_SCOPED_ENUM_START(time_estimation)
    {
        automatic_time_estimate,
        none
    };
    BOOST_SCOPED_ENUM_END;

    BOOST_SCOPED_ENUM_START(bar_type)
    {
        progress,
        marquee,
        none
    };
    BOOST_SCOPED_ENUM_END;

    BOOST_SCOPED_ENUM_START(minimisable)
    {
        yes,
        no
    };
    BOOST_SCOPED_ENUM_END;

    BOOST_SCOPED_ENUM_START(cancellability)
    {
        cancellable,
        uncancellable
    };
    BOOST_SCOPED_ENUM_END;

    /**
     * Create and display the default system progress dialogue.
     *
     * It will be hidden and destroyed when this object is destroyed.
     *
     * @param hwnd_parent      Parent window.
     * @param title            Text for the progress window's title bar.
     * @param modality         Modal relationship of dialogue to its parent
     *                         window.
     * @param time_estimation  Whether dialogue automatically estimates
     *                         remaining time.
     * @param bar_type         Style of progress bar.
     * @param minimisable      Whether dialogue can me minimised.
     * @param cancellability   Whether dialogue allows user to cancel the
     *                         operation.
     * @param ole_site         OLE site.  Something to do with modal behaviour.
     */
    progress(
        HWND hwnd_parent, const std::wstring& title,
        BOOST_SCOPED_ENUM(modality) modality,
        BOOST_SCOPED_ENUM(time_estimation) time_estimation,
        BOOST_SCOPED_ENUM(bar_type) bar_type,
        BOOST_SCOPED_ENUM(minimisable) minimisable,
        BOOST_SCOPED_ENUM(cancellability) cancellability,
        comet::com_ptr<IUnknown> ole_site=NULL)
        : m_progress(comet::com_ptr<IProgressDialog>(CLSID_ProgressDialog))
    {
        _init(
            hwnd_parent, title, modality, time_estimation, bar_type,
            minimisable, cancellability, ole_site);
    }

    /**
     * Display the given progress dialogue for the lifetime of this object.
     *
     * @param progress         The existing IProgressDialog.  This object will 
     *                         display the dialogue and hide it when the object
     *                         is destroyed.
     * @param hwnd_parent      Parent window.
     * @param title            Text for the progress window's title bar.
     * @param modality         Modal relationship of dialogue to its parent
     *                         window.
     * @param time_estimation  Whether dialogue automatically estimates
     *                         remaining time.
     * @param bar_type         Style of progress bar.
     * @param minimisable      Whether dialogue can me minimised.
     * @param cancellability   Whether dialogue allows user to cancel the
     *                         operation.
     * @param ole_site         OLE site.  Something to do with modal behaviour.
     *
     * @todo  What happens if the dialog is already started?
     */
    progress(
        comet::com_ptr<IProgressDialog> progress, HWND hwnd_parent,
        const std::wstring& title,
        BOOST_SCOPED_ENUM(modality) modality,
        BOOST_SCOPED_ENUM(time_estimation) time_estimation,
        BOOST_SCOPED_ENUM(bar_type) bar_type,
        BOOST_SCOPED_ENUM(minimisable) minimisable,
        BOOST_SCOPED_ENUM(cancellability) cancellability,
        comet::com_ptr<IUnknown> ole_site=NULL)
        : m_progress(progress)
    {
        if (!progress)
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Progress dialog required"));

        _init(
            hwnd_parent, title, modality, time_estimation, bar_type,
            minimisable, cancellability, ole_site);
    }

    // Move constructor
    progress(BOOST_RV_REF(progress) other) : m_progress(other.m_progress)
    {
        other.m_progress = comet::com_ptr<IProgressDialog>();
    }

    // Move assignment
    progress& operator=(BOOST_RV_REF(progress) other)
    {
        if (this != &other)
        {
            m_progress->StopProgressDialog();
            m_progress = other.m_progress;
            other.m_progress = comet::com_ptr<IProgressDialog>();
        }
        return *this;
    }

    ~progress()
    {
        if (m_progress)
            m_progress->StopProgressDialog();
    }

    /**
     * Set the indexth line of the display to the given text.
     *
     * Setting a third line will throw an exception if automatic time estimation
     * is enabled.  The third line is used to display that estimate.
     */
    void line(DWORD index, const std::wstring& text)
    {
        HRESULT hr = m_progress->SetLine(index, text.c_str(), FALSE, NULL);
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(com_error_from_interface(m_progress, hr));
    }

    /**
     * Set the indexth line of the display to the given text and compress paths
     * if needed.
     *
     * If the next is too big to fit in the dialogue, the text will be searched
     * for strings that look like Windows filesystem path (have backslashes)
     * and they will be compressed using elipses.
     *
     * Setting a third line will throw an exception if automatic time estimation
     * is enabled.  The third line is used to display that estimate.
     */
    void line_compress_paths_if_needed(DWORD index, const std::wstring& text)
    {
        HRESULT hr = m_progress->SetLine(index, text.c_str(), TRUE, NULL);
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(com_error_from_interface(m_progress, hr));
    }

    /**
     * Update the indicator to show current progress level.
     */
    void update(ULONGLONG so_far, ULONGLONG out_of)
    {
        HRESULT hr = m_progress->SetProgress64(so_far, out_of);
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(com_error_from_interface(m_progress, hr));
    }

    /**
     * Has the user cancelled the operation via the progress dialogue?
     */
    bool user_cancelled() const
    {
        return m_progress->HasUserCancelled() != FALSE;
    }

private:
    
    BOOST_MOVABLE_BUT_NOT_COPYABLE(progress)

    /**
     * Common constructor.
     */
    void _init(
        HWND hwnd_parent, const std::wstring& title,
        BOOST_SCOPED_ENUM(modality) modality,
        BOOST_SCOPED_ENUM(time_estimation) time_estimation,
        BOOST_SCOPED_ENUM(bar_type) bar_type,
        BOOST_SCOPED_ENUM(minimisable) minimisable,
        BOOST_SCOPED_ENUM(cancellability) cancellability,
        comet::com_ptr<IUnknown> ole_site)
    {
        HRESULT hr;

        hr = m_progress->SetTitle(title.c_str());
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(
                com_error_from_interface(m_progress, hr));

        hr = m_progress->StartProgressDialog(
            hwnd_parent, ole_site.get(),
            _options_to_progress_flags(
                modality, time_estimation, bar_type, minimisable,
                cancellability),
            NULL);
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(
                com_error_from_interface(m_progress, hr));
    }

    /*
     * Some of the IProgressDialog flags are Windows-version dependent so
     * we redefine them here so they aren't #ifdeffed out.
     */
    struct PROGDLG
    {
        static const DWORD NORMAL = 0x00000000;
        static const DWORD MODAL = 0x00000001;
        static const DWORD AUTOTIME = 0x00000002;
        static const DWORD NOTIME = 0x00000004;
        static const DWORD NOMINIMIZE = 0x00000008;
        static const DWORD NOPROGRESSBAR = 0x00000010;
        static const DWORD MARQUEEPROGRESS = 0x00000020;
        static const DWORD NOCANCEL = 0x00000040;
    };

    DWORD _options_to_progress_flags(
        BOOST_SCOPED_ENUM(modality) modality,
        BOOST_SCOPED_ENUM(time_estimation) time_estimation,
        BOOST_SCOPED_ENUM(bar_type) bar_type,
        BOOST_SCOPED_ENUM(minimisable) minimisable,
        BOOST_SCOPED_ENUM(cancellability) cancellability)
    {
        DWORD flags = PROGDLG::NORMAL;

        switch (modality)
        {
        case modality::modal:
            flags |= PROGDLG::MODAL;
            break;
        case modality::non_modal:
            break;
        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown modality"));
        }

        switch (time_estimation)
        {
        case time_estimation::automatic_time_estimate:
            flags |= PROGDLG::AUTOTIME;
            break;
        case time_estimation::none:
            break;
        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown time estimation"));
        }

        switch (bar_type)
        {
        case bar_type::progress:
            break;
        case bar_type::marquee:
            flags |= PROGDLG::MARQUEEPROGRESS;
            break;
        case bar_type::none:
            flags |= PROGDLG::NOPROGRESSBAR;
            break;
        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown bar type"));
        }

        switch (minimisable)
        {
        case minimisable::yes:
            break;
        case minimisable::no:
            flags |= PROGDLG::NOMINIMIZE;
            break;
        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown minimisability"));
        }

        switch (cancellability)
        {
        case cancellability::cancellable:
            break;
        case cancellability::uncancellable:
            flags |= PROGDLG::NOCANCEL;
            break;
        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown cancellability"));
        }

        return flags;
    }

    comet::com_ptr<IProgressDialog> m_progress;
};

}} // namespace winapi::gui

#endif
