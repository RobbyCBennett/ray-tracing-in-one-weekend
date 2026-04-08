#pragma once


#include "float.hpp"
#include "int.hpp"


// Result of the resize method
enum class ResizeResult
{
	// New data failed to allocate
	AllocateFailure,
	// New data was allocated with a new size
	AllocateSuccess,
	// No change
	SameSize,
};


// A collection of pixels for CPU rendering
class Frame
{
public:
	// Try to create a frame with a size
	Frame() = default;

	// Try to create a frame with a size
	Frame(S32 width, S32 height);

	// Move constructor
	Frame(Frame&& frame) = default;
	// Move assignment operator
	Frame& operator=(Frame&& frame) = default;

	// Free the pixels
	~Frame();

	// Get the pixel data
	const U32 * get_data() const;

	// Get the width in pixels
	S32 get_width() const;
	// Get the height in pixels
	S32 get_height() const;

	// Change the size if the area is different
	// - New data is uninitialized.
	// - The width and height become zero on failure to allocate.
	ResizeResult resize(S32 width, S32 height);

	// Set a pixel in the frame with an integer color
	// - Assumes that the previous resize, if any, was successful
	// - Assumes that x is the range [0, width)
	// - Assumes that y is the range [0, height)
	void set_pixel(S32 x, S32 y, U8 red, U8 green, U8 blue)
	{
		#ifdef _WIN32
			U32 *const pixel = data + y * width + x;
			constexpr U32 ALPHA = (U32) 255 << 24;
			const U32 new_red = (U32) red << 16;
			const U32 new_green = (U32) green << 8;
			*pixel = ALPHA | new_red | new_green | blue;
		#endif
	}

	// Set a pixel in the frame with a doubleing-point color
	// - Assumes that the previous resize, if any, was successful
	// - Assumes that x is the range [0, width)
	// - Assumes that y is the range [0, height)
	// - Assumes that red, green, and blue are the range [0, 1]
	void set_pixel(S32 x, S32 y, Float red, Float green, Float blue)
	{
		#ifdef _WIN32
			U32 *const pixel = data + y * width + x;
			constexpr U32 ALPHA = (U32) 255 << 24;
			const U32 new_red = (U32) (red * 255) << 16;
			const U32 new_green = (U32) (green * 255) << 8;
			const U32 new_blue = (U32) (blue * 255);
			*pixel = ALPHA | new_red | new_green | new_blue;
		#endif
	}

	// Whether it is not none
	operator bool() const;

private:
	S32 width;
	S32 height;
	U32* data = nullptr;

	Frame(S32 width, S32 height, U32* data);

	Frame(const Frame& frame) = delete;
	Frame operator=(const Frame& frame) = delete;
};
