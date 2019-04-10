#include "Assets.h"

#pragma region TEXTURE_CLASS
Texture::Texture(const uint32_t& width, const uint32_t& height, const uint32_t* pixels, const uint32_t& mip_levels, bool pixels_on_heap)
	: prv_Width(width), prv_Height(height), prv_Data(pixels), prv_MipLevels(mip_levels), prv_PixelsOnHeap(pixels_on_heap) {}


Texture::~Texture()
{
	if (prv_PixelsOnHeap)
		delete[] prv_Data;
}
#pragma endregion