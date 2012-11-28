/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/future/widgets/headers/widget_utils.hpp>

#include <adobe/future/widgets/headers/platform_metrics.hpp>
#include <adobe/future/windows_cast.hpp>
#include <adobe/name.hpp>
#include <adobe/unicode.hpp>

#include <boost/cstdint.hpp>

#include <vector>

// for native widget callback setup
#include <adobe/future/widgets/headers/platform_button.hpp>
#include <adobe/future/widgets/headers/platform_checkbox.hpp>
#include <adobe/future/widgets/headers/platform_radio_button.hpp>
#include <adobe/future/widgets/headers/platform_slider.hpp>


#ifdef ADOBE_PLATFORM_WT

#include <Wt/WCheckBox>
#include <Wt/WContainerWidget>
#include <Wt/WFormWidget>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WRadioButton>
#include <Wt/WText>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

void set_window_title(platform_display_type window, const display_string & title)
{
	// liukahr : make some ugly dynamic cast switch here ...

}

std::string get_window_title(platform_display_type window)
{
	return "GiveMeAName";
	// liukahr : make some ugly dynamic cast switch here ...
}

/****************************************************************************************************/


template <typename native_widget_t>
native_widget_t add_widget (platform_display_type parent, native_widget_t w)
{
	assert (w);
	if (Wt::WContainerWidget * c = dynamic_cast<Wt::WContainerWidget*> (parent))
	{
		c->addWidget (w);
	}
	else if (Wt::WTabWidget * t = dynamic_cast<Wt::WTabWidget*> (parent))
	{
		t->addTab(w, "GiveMeAName");
	}
	else
	{
		throw std::exception("unknown parent");
	}
	return w;
}

/****************************************************************************************************/

native_button_t make_button (platform_display_type parent, const std::string & label, bool is_default)
{
	return add_widget (parent, new Wt::WPushButton (label));
}

native_checkbox_t make_checkbox (platform_display_type parent, const std::string & label)
{
	return add_widget (parent, new Wt::WCheckBox (label));
}

native_label_t make_label (platform_display_type parent, const std::string & label)
{
	return add_widget (parent, new Wt::WText (label));
}

native_radio_button_t make_radio_button (platform_display_type parent, const std::string & label)
{
	return add_widget (parent, new Wt::WRadioButton (label));
}

native_slider_t make_slider (platform_display_type parent, bool is_vertical, std::size_t num_ticks, adobe::slider_style_t style)
{
	native_slider_t s = new Wt::WSlider (is_vertical ? Wt::Vertical : Wt::Horizontal);
	s->setMinimum(0);
	s->setMaximum(100);
	return add_widget (parent, s);
}

native_panel_t make_panel (platform_display_type parent)
{
	return add_widget (parent, new Wt::WPanel);
}

native_window_t make_window (platform_display_type parent, const std::string & label, const bool & has_size_box)
{
	return add_widget (parent, new Wt::WContainerWidget);
}

/****************************************************************************************************/

void setup_callback_radio_button(adobe::radio_button_t & element)
{
	element.control_m->clicked().connect(&element, &adobe::radio_button_t::on_clicked);
}

void setup_callback_checkbox(adobe::checkbox_t & element)
{
	element.control_m->clicked().connect(&element, &adobe::checkbox_t::on_clicked);
}

void setup_callback_button(adobe::button_t & element)
{
	element.control_m->clicked().connect(&element, &adobe::button_t::on_clicked);
}

void setup_callback_slider(adobe::slider_t & element)
{
	element.control_m->valueChanged().connect(SLOT(&element, adobe::slider_t::on_new_value));
}

/****************************************************************************************************/

void set_control_bounds(platform_display_type control, const place_data_t& place_data)
{
    assert(control);
}

void set_control_alt_text(platform_display_type control, const std::string &)
{
    assert(control);
}

}

/****************************************************************************************************/

