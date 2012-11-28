/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_WINDOWS_MESSAGE_HANDLER_HPP
#define ADOBE_WINDOWS_MESSAGE_HANDLER_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/future/platform_primitives.hpp>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/*!
Class that manages a message handling function attched to a win32 HWND.
*/

class message_handler_t : boost::noncopyable
{
public:
	/*!
	Function prototype for the message handling function.
	\param window HWND that received the message
	\param message win32 message sent to the window
	\param wparam wparam argument sent with the message
	\param lparam lparam argument sent with the message
	\param next_proc the next WNDPROC in the handler chain; can be invoked with CallWindowProc to
	chain to the next handler
	\return LRESULT to be returned from the WNDPROC.
	*/

	typedef boost::function<window_proc_t> callback_t;

	message_handler_t(callback_t callback = callback_t());

	~message_handler_t();

	/*!
	Attach the message handler to the window with a specific string handle. A given string handle
	can be attached to a window at most once, attempting to re-install a handler with the same
	string handle on a given window is an error. Attempting to install a message handler to two
	windows at the same time is an error.

	Note that the ProcName template argument is a const char*, which requires a very specific
	coding style to be used by clients. The string being passed must be staticly defined, but
	cannot be a string literal, as the following example demonstrates:
	<code>
		extern const char my_proc_name_k[] = "my-unique-proc-name";

		message_handler_t handler(&my_handler_func);
		handler.install<my_proc_name_k>(my_hwnd);
	</code>
	*/
	template <const char* ProcName>
	inline void install(platform_display_type window)
		{ install_implementation(window, ProcName, &win_proc<ProcName>); }

	void uninstall();

	inline bool is_installed() const
#if defined ADOBE_PLATFORM_WT
		;
#elif defined ADOBE_PLATFORM_WIN
		{ return NULL != property_m; }
#else 
    #error "Unknown platform"
#endif

	inline void set_callback(callback_t callback)
#if defined ADOBE_PLATFORM_WT
		;
#elif defined ADOBE_PLATFORM_WIN
		{ callback_m = next_proc_handler(callback); }
#else 
    #error "Unknown platform"
#endif

private:

#if defined ADOBE_PLATFORM_WIN

	typedef boost::function<LRESULT (HWND window, UINT message, WPARAM wparam, LPARAM lparam, WNDPROC next_proc)> callback_w_next_proc_t;

	struct next_proc_handler
	{
		next_proc_handler (const callback_t & callback) : callback_m (callback)
		{}

		LRESULT operator() (HWND window, UINT message, WPARAM wparam, LPARAM lparam, WNDPROC next_proc)
		{
			callback_m (window, message, wparam, lparam);
			return ::CallWindowProc(next_proc, window, message, wparam, lparam);
		}
	private:
		callback_t callback_m;
	};

	struct property_t
	{
		message_handler_t*	handler_m;
		WNDPROC				next_proc_m;
	};

	property_t*	property_m;
	callback_w_next_proc_t	callback_m;

	void install_implementation(platform_display_type window, const char* proc_name, WNDPROC win_proc);

	static void set_handler_property(platform_display_type window, const char* proc_name, property_t* property);

	static void remove_handler_property(platform_display_type window, const char* proc_name);

	static property_t* get_handler_property(platform_display_type window, const char* proc_name);

	static LRESULT win_proc_implementation(HWND window, UINT message, WPARAM wparam, LPARAM lparam, const char* proc_name);

	template <const char* ProcName>
	static LRESULT CALLBACK win_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		return win_proc_implementation(window, message, wparam, lparam, ProcName);
	}
#endif //defined ADOBE_PLATORM8_WIN
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
