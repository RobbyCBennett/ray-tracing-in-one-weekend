#pragma once


#include "key.hpp"
#include "window.hpp"


// A number corresponding to a mouse button
using MouseButton = U16;

// Vector of a mouse position
struct MouseVec
{
	S16 x;
	S16 y;
};


// Common buttons on a mouse
enum CommonMouseButton: MouseButton
{
	LEFT_MOUSE_BUTTON,
	MIDDLE_MOUSE_BUTTON,
	RIGHT_MOUSE_BUTTON,
	BACK_MOUSE_BUTTON,
	FORWARD_MOUSE_BUTTON,
};


// Kinds of events provided by this library
enum EventKind: U8
{
	KEY_PRESS,
	KEY_RELEASE,
	KEY_REPEAT,
	MOUSE_BUTTON_CLICK,
	MOUSE_BUTTON_DOUBLE_CLICK,
	MOUSE_BUTTON_REELASE,
	MOUSE_MOVE,
	MOUSE_MOVE_ENTER,
	MOUSE_MOVE_LEAVE,
	MOUSE_SCROLL,
};


// The actual value in an event
union EventValue
{
	Key key_press;
	Key key_press_repeat;
	Key key_release;
	MouseButton mouse_button_click;
	MouseButton mouse_button_double_click;
	MouseButton mouse_button_release;
	MouseVec mouse_move;
	MouseVec mouse_move_enter;
	MouseVec mouse_move_leave;
	MouseVec mouse_scroll;

	// None
	EventValue();

	// Create from a key or mouse button
	EventValue(U16 key_or_mouse_button);

	// Create from a mouse vector
	EventValue(MouseVec mouse_vec);
};


// A message from the system like input or a window change
class Event
{
public:
	// None
	Event();

	// Create an event
	// - Be sure that the kind and value match each other as in the definition
	//   of `EventValue`.
	Event(EventKind kind, const EventValue& value, WindowId window_id);

	// Possibly get an event
	//
	// Windows:
	// - This must be called on the App thread.
	static Event get();

	// Handle window messages and possibly get an input event
	//
	// Windows:
	// - This blocks when the user interacts with the window frame.
	// - This must be called on the App thread.
	static Event get_blocking();

	// Get the input kind, which is tied to the value
	EventKind get_kind() const;

	// Get the actual input value, which is tied to the kind
	const EventValue& get_value() const;

	// Get the ID of the window where the input event occured
	WindowId get_window_id() const;

	// Whether it is not none
	operator bool() const;

private:
	bool none;
	EventKind kind;
	EventValue value;
	WindowId window_id;
};