bool is_null_control (platform_display_type control)
{
	return NULL == control;
}

/****************************************************************************************************/

void set_font_checkbox		(platform_display_type w) {}
void set_font_dropdownbutton(platform_display_type w) {}
void set_font_edittext		(platform_display_type w) {}
void set_font_groupbox		(platform_display_type w) {}
void set_font_progressbar	(platform_display_type w) {}
void set_font_pushbutton	(platform_display_type w) {}
void set_font_radiobutton	(platform_display_type w) {}
void set_font_tabitem		(platform_display_type w) {}
void set_font_thumbtop		(platform_display_type w) {}
void set_font_thumbleft		(platform_display_type w) {}


/****************************************************************************************************/

platform_display_type get_parent_control (platform_display_type child)
{
	return dynamic_cast<platform_display_type> (child->parent());
}

/****************************************************************************************************/

void set_control_visible(platform_display_type control, bool make_visible)
{
    assert(control);
	control->setHidden(!make_visible);
}

/****************************************************************************************************/

void set_control_enabled(platform_display_type control, bool make_enabled)
{
	assert(control);
	control->setDisabled(!make_enabled);
}

/****************************************************************************************************/

bool get_control_enabled(platform_display_type control)
{
    assert(control);
    return control->isEnabled();
}

/****************************************************************************************************/

void set_control_checked(platform_display_type control, check_state state)
{
	assert(control);
	Wt::WCheckBox * cb = dynamic_cast<Wt::WCheckBox*>(control);
	assert(cb);

	Wt::CheckState native_state(Wt::PartiallyChecked);
	if (state)
		native_state = Wt::Checked;
	else if (!state)
		native_state = Wt::Unchecked;
	
	cb->setCheckState(native_state);
}

/****************************************************************************************************/

bool is_key_return(const key_type & key)
{
	return Wt::Key_Enter == key;
}

/****************************************************************************************************/

bool is_key_escape(const key_type & key)
{
	return Wt::Key_Escape == key;
}

/****************************************************************************************************/

}

/****************************************************************************************************/

#else

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

adobe::platform_display_type tooltip_control()
{
    /*
        Tooltips on Windows are windows in and of themselves, and they have a queue of tools to which
        they are bound. In order to take advantage of this we have one global tooltip window, and each
        tool is bound to it with its alternative text string.
    */

    static HWND tooltip_s = CreateWindowEx(WS_EX_TOPMOST,
                                           TOOLTIPS_CLASS,
                                           NULL,
                                           WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           NULL,
                                           NULL,
                                           ::GetModuleHandle(NULL),
                                           NULL
                                           );
    static bool inited(false);

    if (!inited)
    {
        SetWindowPos(tooltip_s,
                     HWND_TOPMOST,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        inited = true;
    }

    return tooltip_s;
}

/****************************************************************************************************/

std::string window_class(HWND window)
{
    if (::IsWindow(window))
    {
        std::vector<char> class_name(64, 0);

        int num_copied = ::GetClassNameA(window, &class_name[0], static_cast<int>(class_name.size()));

        return std::string(&class_name[0], num_copied);
    }

    return std::string();
}

/****************************************************************************************************/

void set_font(HWND window, int uxtheme_type)
{
    LOGFONTW        log_font = { 0 };
    HFONT           font = 0;

    //
    // Use the metrics to obtain the correct font for this widget.
    //
	adobe::metrics::set_window(window);

	if (adobe::metrics::get_font(uxtheme_type, log_font))
    {
        //
        // Create a font from the LOGFONT structure.
        //
        font = ::CreateFontIndirectW(&log_font);
    }
    assert(font);

    ::SendMessage(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), true);
}

} // namespace

/****************************************************************************************************/

