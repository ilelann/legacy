/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_UI_CORE_OS_UTILITIES_HPP
#define ADOBE_UI_CORE_OS_UTILITIES_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/future/windows_cast.hpp>
#include <adobe/future/platform_primitives.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/name_fwd.hpp>
#include <adobe/widget_attributes.hpp>
#include <adobe/future/widgets/headers/slider_helper.hpp>
#include <boost/filesystem/path.hpp>

#include <string>

/****************************************************************************************************/

#ifndef ADOBE_PLATFORM_WT
#ifndef ADOBE_THROW_LAST_ERROR
    #define ADOBE_THROW_LAST_ERROR adobe::implementation::throw_last_error_exception(__FILE__, __LINE__)
#endif
#endif

/****************************************************************************************************/

namespace hackery {

/****************************************************************************************************/

#ifndef ADOBE_PLATFORM_WT
std::string convert_utf(const WCHAR* buffer, std::size_t size);
std::string convert_utf(const WCHAR* buffer);

#ifndef ADOBE_PLATFORM_CYGWIN
    std::wstring convert_utf(const CHAR* buffer, std::size_t size);
    std::wstring convert_utf(const CHAR* buffer);

    inline std::wstring convert_utf(const std::string& name)
    { return convert_utf(name.c_str(), name.size()); }
#endif
#endif

/****************************************************************************************************/

} // namespace hackery

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct button_t;
struct checkbox_t;
struct radio_button_t;
struct slider_t;

namespace implementation {

/****************************************************************************************************/

native_button_t			make_button			(platform_display_type parent, const std::string & label, bool is_default);
native_checkbox_t		make_checkbox		(platform_display_type parent, const std::string & label);
native_label_t			make_label			(platform_display_type parent, const std::string & label);
native_panel_t			make_panel			(platform_display_type parent);
native_radio_button_t	make_radio_button	(platform_display_type parent, const std::string & label);
native_slider_t			make_slider			(platform_display_type parent, bool is_vertical, std::size_t num_ticks, slider_style_t style);
native_tab_group_t		make_tab_group		(platform_display_type parent);
native_window_t			make_window			(platform_display_type parent, const std::string & label, const bool & has_size_box);

/****************************************************************************************************/

void setup_callback_button			(adobe::button_t & element);
void setup_callback_checkbox		(adobe::checkbox_t & element);
void setup_callback_radio_button	(adobe::radio_button_t & element);
void setup_callback_slider			(adobe::slider_t & element);

/****************************************************************************************************/

typedef std::string display_string;

std::string get_window_title(platform_display_type window);

void set_window_title(platform_display_type window, const display_string & title);

//void set_window_image (platform_display_type window, const image_type & image);

inline display_string get_control_string(platform_display_type control)
{ return implementation::get_window_title(control); }

inline void set_control_string(platform_display_type control, const display_string & str)
{ return implementation::set_window_title(control, str); }


void get_control_bounds(platform_display_type control, place_data_t& p);


void set_control_bounds(platform_display_type control, const place_data_t& place_data);

template <typename T>
inline display_string get_field_text(T& x)
{ return get_control_string(x.control_m); }

template <>
inline display_string get_field_text<platform_display_type>(platform_display_type& x)
{ return get_control_string(x); }

/****************************************************************************************************/


template <typename T>
inline bool is_focused(T& control)
{ return is_focused(control.control_m); }

#ifdef ADOBE_PLATFORM_WT

template <>
inline bool is_focused(platform_display_type& control)
{ 
	throw;
	return true /*::GetFocus() == control*/; 
}

#else

template <>
inline bool is_focused(platform_display_type& control)
{ return ::GetFocus() == control; }

#endif

/****************************************************************************************************/



#ifndef ADOBE_PLATFORM_WT

inline void to_native (const place_data_t & p, RECT & r)
{
	r.left = left(p);
	r.top = top(p);
	r.right = right(p);
	r.bottom = bottom(p);
}

inline void from_native (const RECT & r, place_data_t & p)
{
	top(p) = r.top;
	left(p) = r.left;
	height(p) = r.bottom - r.top;
	width(p) = r.right - r.left;
}

modifiers_t modifier_state();

/****************************************************************************************************/

void throw_last_error_exception(const char* file, long line);

/****************************************************************************************************/

#endif

} // namespace implementation

/****************************************************************************************************/


#ifndef ADOBE_PLATFORM_WT

//
/// Win32's event mechanism is tweaked in that sometimes a parent of a widget is called when the user
/// changes something about the widget. forward_message is intended to let the pertinent window have
/// a stab at the event. We want to use the window's standard message handling system so we don't have
/// to introduce any new event hierarchies.
///
/// /return true if the forward_result should be used as a return value; false otherwise.
//

bool forward_message(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& forward_result);

/****************************************************************************************************/

LONG_PTR get_user_reference(platform_display_type control);

/****************************************************************************************************/

template <typename T>
void set_user_reference(platform_display_type control, T data)
{
    assert(control);

    ::SetWindowLongPtr(control, GWLP_USERDATA, hackery::cast<LONG>(data));
}


#endif //ADOBE_PLATFORM_WT


/****************************************************************************************************/

bool is_null_control (platform_display_type control);

template <typename T>
T check_not_null (T t)
{
	assert (!is_null_control (t));

#ifndef ADOBE_PLATFORM_WT
	if (is_null_control(t))
        ADOBE_THROW_LAST_ERROR;
#endif

	return t;
}

/****************************************************************************************************/

platform_display_type get_parent_control (platform_display_type child);

/****************************************************************************************************/

void set_control_visible(platform_display_type control, bool make_visible);

/****************************************************************************************************/

void set_control_enabled(platform_display_type control, bool make_enabled);

/****************************************************************************************************/

bool get_control_enabled(platform_display_type control);

/****************************************************************************************************/

void set_control_checked(platform_display_type control, check_state state);

/****************************************************************************************************/

bool context_menu(platform_display_type parent,
                  long x, long y,
                  const name_t* first,
                  const name_t* last,
                  name_t& result);

/****************************************************************************************************/

//
/// Information on the uxtheme_type parameter's values is
/// available in the "Parts and States" documentation here:
/// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/userex/topics/partsandstates.asp
//

void set_font_checkbox		(platform_display_type control);
void set_font_dropdownbutton(platform_display_type control);
void set_font_edittext		(platform_display_type control);
void set_font_groupbox		(platform_display_type control);
void set_font_progressbar	(platform_display_type control);
void set_font_pushbutton	(platform_display_type control);
void set_font_radiobutton	(platform_display_type control);
void set_font_tabitem		(platform_display_type control);
void set_font_thumbtop		(platform_display_type control);
void set_font_thumbleft		(platform_display_type control);


/****************************************************************************************************/

bool is_key_return(const key_type & key);

/****************************************************************************************************/

bool is_key_escape(const key_type & key);

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
