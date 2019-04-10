#ifndef ASSETS_H
#define ASSETS_H

#include "Defines.h"

class Texture
{

public:
	Texture() = default;
	Texture(const uint32_t& width, const uint32_t& height, const uint32_t* pixels, const uint32_t& mip_levels, bool pixels_on_heap = true);
	~Texture();

	const uint32_t get_width() const { return prv_Width; }
	const uint32_t get_height() const { return prv_Height; }
	const uint32_t* get_data() const { return prv_Data; }
	const uint32_t get_mip_levels() const { return prv_MipLevels; }
	const bool get_pixel_on_heap() const { return prv_PixelsOnHeap; }

	void set_width(const uint32_t& width) { prv_Width = width; }
	void set_height(const uint32_t& height) { prv_Height = height; }
	void set_data(const uint32_t* pixels) { prv_Data = pixels; }
	void set_mip_levels(const uint32_t& mip_levels) { prv_MipLevels = mip_levels; }

	__declspec(property(get = get_width, put = set_width)) uint32_t width;
	__declspec(property(get = get_height, put = set_height)) uint32_t height;
	__declspec(property(get = get_data, put = set_data)) const uint32_t* data;
	__declspec(property(get = get_mip_levels, put = set_mip_levels)) const uint32_t mip_levels;
	__declspec(property(get = get_pixel_on_heap)) bool on_heap;

private:
	uint32_t prv_Width, prv_Height, prv_MipLevels;
	bool prv_PixelsOnHeap;
	const uint32_t* prv_Data;
};

#endif