namespace hackery {

/****************************************************************************************************/

std::string convert_utf(const WCHAR* buffer, std::size_t size)
{
    assert(buffer);

    typedef     std::vector<char> buftype;
    int         length = WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(size), 0, 0, 0, 0);
    buftype     tmpbuf(length + 1);
    //
    // Convert characters.
    //
    WideCharToMultiByte(CP_UTF8, 0, buffer, static_cast<int>(size), &(tmpbuf[0]), length, 0, 0);
    return std::string(&(tmpbuf[0]));
}

/****************************************************************************************************/

std::string convert_utf(const WCHAR* buffer)
{
    assert(buffer);

    std::size_t size(0);

    while (buffer[size] != WCHAR(0)) ++size;

    return convert_utf(buffer, size);
}

/****************************************************************************************************/

std::wstring convert_utf(const CHAR* buffer, std::size_t size)
{
    assert(buffer);

    typedef     std::vector<WCHAR> buftype;
    int         length = MultiByteToWideChar(CP_UTF8, 0, buffer, static_cast<int>(size), 0, 0);
    buftype     tmpbuf(length + 1);
    //
    // Convert characters.
    //
    MultiByteToWideChar(CP_UTF8, 0, buffer, static_cast<int>(size), &(tmpbuf[0]), length);
    return std::wstring(&(tmpbuf[0]), tmpbuf.size());
}

/****************************************************************************************************/

std::wstring convert_utf(const CHAR* buffer)
{
    assert(buffer);

    std::size_t size(0);

    while (buffer[size] != CHAR(0)) ++size;

    return convert_utf(buffer, size);
}

/****************************************************************************************************/

} // namespace hackery

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

