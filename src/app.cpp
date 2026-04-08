#ifdef _WIN32
	#define NOCLIPBOARD
	#define NOCOLOR
	// #define NOCTLMGR
	#define NODEFERWINDOWPOS
	#define NODRAWTEXT
	// #define NOGDI
	#define NOHELP
	#define NOICONS
	#define NOIME
	#define NOKEYSTATES
	#define NOMB
	#define NOMENUS
	#define NOMINMAX
	#define NOMSG
	// #define NONLS
	#define NOSCROLL
	#define NOSERVICE
	// #define NOSHOWWINDOW
	#define NOSYSCOMMANDS
	#define NOSYSMETRICS
	// #define NOUSER
	#define NOVIRTUALKEYCODES
	#define NOWH
	// #define NOWINMESSAGES
	#define NOWINOFFSETS
	// #define NOWINSTYLES
	#define OEMRESOURCE
	#define WIN32_LEAN_AND_MEAN
	#include <dwmapi.h>
#endif

#include "app.hpp"


static WindowId s_main_window_id = NO_PARENT_WINDOW;
static bool s_exiting = false;
static bool s_initialized = false;


static bool initialize_app();


App& App::instance()
{
	thread_local App app;
	return app;
}


bool App::initialized()
{
	return s_initialized;
}


WindowId App::create_main_window(
	const char* name,
	WindowKind kind,
	S32 x,
	S32 y,
	S32 width,
	S32 height)
{
	s_main_window_id = create_window(name, kind, x, y, width, height, NO_PARENT_WINDOW);
	return s_main_window_id;
}


WindowId App::create_window(
	const char* name,
	WindowKind kind,
	S32 x,
	S32 y,
	S32 width,
	S32 height,
	WindowId parent)
{
	// TODO add 2 ways to create a size:
	// - Outer size: size of non-client area if there's a title bar, otherwise
	//   the same as inner size
	// - Inner size: size of client-area

	// TODO consider a WindowOptions struct

	#ifdef _WIN32
		const bool framed = not (kind & WS_POPUP);
		const bool fullscreen = kind & WS_MAXIMIZE;
		const bool minimized = kind & WS_MINIMIZE;

		// Set the extended window style to remove the icon
		// https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
		const WindowKind extended_style = framed ? WS_EX_DLGMODALFRAME : 0;

		// White flash of death: hide framed window
		if (framed and not minimized)
			kind &= ~(U32) WS_VISIBLE;

		const WindowId window_id = CreateWindowExA(
			extended_style,
			"_",
			name,
			kind,
			x,
			y,
			width,
			height,
			parent,
			nullptr,
			GetModuleHandleA(nullptr),
			nullptr);

		if (window_id == nullptr)
			return nullptr;

		// Modify the window attributes
		// https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/nf-dwmapi-dwmsetwindowattribute
		// https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/ne-dwmapi-dwmwindowattribute
		// Frameless: add rounded corners and a set the border color
		if (not framed and not fullscreen) {
			// Rounded corners
			constexpr S32 corner = DWMWCP_ROUND;
			DwmSetWindowAttribute(window_id, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

			// Border color
			constexpr U32 border = DWMWA_COLOR_NONE;
			DwmSetWindowAttribute(window_id, DWMWA_BORDER_COLOR, &border, sizeof(border));
		}
		// Framed: set the top bar color and border color
		else if (framed) {
			// Top bar color
			constexpr U32 top = RGB(24, 26, 31);
			DwmSetWindowAttribute(window_id, DWMWA_CAPTION_COLOR, &top, sizeof(top));

			// Border color
			constexpr U32 border = DWMWA_COLOR_NONE;
			DwmSetWindowAttribute(window_id, DWMWA_BORDER_COLOR, &border, sizeof(border));

			// White flash of death: show framed window
			if (not minimized) {
				SetWindowPos(window_id, HWND_TOP, x, y, width, height, SWP_NOMOVE);
				if (fullscreen)
					ShowWindow(window_id, SW_SHOWMAXIMIZED);
				else
					ShowWindow(window_id, SW_SHOWNORMAL);
			}
		}

		return window_id;

	#else
		// TODO
		return NO_PARENT_WINDOW;
	#endif
}


bool App::exiting()
{
	return s_exiting;
}


void App::exit()
{
	s_exiting = true;

	#ifdef _WIN32
		PostQuitMessage(0);
	#endif
}


WindowId App::main_window()
{
	return s_main_window_id;
}


void App::set_main_window(WindowId handle)
{
	s_main_window_id = handle;
}


App::App()
{
	s_initialized = initialize_app();
}


#ifdef _WIN32
	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
	static S64 handle_window_event(
		WindowId window_id,
		U32 message,
		U64 extra_w,
		S64 extra_l)
	{
		// Close program if the main window is closed
		if (message == WM_DESTROY and window_id == s_main_window_id) {
			App::instance().exit();
			return 0;
		}

		// Default window procedure for other events
		return DefWindowProcA(window_id, message, extra_w, extra_l);
	}
#endif


bool static initialize_app()
{
	#ifdef _WIN32

		// Fix the console to use UTF-8
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);

		// Use the real size and position
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		// Parameters to register window classification
		const WNDCLASSEXA window_class = {
			.cbSize        = sizeof(WNDCLASSEXA),
			.style         = 0,
			.lpfnWndProc   = handle_window_event,
			.cbClsExtra    = 0,
			.cbWndExtra    = 0,
			.hInstance     = GetModuleHandleA(nullptr),
			.hIcon         = nullptr,
			.hCursor       = LoadCursorA(nullptr, IDC_ARROW),
			.hbrBackground = CreateSolidBrush(RGB(29, 32, 38)),
			.lpszMenuName  = nullptr,
			.lpszClassName = "_",
			.hIconSm       = nullptr,
		};

		// Register window classification
		return RegisterClassExA(&window_class) != 0;

	#else

		// TODO
		return false;

	#endif
}
