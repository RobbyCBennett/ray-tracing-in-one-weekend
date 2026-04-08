#ifdef _WIN32
	#define NOCLIPBOARD
	#define NOCOLOR
	#define NOCTLMGR
	#define NODEFERWINDOWPOS
	#define NODRAWTEXT
	#define NOGDI
	#define NOHELP
	#define NOICONS
	#define NOIME
	#define NOKEYSTATES
	#define NOMB
	#define NOMENUS
	#define NOMINMAX
	// #define NOMSG
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
	#include <Windows.h>
#endif

#include "event.hpp"


#ifdef _WIN32
	static MouseButton mouse_button_from_msg(const MSG& message);
	static MouseVec mouse_move_from_msg(const MSG& message);
	static MouseVec mouse_scroll_from_msg(const MSG& message);
	static bool get_msg(MSG& message);
	static Event input_event_from_msg(MSG& message);
	static Key key_from_msg(const MSG& message);
#endif


EventValue::EventValue()
{}


EventValue::EventValue(U16 key_or_mouse_button):
	key_press(key_or_mouse_button)
{}


EventValue::EventValue(MouseVec mouse_vec):
	mouse_move(mouse_vec)
{}


Event Event::get()
{
	#ifdef _WIN32

		// Get a message from the OS or fail
		MSG message;
		if (not get_msg(message))
			return {};

		return input_event_from_msg(message);

	#else

		// TODO
		return {};

	#endif
}


Event Event::get_blocking()
{
	#ifdef _WIN32

		// Get a message from the OS or fail
		MSG message;
		if (not get_msg(message))
			return {};

		// Properly relay all messages back to the OS
		TranslateMessage(&message);
		DispatchMessageA(&message);

		return input_event_from_msg(message);

	#else

		// TODO
		return {};

	#endif
}


EventKind Event::get_kind() const
{
	return kind;
}


const EventValue& Event::get_value() const
{
	return value;
}


WindowId Event::get_window_id() const
{
	return window_id;
}


Event::operator bool() const
{
	return not none;
}


Event::Event():
	none(true)
{}


Event::Event(EventKind kind_, const EventValue& value_, WindowId window_id_):
	none(false), kind(kind_), value(value_), window_id(window_id_)
{}