boost::filesystem::path to_path(const std::vector<TCHAR>& path_buffer)
{
    // At this point we have a path to the save file location, but we don't know
    // if the return value is utf16 or utf8. Boost::filesystem at the moment requires
    // utf8 (or ASCII?) so we need to make the conversion at runtime if necessary.
    // That's what this code does.

    std::vector<char> cpath_buffer;

    const TCHAR* end = std::find(&path_buffer[0], &path_buffer[0] + path_buffer.size(), TCHAR(0));

    cpath_buffer.reserve(std::distance(&path_buffer[0], end));

    adobe::to_utf8(&path_buffer[0], end, std::back_inserter(cpath_buffer));

    cpath_buffer.push_back(0);

    // finally, construct the new path from the converted string buffer and return

    return boost::filesystem::path(&cpath_buffer[0], boost::filesystem::native);
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/
	
native_button_t make_button	(platform_display_type parent, const std::string & label, bool is_default)
{
    DWORD win_style(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | BS_NOTIFY);
	if (is_default)
        win_style |= BS_DEFPUSHBUTTON;

    return check_not_null (::CreateWindowExW(WS_EX_COMPOSITED /*| WS_EX_TRANSPARENT*/, L"BUTTON",
                                          hackery::convert_utf(label).c_str(),
                                          win_style,
                                          0, 0, 70, 20,
                                          parent,
                                          0,
                                          ::GetModuleHandle(NULL),
                                          NULL));
}

native_checkbox_t make_checkbox (platform_display_type parent, const std::string & label)
{
    return check_not_null (::CreateWindowExW(WS_EX_COMPOSITED | WS_EX_TRANSPARENT, L"BUTTON",
                                          ::hackery::convert_utf(label).c_str(),
                                          WS_CHILD | WS_VISIBLE | BS_3STATE | WS_TABSTOP | BS_NOTIFY,
                                          0, 0, 100, 20,
                                          parent,
                                          NULL,
                                          ::GetModuleHandle(NULL),
                                          NULL));
}


native_label_t make_label (platform_display_type parent, const std::string & label)
{
	return check_not_null (::CreateWindowExW(WS_EX_COMPOSITED, L"STATIC",
                                       hackery::convert_utf(label).c_str(),
                                       WS_CHILD | WS_VISIBLE,
                                       0, 0, 100, 20,
                                       parent,
                                       NULL,
                                       ::GetModuleHandle(NULL),
                                       NULL));
}

native_panel_t make_panel (platform_display_type parent)
{
	return check_not_null (::CreateWindowEx(   WS_EX_CONTROLPARENT | WS_EX_COMPOSITED,
                    _T("eve_panel"),
                    NULL,
                    WS_CHILD | WS_VISIBLE,
                    0, 0, 10, 10,
                    parent,
                    0,
                    ::GetModuleHandle(NULL),
                    NULL));
}

native_radio_button_t make_radio_button (platform_display_type parent, const std::string & label)
{
	return check_not_null (::CreateWindowExW(WS_EX_COMPOSITED | WS_EX_TRANSPARENT, L"BUTTON",
                                          hackery::convert_utf(label).c_str(),
                                          WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_TABSTOP | BS_NOTIFY,
                                          0, 0, 100, 20,
                                          parent,
                                          0,
                                          ::GetModuleHandle(NULL),
                                          NULL));
}

native_slider_t make_slider (platform_display_type parent, bool is_vertical, std::size_t num_ticks, adobe::slider_style_t style)
{
    DWORD win32_style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;

    win32_style |= is_vertical ? TBS_VERT : TBS_HORZ;

    if (num_ticks)
    {
        switch (style)
        {
            case adobe::slider_points_up_s:    win32_style |= TBS_TOP;    break;
            case adobe::slider_points_left_s:  win32_style |= TBS_LEFT;   break;
            case adobe::slider_points_down_s:  win32_style |= TBS_BOTTOM; break;
            case adobe::slider_points_right_s: win32_style |= TBS_RIGHT;  break;
            default: break; // silences a GCC warning
        }
    }
    else
    {
        win32_style |= TBS_NOTICKS;
    }

    return check_not_null (::CreateWindowEx(WS_EX_COMPOSITED,
                                         TRACKBAR_CLASS,
                                         NULL,
                                         win32_style,
                                         0, 0, 20, 20,
                                         parent,
                                         0,
                                         ::GetModuleHandle(NULL),
                                         NULL));

}

native_window_t make_window (platform_display_type parent, const std::string & label, const bool & has_size_box)
{
    DWORD platform_style(WS_OVERLAPPED | WS_CAPTION | WS_BORDER/* | WS_SYSMENU*/);
    DWORD dialog_extended_style = WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME | WS_EX_COMPOSITED;

	if (has_size_box)
	{
		platform_style |= WS_SIZEBOX;
	}

	return check_not_null (::CreateWindowExW(dialog_extended_style,
                                         L"eve_dialog",
                                         hackery::convert_utf(label).c_str(),
                                         platform_style,    
                                         10, 10, 20, 20,
                                         parent,
                                         NULL,
                                         ::GetModuleHandle(NULL),
                                         NULL));
}

/****************************************************************************************************/

LRESULT CALLBACK radio_button_subclass_proc(HWND     window,
                                      UINT     message,
                                      WPARAM   wParam,
                                      LPARAM   lParam,
                                      UINT_PTR ptr,
                                      DWORD_PTR /* ref */)
{
    if (message == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
    {
	    adobe::radio_button_t& radio_button(*reinterpret_cast<adobe::radio_button_t*>(ptr));
		radio_button.on_clicked();
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

void setup_callback_radio_button (radio_button_t & element)
{
	::SetWindowSubclass(element.control_m, &radio_button_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);
}

LRESULT CALLBACK button_subclass_proc(HWND     window,
                                      UINT     message,
                                      WPARAM   wParam,
                                      LPARAM   lParam,
                                      UINT_PTR ptr,
                                      DWORD_PTR /* ref */)
{
    if (message == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
    {
	    adobe::button_t& button(*reinterpret_cast<adobe::button_t*>(ptr));
		button.on_clicked();
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

void setup_callback_button(adobe::button_t & element)
{
	::SetWindowSubclass(element.control_m, &button_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);
}

LRESULT CALLBACK checkbox_subclass_proc(HWND     window,
                                      UINT     message,
                                      WPARAM   wParam,
                                      LPARAM   lParam,
                                      UINT_PTR ptr,
                                      DWORD_PTR /* ref */)
{
    if (message == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
    {
	    adobe::checkbox_t& checkbox(*reinterpret_cast<adobe::checkbox_t*>(ptr));
		checkbox.on_clicked();
		return 0;
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

void setup_callback_checkbox(adobe::checkbox_t & element)
{
	::SetWindowSubclass(element.control_m, &checkbox_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);
}

LRESULT CALLBACK slider_subclass_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR ptr, DWORD_PTR /* ref */)
{
    adobe::slider_t& control(*reinterpret_cast<adobe::slider_t*>(ptr));

    assert(control.control_m);

    if (control.value_proc_m.empty() == false &&
        (message == WM_HSCROLL || message == WM_VSCROLL))
    {
        WORD submsg(LOWORD(wParam));

        if (submsg == TB_LINEUP        || submsg == TB_LINEDOWN   ||
            submsg == TB_PAGEUP        || submsg == TB_PAGEDOWN   ||
            submsg == TB_THUMBPOSITION || submsg == TB_THUMBTRACK ||
            submsg == TB_TOP           || submsg == TB_BOTTOM     ||
            submsg == TB_ENDTRACK)
        {
            long   new_position(static_cast<long>(::SendMessage(window, TBM_GETPOS, 0, 0)));
			adobe::slider_t::model_type new_value(control.format_m.at(new_position).cast<adobe::slider_t::model_type>());

			control.on_new_value (new_value);
        }
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}


void setup_callback_slider(adobe::slider_t & element)
{
	::SetWindowSubclass(element.control_m, &slider_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);
}


/****************************************************************************************************/

bool pick_file(boost::filesystem::path& path, platform_display_type dialog_parent)
{
    // This upper part sets up the OS-specific open file dialog
    // and invokes it so the user can pick a file to open

    std::vector<TCHAR> path_buffer(2048, 0);
    OPENFILENAME       params = { 0 };

    params.hwndOwner = dialog_parent;
    params.lStructSize = sizeof(params);
    params.lpstrFilter = L"Text File\0*.txt\0All Files\0*.*\0\0";
    params.lpstrFile = &path_buffer[0];
    params.nMaxFile = static_cast<DWORD>(path_buffer.size());
    params.Flags = OFN_DONTADDTORECENT |
                   OFN_LONGNAMES |
                   OFN_NOCHANGEDIR |
                   OFN_NONETWORKBUTTON |
                   OFN_PATHMUSTEXIST |
                   OFN_FILEMUSTEXIST;
    params.lpstrDefExt = L"txt";

    bool result(::GetOpenFileName(&params) != 0);

    if (result)
        path = to_path(path_buffer);

    return result;
}

/****************************************************************************************************/

bool pick_save_path(boost::filesystem::path& path, platform_display_type dialog_parent)
{
    // This upper part sets up the OS-specific save file dialog and
    // invokes it so the user can pick a save file location

    std::vector<TCHAR> path_buffer(2048, 0);
    OPENFILENAME       params = { 0 };

    params.hwndOwner = dialog_parent;
    params.lStructSize = sizeof(params);
    params.lpstrFilter = L"Text File\0*.txt\0All Files\0*.*\0\0";
    params.lpstrFile = &path_buffer[0];
    params.nMaxFile = static_cast<DWORD>(path_buffer.size());
    params.Flags = OFN_DONTADDTORECENT |
                   OFN_LONGNAMES |
                   OFN_NOCHANGEDIR |
                   OFN_NONETWORKBUTTON |
                   OFN_NOREADONLYRETURN |
                   OFN_PATHMUSTEXIST;
    params.lpstrDefExt = L"txt";

    bool result(::GetSaveFileName(&params) != 0);

    if (result)
        path = to_path(path_buffer);

    return result;
}

/****************************************************************************************************/

void set_control_alt_text(HWND control, const std::string& alt_text)
{
    HWND               tooltip(tooltip_control()); // handle to the ToolTip control
    TOOLINFO           tooltip_info;               // struct specifying info about tool in ToolTip control
    std::vector<TCHAR> text_buffer(alt_text.size() * 2, 0);

    /* Set up the text for filling in the tooltip structure below */
    to_utf16(&alt_text[0], &alt_text[0] + alt_text.size(), &text_buffer[0]);

    /* Basic initialization of the tooltip structure parameters */
    tooltip_info.cbSize = sizeof(TOOLINFO);
    tooltip_info.uFlags = TTF_SUBCLASS | TTF_IDISHWND | TTF_CENTERTIP;
    tooltip_info.hwnd = control;
    tooltip_info.hinst = ::GetModuleHandle(NULL);
    tooltip_info.uId = hackery::cast<UINT_PTR>(control);
    tooltip_info.lpszText = &text_buffer[0];

    // ToolTip control will cover the whole window
    ::GetClientRect (control, &tooltip_info.rect);

    /*
        First delete any previous references to this control from the TT's tool list. This
        allows for widgets with potentially multiple alt_texts (e.g., buttons and their
        different states) to display each one when appropriate. If there is no previous
        reference then this does nothing, which is fine.
    */
    SendMessage(tooltip, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &tooltip_info);

    /* At the last, we bind the tooltip to the control */
    SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &tooltip_info);
}

/****************************************************************************************************/

std::string get_window_title(HWND window)
{
    assert(window);

    int const buffer_size = ::GetWindowTextLengthW(window) + 1;

    std::vector<WCHAR> titlename(buffer_size, 0);

    int const text_length = ::GetWindowTextW(window, &titlename[0], buffer_size);

    assert(text_length < buffer_size);

    return std::string(hackery::convert_utf(&titlename[0]));
}

/****************************************************************************************************/

void set_window_title(platform_display_type window, const display_string & title)
{
    assert(window);

	::SetWindowTextW(window, hackery::convert_utf (title).c_str());
}

/****************************************************************************************************/

//void set_window_image (platform_display_type window, const image_type & image)
//{
//    assert(window);
//
//	::SetWindowTextW(window, hackery::convert_utf (title).c_str());
//}

/****************************************************************************************************/

void get_control_bounds(platform_display_type control, place_data_t& p)
{
    assert(control);

	RECT rect;
    ::GetWindowRect(control, &rect);

	from_native (rect, p);
}

/****************************************************************************************************/

void set_control_bounds(HWND control, const place_data_t& place_data)
{
    assert(control);

    ::MoveWindow(control, left(place_data), top(place_data), width(place_data), height(place_data), TRUE);
}

/****************************************************************************************************/

void throw_last_error_exception(const char* /* file */, long /* line */)
{
    DWORD error(::GetLastError());
    char  the_message[2048] = { 0 };

    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        0,
        error,
        0,
        &the_message[0],
        sizeof(the_message),
        NULL);

    char* actual_error = the_message[0] ? the_message : "Unknown Error.";

    ::MessageBoxA(0, actual_error, "Error Message From Windows", MB_ICONEXCLAMATION | MB_APPLMODAL | MB_OK);

    throw std::runtime_error(the_message);
}

/****************************************************************************************************/

modifiers_t convert_modifiers(ULONG os_modifiers)
{
    modifiers_t result(modifiers_none_s);

#define ADOBE_MAPMOD(osmod, adobemod)   if (os_modifiers == (osmod)) result = result | (adobemod)
    //
    // Note that VK_MENU is the ALT key.
    //
    ADOBE_MAPMOD(VK_CAPITAL,    modifiers_caps_lock_s);
    ADOBE_MAPMOD(VK_CONTROL,    modifiers_any_control_s);
    ADOBE_MAPMOD(VK_SHIFT,      modifiers_any_shift_s);
    ADOBE_MAPMOD(VK_MENU,       modifiers_any_option_s);

#undef ADOBE_MAPMOD

    return result;
}

/****************************************************************************************************/

modifiers_t convert_modifiers(BYTE keyboard_state[256])
{
    modifiers_t result(modifiers_none_s);

    if (keyboard_state[VK_CAPITAL] & 0x80)  result |= modifiers_caps_lock_s;
    if (keyboard_state[VK_LSHIFT] & 0x80)   result |= modifiers_left_shift_s;
    if (keyboard_state[VK_RSHIFT] & 0x80)   result |= modifiers_right_shift_s;
    if (keyboard_state[VK_LCONTROL] & 0x80) result |= modifiers_left_control_s;
    if (keyboard_state[VK_RCONTROL] & 0x80) result |= modifiers_right_control_s;
    if (keyboard_state[VK_LMENU] & 0x80)    result |= modifiers_left_option_s;
    if (keyboard_state[VK_RMENU] & 0x80)    result |= modifiers_right_option_s;

    return result;
}

/****************************************************************************************************/

modifiers_t modifier_state()
{
    BYTE keyboard_state[256] = { 0 };

    ::GetKeyboardState(&keyboard_state[0]);

    return convert_modifiers(keyboard_state);
}

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

bool forward_message(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& forward_result)
{
    static const std::string listview_class_k("SysListView32");

    if (message == WM_COMMAND)
    {
        HWND actual_window(reinterpret_cast<HWND>(lParam));

        if (actual_window == NULL)
            return false;

        if (window_class(actual_window) == listview_class_k)
            return false;

        forward_result = ::SendMessage(actual_window, message, wParam, lParam);

        return forward_result == 0;
    }
    else if (message == WM_NOTIFY)
    {
        NMHDR& notify_header(*reinterpret_cast<NMHDR*>(lParam));
        HWND   actual_window(notify_header.hwndFrom);

        if (actual_window == NULL)
            return false;

        //if (window_class(actual_window) == listview_class_k)
        //    return false;

        forward_result = ::SendMessage(actual_window, message, wParam, lParam);

        // From WM_NOTIFY docs:
        // "The return value is ignored except for notification messages that specify otherwise."
        // Lame.

        // note: this list is not exhaustive. Items should be added to the list
        //       as notification messages are found that require return values.

        bool notify_message_set_that_returns_stuff(message == NM_CUSTOMDRAW);

        return notify_message_set_that_returns_stuff;
    }
    else if (message == WM_HSCROLL || message == WM_VSCROLL)
    {
        HWND actual_window(reinterpret_cast<HWND>(lParam));

        if (actual_window == NULL)
            return false;

        if (window_class(actual_window) == listview_class_k)
            return false;

        forward_result = ::SendMessage(actual_window, message, wParam, lParam);

        return forward_result == 0;
    }

    return false;
}

/****************************************************************************************************/

bool context_menu(HWND parent,
                  long x, long y,
                  const name_t* first,
                  const name_t* last,
                  name_t& result)
{
    HMENU menu(::CreatePopupMenu());

    const name_t* the_first(first);

    long count(0);

    for (; first != last; ++first)
    {
        MENUITEMINFOA item = { 0 };
        std::size_t   length(std::strlen(first->c_str()));
        bool          is_separator(*first == static_name_t("-"));

        item.cbSize = sizeof(item);
        item.fMask = MIIM_FTYPE | MIIM_ID | (is_separator ? 0 : MIIM_STRING);
        item.fType = is_separator ? MFT_SEPARATOR : MFT_STRING;
        item.wID = ++count;
        item.dwTypeData = (LPSTR)first->c_str();
        item.cch = (UINT)length;

        ::InsertMenuItemA(menu, count, false, &item);
    }

    long choice(::TrackPopupMenu(menu,
                                 TPM_LEFTALIGN           |
                                     TPM_TOPALIGN        |
                                     TPM_LEFTBUTTON      |
                                     TPM_HORPOSANIMATION |
                                     TPM_VERPOSANIMATION |
                                     TPM_NONOTIFY        |
                                     TPM_RETURNCMD,
                                 x, y, 0, parent, 0));

    ::DestroyMenu(menu);

    if (choice == 0)
        return false;

    result = *(the_first + choice - 1);

    return true;
}

/****************************************************************************************************/

void set_font(HWND window, int uxtheme_type)
{
    LOGFONTW        log_font = { 0 };
    HFONT           font = 0;

    //
    // Use the metrics to obtain the correct font for this widget.
    //
    metrics::set_window(window);

    if (metrics::get_font(uxtheme_type, log_font))
    {
        //
        // Create a font from the LOGFONT structure.
        //
        font = ::CreateFontIndirectW(&log_font);
    }
    assert(font);

    ::SendMessage(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), true);
}

/****************************************************************************************************/

void set_font_checkbox		(platform_display_type w) { return set_font (w, BP_CHECKBOX)		;}
void set_font_dropdownbutton(platform_display_type w) { return set_font (w, CP_DROPDOWNBUTTON)	;}
void set_font_edittext		(platform_display_type w) { return set_font (w, EP_EDITTEXT)		;}
void set_font_groupbox		(platform_display_type w) { return set_font (w, BP_GROUPBOX)		;}
void set_font_progressbar	(platform_display_type w) { return set_font (w, PP_BAR)				;}
void set_font_pushbutton	(platform_display_type w) { return set_font (w, BP_PUSHBUTTON)		;}
void set_font_radiobutton	(platform_display_type w) { return set_font (w, BP_RADIOBUTTON)		;}
void set_font_tabitem		(platform_display_type w) { return set_font (w, TABP_TABITEM)		;}
void set_font_thumbtop		(platform_display_type w) { return set_font (w, TKP_THUMBTOP)		;}
void set_font_thumbleft		(platform_display_type w) { return set_font (w, TKP_THUMBLEFT)		;}

/****************************************************************************************************/

bool is_null_control (platform_display_type control)
{
	return NULL == control;
}

/****************************************************************************************************/

platform_display_type get_parent_control (platform_display_type child)
{
	return ::GetParent (child);
}

/****************************************************************************************************/

void set_control_visible(platform_display_type control, bool make_visible)
{
    assert(control);

    ::ShowWindow(control, make_visible ? SW_SHOWNORMAL : SW_HIDE);
}

/****************************************************************************************************/

void set_control_enabled(platform_display_type control, bool make_enabled)
{
    assert(control);

    ::EnableWindow(control, make_enabled);
}

/****************************************************************************************************/

bool get_control_enabled(platform_display_type control)
{
    assert(control);

    return ::IsWindowEnabled(control) == TRUE;
}

/****************************************************************************************************/

void set_control_checked(platform_display_type control, check_state state)
{
	assert(control);

	WPARAM native_state(BST_INDETERMINATE);
	if (state)
		native_state = BST_CHECKED;
	else if (!state)
		native_state = BST_UNCHECKED;
	
	::SendMessage(control, BM_SETCHECK, native_state, 0);
}

/****************************************************************************************************/

LONG_PTR get_user_reference(HWND control)
{
    assert(control);

    return ::GetWindowLongPtr(control, GWLP_USERDATA);
}

/****************************************************************************************************/

bool is_key_return(const key_type & key)
{
	return VK_RETURN == key;
}

/****************************************************************************************************/

bool is_key_escape(const key_type & key)
{
	return VK_ESCAPE == key;
}

/****************************************************************************************************/

platform_display_type get_top_level_window(platform_display_type thing)
{
    return ::GetAncestor(thing, GA_ROOT);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif