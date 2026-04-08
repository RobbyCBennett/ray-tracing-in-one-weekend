#pragma once


#include "window.hpp"


// The only GUI application
class App
{
public:
	// Get the only instance for this thread
	static App& instance();

	// Whether the GUI is able to be used
	static bool initialized();

	// Whether the main window closed or `exit` was called
	static bool exiting();

	// Get the main window
	// * ENSURE: return value is not null
	static WindowId main_window();

	// Create a new window for this thread and set it as the main window
	// * ENSURE: return value is not null
	WindowId create_main_window(
		const char* name,
		WindowKind kind,
		S32 x,
		S32 y,
		S32 width,
		S32 height);

	// Create a new window for this thread
	// * ENSURE: return value is not null
	WindowId create_window(
		const char* name,
		WindowKind kind,
		S32 x,
		S32 y,
		S32 width,
		S32 height,
		WindowId parent = NO_PARENT_WINDOW);

	// Manually exit
	void exit();

	// Set the main window
	void set_main_window(WindowId id);

private:
	App();

	App(const App&) = delete;
	App operator=(const App&) = delete;
};
