#include "Assets.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_PNG
#define STB_ONLY_BMP
#include "../vendor/stb_image/stb_image.h"

#pragma region TEXTURE_CLASS
Texture::Texture(const int32_t &width, const int32_t &height, const uint32_t *pixels, const uint32_t &mip_levels, const bool &pixels_on_heap)
	: prv_DataH(pixels), prv_MipLevels(mip_levels), prv_PixelsOnHeap(pixels_on_heap) 
{
	prv_Width = new int32_t;
	prv_Height = new int32_t;
	*prv_Width = width;
	*prv_Height = height;
}

Texture::Texture(const char* filename)
{
	prv_Width = new int32_t;
	prv_Height = new int32_t;
	prv_Data = stbi_load(filename, prv_Width, prv_Height, nullptr, 4);
	prv_MipLevels = (int32_t)log2(std::min(*prv_Width, *prv_Height)) + 1;
}

Texture::~Texture()
{
	if (prv_PixelsOnHeap)	delete[] prv_DataH;
	if (prv_Data)			stbi_image_free(&prv_Data);
	if (prv_Width)			delete prv_Width;
	if (prv_Height)			delete prv_Height;
}
#pragma endregion