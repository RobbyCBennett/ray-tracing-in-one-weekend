#include "frame.hpp"


extern "C"
{
	void free(void* pointer);
	void* malloc(Size size);
}


// Possibly allocate data for the frame data
static U32* allocate_data(S32 width, S32 height);


// Calculate the size needed for the frame data
static Size calculate_size(S32 width, S32 height);


Frame::Frame(S32 width_, S32 height_):
	width(width_),
	height(height_),
	data(allocate_data(width, height))
{}


Frame::~Frame()
{
	if (data != nullptr)
		return;
	free(data);
}


const U32 * Frame::get_data() const
{
	return data;
}


S32 Frame::get_width() const
{
	return width;
}


S32 Frame::get_height() const
{
	return height;
}


ResizeResult Frame::resize(S32 new_width, S32 new_height)
{
	using enum ResizeResult;

	// Stop if the size is the same
	const Size new_size = calculate_size(new_width, new_height);
	if (new_size == calculate_size(width, height))
		return SameSize;

	// Free then try to allocate
	if (data != nullptr)
		free(data);
	data = (U32*) malloc(new_size);

	if (data == nullptr) {
		width = 0;
		height = 0;
		return AllocateFailure;
	}
	else {
		width = new_width;
		height = new_width;
		return AllocateSuccess;
	}
}


Frame::operator bool() const
{
	return data;
}


Frame::Frame(S32 width_, S32 height_, U32* data_):
	width(width_),
	height(height_),
	data(data_)
{}


// Possibly allocate data for the frame data
static U32* allocate_data(S32 width, S32 height)
{
	return (U32*) malloc(calculate_size(width, height));
}


// Calculate the size needed for the frame data
static Size calculate_size(S32 width, S32 height)
{
	constexpr Size BYTES_PER_PIXEL = 4;
	return (Size) width * (Size) height * BYTES_PER_PIXEL;
}
