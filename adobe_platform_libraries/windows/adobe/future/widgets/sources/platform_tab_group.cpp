/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/algorithm.hpp>
#include <adobe/algorithm/find_match.hpp>

#include <adobe/future/widgets/headers/platform_tab_group.hpp>
#include <adobe/future/widgets/headers/platform_label.hpp>
#include <adobe/future/widgets/headers/display.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

#ifdef ADOBE_PLATFORM_WT

void setup_callbacks(adobe::tab_group_t & element)
{
	element.control_m->currentChanged().connect(&element, &adobe::tab_group_t::on_tab_changed);
}

#else

LRESULT CALLBACK tab_group_subclass_proc(HWND     window,
                                         UINT     message,
                                         WPARAM   wParam,
                                         LPARAM   lParam,
                                         UINT_PTR ptr,
                                         DWORD_PTR /* ref */)
{
	adobe::tab_group_t& tab_group(*reinterpret_cast<adobe::tab_group_t*>(ptr));
    NMHDR*                         notice((NMHDR*) lParam);

    //
    // Here we report the selected tab changing to ui-core users.
    //

    if (message == WM_NOTIFY && notice && notice->hwndFrom == window)
    {
        if (notice->code == TCN_SELCHANGE)
        {
            long index(static_cast<long>(::SendMessage(tab_group.control_m, TCM_GETCURSEL, 0, 0)));
			tab_group.on_tab_changed (index);
        }

        return 0;
    }
    else
    {
        LRESULT forward_result(0);

		if (adobe::forward_message(message, wParam, lParam, forward_result))
            return forward_result;
    }

    return ::DefSubclassProc(window, message, wParam, lParam);
}

void setup_callbacks(adobe::tab_group_t & element)
{
	::SetWindowSubclass(element.control_m, &tab_group_subclass_proc, reinterpret_cast<UINT_PTR>(&element), 0);
}

#endif

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

tab_group_t::tab_group_t(const tab_t* first, const tab_t* last, theme_t theme)
: control_m(NULL), theme_m(theme), value_proc_m(), items_m(first, last)
{
}

/****************************************************************************************************/

void tab_group_t::on_tab_changed (int index)
{
	if (!value_proc_m.empty())
		value_proc_m(items_m[index].value_m);
}

/****************************************************************************************************/

void tab_group_t::initialize(platform_display_type parent)
{
    assert (is_null_control(control_m));

#if defined ADOBE_PLATFORM_WT

	control_m = new Wt::WTabWidget(dynamic_cast<Wt::WContainerWidget*>(parent));

#else

    control_m = ::CreateWindowEx(WS_EX_COMPOSITED | WS_EX_CONTROLPARENT,
                                 WC_TABCONTROL,
                                 NULL,
                                 WS_CHILD | WS_VISIBLE, 
                                 0, 0, 10, 10,
                                 parent,
                                 0,
                                 ::GetModuleHandle(NULL),
                                 NULL);
#endif

    if (is_null_control(control_m))
        ADOBE_THROW_LAST_ERROR;

    set_font_tabitem(control_m);

	setup_callbacks(*this);

    for (tab_set_t::iterator first(items_m.begin()), last(items_m.end()); first != last; ++first)
    {
        TCITEM  item = { TCIF_TEXT };

        std::wstring localized(hackery::convert_utf(first->name_m));
        item.pszText = const_cast<wchar_t*>(localized.c_str());
        item.cchTextMax = static_cast<int>(localized.size());

        ::SendMessage(control_m, TCM_INSERTITEM, ::SendMessage(control_m, TCM_GETITEMCOUNT, 0, 0), (LPARAM) &item);
    }
}

/****************************************************************************************************/

void tab_group_t::measure(extents_t& result)
{
    assert(control_m);

    // REVISIT (fbrereto) : A lot of static metrics values added here

    for (tab_set_t::iterator first(items_m.begin()), last(items_m.end()); first != last; ++first)
    {
        extents_t attrs;
        measure_label_text(label_t(first->name_m, std::string(), 0, theme_m), attrs, get_parent_control(control_m));

        result.width() += attrs.width() + 18;
        result.height() = (std::max)(result.height(), attrs.height());
    }

    result.vertical().frame_m.first = result.height() + 7;

    result.height() = 5;
}

/****************************************************************************************************/

void tab_group_t::place(const place_data_t& place_data)
{
    implementation::set_control_bounds(control_m, place_data);
}

/****************************************************************************************************/

void tab_group_t::display(const any_regular_t& new_value)
{
    assert(control_m);

    tab_set_t::iterator iter(find_match(items_m, new_value, compare_members(&tab_t::value_m, std::equal_to<any_regular_t>())));

    if (iter == items_m.end()) return;

    ::SendMessage(control_m, TCM_SETCURSEL, iter - items_m.begin(), 0);
}

/****************************************************************************************************/

void tab_group_t::monitor(const tab_group_value_proc_t& proc)
{
    assert(control_m);

    if (!value_proc_m)
        value_proc_m = proc;
}

/****************************************************************************************************/

template <>
platform_display_type insert<tab_group_t>(display_t&             display,
                                                platform_display_type&  parent,
                                                tab_group_t&     element)
{
    platform_display_type parent_hwnd(parent);

    element.initialize(parent_hwnd);

    return display.insert(parent, element.control_m);
}

/****************************************************************************************************/

} //namespace adobe
