#ifndef ASSETS_H
#define ASSETS_H

#include "../Defines.h"
#include "../VulkanObj/VkObj_Context.h"

class Texture
{
public:
	Texture() = default;
	Texture(const char* filename);
	Texture(const int32_t &width, const int32_t &height, void *pixels, const uint32_t &mip_levels, const uint32_t &texture_type);
	~Texture();

	const uint32_t get_width() const { return prv_Width; }
	const uint32_t get_height() const { return prv_Height; }
	const void* get_data() const { return prv_Data; }
	const uint32_t get_mip_levels() const { return prv_MipLevels; }
	const uint32_t get_texture_type() const { return prv_TextureType; }
	const VkImage get_image() const { return prv_Image; }
	const VkImageView get_image_view() const { return prv_ImageView; }
	const VkDeviceMemory get_image_memory() const { return prv_ImageMemory; }
	const VkSampler get_sampler() const { return prv_Sampler; }

	__declspec(property(get = get_width)) uint32_t width;
	__declspec(property(get = get_height)) uint32_t height;
	__declspec(property(get = get_data)) const void *data;
	__declspec(property(get = get_mip_levels)) const uint32_t mip_levels;
	__declspec(property(get = get_texture_type)) uint32_t texture_type;
	__declspec(property(get = get_image)) VkImage image;
	__declspec(property(get = get_image_view)) VkImageView image_view;
	__declspec(property(get = get_image_memory)) VkDeviceMemory image_memory;
	__declspec(property(get = get_sampler)) VkSampler sampler;

	void init();
	void swizzle(const VkComponentSwizzle &r, const VkComponentSwizzle &g, const VkComponentSwizzle &b, const VkComponentSwizzle &a);
	void set_sampler(const VkSamplerCreateInfo &sampler_create_info);
	void cleanup();
private:
	void CreateImage();
	void CreateImageView();
	void CreateDefaultSampler();

	void *prv_Data;
	int32_t prv_Width, prv_Height, prv_MipLevels;
	uint32_t prv_TextureType;
	VkImage prv_Image;
	VkImageView prv_ImageView;
	VkDeviceMemory prv_ImageMemory;
	VkComponentSwizzle prv_R;
	VkComponentSwizzle prv_G;
	VkComponentSwizzle prv_B;
	VkComponentSwizzle prv_A;
	VkSampler prv_Sampler;
};

#endif