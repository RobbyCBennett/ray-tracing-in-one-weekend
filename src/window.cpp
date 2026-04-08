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
	#define NONLS
	#define NOSCROLL
	#define NOSERVICE
	#define NOSHOWWINDOW
	#define NOSYSCOMMANDS
	#define NOSYSMETRICS
	// #define NOUSER
	#define NOVIRTUALKEYCODES
	#define NOWH
	// #define NOWINMESSAGES
	#define NOWINOFFSETS
	#define NOWINSTYLES
	#define OEMRESOURCE
	#define WIN32_LEAN_AND_MEAN
	#include <dwmapi.h>
#endif

#include "window.hpp"


Window::Window(WindowId id_)
{
	#ifdef _WIN32
		window_context = GetDC(id_);
	#else
		// TODO
	#endif
}


bool Window::draw_frame(const Frame& frame)
{
	#ifdef _WIN32
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wmissing-field-initializers"
		const BITMAPINFO bitmap_info {
			.bmiHeader = {
				.biSize = sizeof(BITMAPINFOHEADER),
				.biWidth = frame.get_width(),
				.biHeight = -frame.get_height(),
				.biPlanes = 1,
				.biBitCount = 32,
			},
		};
		#pragma clang diagnostic pop

		return StretchDIBits(
			window_context,
			0,
			0,
			frame.get_width(),
			frame.get_height(),
			0,
			0,
			frame.get_width(),
			frame.get_height(),
			frame.get_data(),
			&bitmap_info,
			DIB_RGB_COLORS,
			SRCCOPY
		) != 0;
	#else
		// TODO
		return false;
	#endif
}
