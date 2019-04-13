#ifndef ASSETS_H
#define ASSETS_H

#include "Defines.h"

class Texture
{

public:
	Texture() = default;
	Texture(const char* filename);
	Texture(const int32_t &width, const int32_t &height, const uint32_t *pixels, const uint32_t &mip_levels, const bool &pixels_on_heap = true);
	~Texture();

	const uint32_t get_width() const { return *prv_Width; }
	const uint32_t get_height() const { return *prv_Height; }
	const uint32_t* get_data_h() const { return prv_DataH; }
	const unsigned char* get_data() const { return prv_Data; }
	const uint32_t get_mip_levels() const { return prv_MipLevels; }
	const bool get_pixel_on_heap() const { return prv_PixelsOnHeap; }

	__declspec(property(get = get_width)) uint32_t width;
	__declspec(property(get = get_height)) uint32_t height;
	__declspec(property(get = get_data_h)) const uint32_t* data_h;
	__declspec(property(get = get_data)) const unsigned char* data;
	__declspec(property(get = get_mip_levels)) const uint32_t mip_levels;
	__declspec(property(get = get_pixel_on_heap)) bool on_heap;

private:
	const uint32_t *prv_DataH;
	const unsigned char *prv_Data;
	int32_t *prv_Width, *prv_Height, prv_MipLevels;
	bool prv_PixelsOnHeap;
};

#endif