#ifndef ASSETS_H
#define ASSETS_H

#include "Defines.h"

class Texture
{

public:
	Texture() = default;
	Texture(const uint32_t width, const uint32_t& height, const uint32_t* pixels, bool pixels_on_heap = true);
	~Texture();

	const uint32_t get_width() const { return prv_Width; }
	const uint32_t get_height() const { return prv_Height; }
	const uint32_t* get_data() const { return prv_Data; }

	void set_width(uint32_t width) { prv_Width = width; }
	void set_height(uint32_t height) { prv_Height = height; }
	void set_data(const uint32_t* pixels) { prv_Data = pixels; }

	__declspec(property(get = get_width, put = set_width)) uint32_t width;
	__declspec(property(get = get_height, put = set_height)) uint32_t height;
	__declspec(property(get = get_data, put = set_data)) const uint32_t* data;

private:
	uint32_t prv_Width, prv_Height;
	bool prv_PixelsOnHeap;
	const uint32_t* prv_Data;
};

#endif