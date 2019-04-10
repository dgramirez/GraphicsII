#include "Assets.h"



Texture::Texture(const uint32_t width, const uint32_t& height, const uint32_t* pixels, bool pixels_on_heap)
	: prv_Width(width), prv_Height(height), prv_Data(pixels), prv_PixelsOnHeap(pixels_on_heap) {}


Texture::~Texture()
{
	if (prv_PixelsOnHeap)
		delete[] prv_Data;
}
