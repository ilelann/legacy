/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_PLATFORM_PRIMITIVES_EDGE_HPP
#define ADOBE_PLATFORM_PRIMITIVES_EDGE_HPP

/****************************************************************************************************/

#include <boost/logic/tribool.hpp>
#include <string>

#if defined ADOBE_PLATFORM_WT

#if 1
#   include <Wt/WCheckBox>
#   include <Wt/WContainerWidget>
#   include <Wt/WFormWidget>
#   include <Wt/WPanel>
#   include <Wt/WPushButton>
#   include <Wt/WRadioButton>
#   include <Wt/WSlider>
#   include <Wt/WTabWidget>
#   include <Wt/WText>
#endif

#   include <Wt/WGlobal>
#   include <Wt/WObject>

namespace Wt
{
	class WCheckBox;
	class WContainerWidget;
	class WFormWidget;
	class WPanel;
	class WPushButton;
	class WRadioButton;
	class WSlider;
	class WText;
	class WTabWidget;
}

#else
#   define WINDOWS_LEAN_AND_MEAN 1
#   include <windows.h>
#	include <boost/gil/gil_all.hpp>
#	include <uxtheme.h>
#	include <tchar.h>

#if (NTDDI_VERSION >= NTDDI_LONGHORN)
#include <vssym32.h>
#else
	//Yuck:
#	include <tmschema.h>
#	define SCHEME_STRINGS 1
#	include <tmschema.h>
	//End yuck
#endif
#endif

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

#if defined ADOBE_PLATFORM_WT
	typedef Wt::WWidget* platform_display_type;
	typedef int timer_handle_t;
	typedef void window_proc_t();
	typedef int label_message_t;
	typedef void raw_callback_t ();
	typedef void win_proc_implementation_t ();

	typedef std::string image_type;
	typedef struct { int x; int y;} point_t;

	typedef Wt::Key key_type;

	// widget handles
	typedef Wt::WCheckBox*			native_checkbox_t;
	typedef Wt::WText*				native_label_t;
	typedef Wt::WPanel*				native_panel_t;
	typedef Wt::WPushButton*		native_button_t;
	typedef Wt::WRadioButton*		native_radio_button_t;
	typedef Wt::WSlider*			native_slider_t;
	typedef Wt::WText*				native_label_t;
	typedef Wt::WTabWidget*			native_tab_group_t;
	typedef Wt::WContainerWidget*	native_window_t;


#define ADOBE_PLATFORM_WIDGET_BASES Wt::WObject, boost::noncopyable

#else // if defined ADOBE_PLATFORM_WIN
	typedef HWND platform_display_type;
	typedef UINT_PTR timer_handle_t;
//	typedef WNDPROC window_proc_t;
	typedef LRESULT window_proc_t (HWND window, UINT message, WPARAM wParam, LPARAM lParam);
	
	typedef LRESULT label_message_t (UINT message, WPARAM wParam, LPARAM lParam);
	typedef LRESULT raw_callback_t (HWND window, UINT message, WPARAM wparam, LPARAM lparam, WNDPROC next_proc);
	typedef LRESULT win_proc_implementation_t (HWND window, UINT message, WPARAM wparam, LPARAM lparam, const char* proc_name);

	typedef boost::gil::rgba8_image_t image_type;
	typedef POINTS point_t;

	typedef long key_type;

	// widget handles
	typedef HWND native_checkbox_t;
	typedef HWND native_label_t;
	typedef HWND native_panel_t;
	typedef HWND native_button_t;
	typedef HWND native_radio_button_t;
	typedef HWND native_label_t;
	typedef HWND native_slider_t;
	typedef HWND native_tab_group_t;
	typedef HWND native_window_t;

	#define ADOBE_PLATFORM_WIDGET_BASES boost::noncopyable
#endif

	// temp typedef to track my changes
	struct place_data_t;
	typedef place_data_t place_data_liukahr_t; // RECT
	typedef wchar_t wide_char_t;
	typedef boost::logic::tribool check_state;


/*************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