#ifdef _WIN32


	static MouseButton mouse_button_from_msg(const MSG& message)
	{
		// The mouse button before back or forward
		constexpr MouseButton BEFORE = RIGHT_MOUSE_BUTTON;

		// TODO instead of this, try bit fields

		// Get the button from the second lowest 16 bits
		// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-xbuttondown
		return (MouseButton) (message.wParam >> 16) + BEFORE;
	}


	static MouseVec mouse_move_from_msg(const MSG& message)
	{
		// Get x from the lowest 16 bits and y from the second lowest 16 bits
		// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-MOUSE_MOVE
		const S16 x = (S16) message.lParam;
		const S16 y = (S16) ((U32) (message.lParam) >> 16);
		return MouseVec {x, y};
	}


	static MouseVec mouse_scroll_from_msg(const MSG& message)
	{
		// Get y from the second lowest 16 bits
		// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
		const S16 x = 0;
		const S16 y = (S16) (message.wParam >> 16);
		return MouseVec {x, y};
	}


	static bool get_msg(MSG& message)
	{
		return PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) != 0;
	}


	static Event input_event_from_msg(MSG& message)
	{
		// Get the window ID
		const WindowId window_id = message.hwnd;

		// Return an input event or fail
		switch (message.message) {
			// Key
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				return Event(EventKind::KEY_PRESS, key_from_msg(message), window_id);
			case WM_KEYUP:
			case WM_SYSKEYUP:
				return Event(EventKind::KEY_RELEASE, key_from_msg(message), window_id);

			// Mouse button
			case WM_LBUTTONDOWN:
				return Event(EventKind::MOUSE_BUTTON_CLICK, LEFT_MOUSE_BUTTON, window_id);
			case WM_LBUTTONUP:
				return Event(EventKind::MOUSE_BUTTON_REELASE, LEFT_MOUSE_BUTTON, window_id);
			case WM_MBUTTONDOWN:
				return Event(EventKind::MOUSE_BUTTON_CLICK, MIDDLE_MOUSE_BUTTON, window_id);
			case WM_MBUTTONUP:
				return Event(EventKind::MOUSE_BUTTON_REELASE, MIDDLE_MOUSE_BUTTON, window_id);
			case WM_RBUTTONDOWN:
				return Event(EventKind::MOUSE_BUTTON_CLICK, RIGHT_MOUSE_BUTTON, window_id);
			case WM_RBUTTONUP:
				return Event(EventKind::MOUSE_BUTTON_REELASE, RIGHT_MOUSE_BUTTON, window_id);
			case WM_XBUTTONDOWN:
				return Event(EventKind::MOUSE_BUTTON_CLICK, mouse_button_from_msg(message), window_id);
			case WM_XBUTTONUP:
				return Event(EventKind::MOUSE_BUTTON_REELASE, mouse_button_from_msg(message), window_id);

			// Mouse move
			case WM_MOUSEMOVE:
				return Event(EventKind::KEY_PRESS, mouse_move_from_msg(message), window_id);

			// Mouse scroll
			case WM_MOUSEWHEEL:
				return Event(EventKind::KEY_PRESS, mouse_scroll_from_msg(message), window_id);

			default:
				return {};
		}
	}


	static Key extended_key_from_msg(const MSG& message, Key regular_key, Key extended_key)
	{
		// Constant for modifier keys, where bit 0 is the least significant
		// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown
		//                                     *( Code )( Repeat Count )
		constexpr S64 EXTENDED_KEY_BIT = 0b1000000000000000000000000;

		return (message.lParam & EXTENDED_KEY_BIT) ? extended_key : regular_key;
	}


	static Key key_from_msg(const MSG& message)
	{
		// TODO verify this with Linux, especially the following
			// KEY_PRINT
			// KEY_STOPCD
			// KEY_APPSELECT

		// Constants for modifier keys, where bit 0 is the least significant
		// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown
		//                                            ( Code )( Repeat Count )
		constexpr S64 KEY_CODE_OF_RIGHT_SHIFT = 0b001101100000000000000000;
		constexpr S64 REPEAT_COUNT            = 0b000000001111111111111111;
		constexpr S64 REMOVE_REPEAT_COUNT     = ~REPEAT_COUNT;

		// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		switch (message.wParam) {
			case 0x08: return KEY_BACKSPACE;
			case 0x09: return KEY_TAB;
			case 0x0C: return KEY_CLEAR;
			case 0x0D:
				return extended_key_from_msg(message, KEY_ENTER, KEY_KPENTER);
			case 0x10:
				// Remove the lowest 16 bits, then see if the scan code matches
				if (((message.lParam & REMOVE_REPEAT_COUNT) ^ KEY_CODE_OF_RIGHT_SHIFT) == 0)
					return KEY_RIGHTSHIFT;
				else
					return extended_key_from_msg(message, KEY_LEFTSHIFT, KEY_RIGHTSHIFT);
			case 0x11:
				return extended_key_from_msg(message, KEY_LEFTCTRL, KEY_RIGHTCTRL);
			case 0x12:
				return extended_key_from_msg(message, KEY_LEFTALT, KEY_RIGHTALT);
			case 0x13: return KEY_PAUSE;
			case 0x14: return KEY_CAPSLOCK;
			case 0x1B: return KEY_ESC;
			case 0x20: return KEY_SPACE;
			case 0x21: return KEY_PAGEUP;
			case 0x22: return KEY_PAGEDOWN;
			case 0x23: return KEY_END;
			case 0x24: return KEY_HOME;
			case 0x25: return KEY_LEFT;
			case 0x26: return KEY_UP;
			case 0x27: return KEY_RIGHT;
			case 0x28: return KEY_DOWN;
			case 0x2C: return KEY_PRINT;
			case 0x2D: return KEY_INSERT;
			case 0x2E: return KEY_DELETE;
			case 0x2F: return KEY_HELP;
			case 0x30: return KEY_0;
			case 0x31: return KEY_1;
			case 0x32: return KEY_2;
			case 0x33: return KEY_3;
			case 0x34: return KEY_4;
			case 0x35: return KEY_5;
			case 0x36: return KEY_6;
			case 0x37: return KEY_7;
			case 0x38: return KEY_8;
			case 0x39: return KEY_9;
			case 0x41: return KEY_A;
			case 0x42: return KEY_B;
			case 0x43: return KEY_C;
			case 0x44: return KEY_D;
			case 0x45: return KEY_E;
			case 0x46: return KEY_F;
			case 0x47: return KEY_G;
			case 0x48: return KEY_H;
			case 0x49: return KEY_I;
			case 0x4A: return KEY_J;
			case 0x4B: return KEY_K;
			case 0x4C: return KEY_L;
			case 0x4D: return KEY_M;
			case 0x4E: return KEY_N;
			case 0x4F: return KEY_O;
			case 0x50: return KEY_P;
			case 0x51: return KEY_Q;
			case 0x52: return KEY_R;
			case 0x53: return KEY_S;
			case 0x54: return KEY_T;
			case 0x55: return KEY_U;
			case 0x56: return KEY_V;
			case 0x57: return KEY_W;
			case 0x58: return KEY_X;
			case 0x59: return KEY_Y;
			case 0x5A: return KEY_Z;
			case 0x5B: return KEY_LEFTMETA;
			case 0x5C: return KEY_RIGHTMETA;
			case 0x5D: return KEY_APPSELECT;
			case 0x5F: return KEY_SLEEP;
			case 0x60: return KEY_KP0;
			case 0x61: return KEY_KP1;
			case 0x62: return KEY_KP2;
			case 0x63: return KEY_KP3;
			case 0x64: return KEY_KP4;
			case 0x65: return KEY_KP5;
			case 0x66: return KEY_KP6;
			case 0x67: return KEY_KP7;
			case 0x68: return KEY_KP8;
			case 0x69: return KEY_KP9;
			case 0x6A: return KEY_KPASTERISK;
			case 0x6B: return KEY_KPPLUS;
			case 0x6D: return KEY_KPMINUS;
			case 0x6E: return KEY_KPDOT;
			case 0x6F: return KEY_KPSLASH;
			case 0x70: return KEY_F1;
			case 0x71: return KEY_F2;
			case 0x72: return KEY_F3;
			case 0x73: return KEY_F4;
			case 0x74: return KEY_F5;
			case 0x75: return KEY_F6;
			case 0x76: return KEY_F7;
			case 0x77: return KEY_F8;
			case 0x78: return KEY_F9;
			case 0x79: return KEY_F10;
			case 0x7A: return KEY_F11;
			case 0x7B: return KEY_F12;
			case 0x7C: return KEY_F13;
			case 0x7D: return KEY_F14;
			case 0x7E: return KEY_F15;
			case 0x7F: return KEY_F16;
			case 0x80: return KEY_F17;
			case 0x81: return KEY_F18;
			case 0x82: return KEY_F19;
			case 0x83: return KEY_F20;
			case 0x84: return KEY_F21;
			case 0x85: return KEY_F22;
			case 0x86: return KEY_F23;
			case 0x87: return KEY_F24;
			case 0x90: return KEY_NUMLOCK;
			case 0x91: return KEY_SCROLLLOCK;
			case 0xA0: return KEY_LEFTSHIFT;
			case 0xA1: return KEY_RIGHTSHIFT;
			case 0xA2: return KEY_LEFTCTRL;
			case 0xA3: return KEY_RIGHTCTRL;
			case 0xA4: return KEY_LEFTALT;
			case 0xA5: return KEY_RIGHTALT;
			case 0xAD: return KEY_MUTE;
			case 0xAE: return KEY_VOLUMEDOWN;
			case 0xAF: return KEY_VOLUMEUP;
			case 0xB0: return KEY_NEXTSONG;
			case 0xB1: return KEY_PREVIOUSSONG;
			case 0xB2: return KEY_STOPCD;
			case 0xB3: return KEY_PLAYPAUSE;
			case 0xBA: return KEY_SEMICOLON;
			case 0xBB: return KEY_EQUAL;
			case 0xBC: return KEY_COMMA;
			case 0xBD: return KEY_MINUS;
			case 0xBE: return KEY_DOT;
			case 0xBF: return KEY_SLASH;
			case 0xC0: return KEY_GRAVE;
			case 0xDB: return KEY_LEFTBRACE;
			case 0xDC: return KEY_BACKSLASH;
			case 0xDD: return KEY_RIGHTBRACE;
			case 0xDE: return KEY_APOSTROPHE;
			default: return KEY_RESERVED;
		}
	}


#endif // _WIN32
