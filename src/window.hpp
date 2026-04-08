#pragma once


#include "frame.hpp"


#ifdef _WIN32
	struct HDC__;
	struct HWND__;
	// Pointer to a window
	using WindowId = HWND__*;
	// General context of a window, before OpenGL/Vulkan context creation
	using WindowContext = HDC__*;
#else
	using WindowId = U64;
#endif


// The kind or style of a window
//
// - Windows: https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
using WindowKind = U32;


// Some specific window kinds that are common across systems
enum CommonWindowKind: WindowKind
{
	// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
	// 0x00010000L: maximize button enabled
	// 0x00020000L: minimize button enabled
	// 0x00040000L: able to change size
	// 0x00080000L: draw icon, minimize, maximize, and close buttons
	// 0x01000000L: maximized
	// 0x10000000L: visible
	// 0x20000000L: minimized
	// 0x80000000L: frameless
	FRAMED = 0x10000000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L,
	FRAMED_MAXIMIZED = FRAMED | 0x01000000L,
	FRAMED_MINIMIZED = FRAMED | 0x20000000L,
	FRAMELESS = 0x10000000L | 0x80000000L,
	FRAMELESS_MAXIMIZED = FRAMELESS | 0x01000000L,
	FRAMELESS_MINIMIZED = FRAMELESS | 0x20000000L,
};


// A GUI window
class Window
{
public:
	// Create from an existing window ID
	Window(WindowId id);

	// Draw a frame that was rendered on the CPU
	bool draw_frame(const Frame& frame);

private:
#ifdef _WIN32
	WindowContext window_context;
#endif
};



#ifdef _WIN32
	constexpr WindowId NO_PARENT_WINDOW = nullptr;
#else
	constexpr WindowId NO_PARENT_WINDOW = 0;
#endif
