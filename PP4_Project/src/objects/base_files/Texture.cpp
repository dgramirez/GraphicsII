#include "pch.h"
#include "Texture.h"
#include "../../VulkanObj/VkObj_Shared.h"
#include "stb_image.h"
#define RAND_NORMAL glm::vec3(rand()/float(RAND_MAX),rand()/float(RAND_MAX),rand()/float(RAND_MAX))

Texture::Texture(const int32_t &width, const int32_t &height, void *pixels, const uint32_t &mip_levels, const uint32_t &texture_type)
	: prv_Width(width), prv_Height(height), prv_Data(pixels), prv_MipLevels(mip_levels), prv_TextureType(texture_type), prv_Image(nullptr), prv_Sampler(nullptr) { }

Texture::Texture(const char* filename)
{
	prv_Data = stbi_load(filename, &prv_Width, &prv_Height, nullptr, 4);
	prv_MipLevels = (int32_t)std::log2(min(prv_Width, prv_Height)) + 1;
	prv_TextureType = TEXTURE_TYPE_STBI;
	prv_Sampler = nullptr;
	prv_Image = nullptr;
}

Texture::~Texture()
{
	cleanup();
}

void Texture::init()
{
	CreateImage();
	if (!prv_Sampler)
		CreateDefaultSampler();
}

void Texture::swizzle(const VkComponentSwizzle &r, const VkComponentSwizzle &g, const VkComponentSwizzle &b, const VkComponentSwizzle &a)
{
	prv_R = r; prv_G = g; prv_B = b; prv_A = a;
}

void Texture::set_sampler(const VkSamplerCreateInfo &sampler_create_info)
{
	vkCreateSampler(myContext.device.logical, &sampler_create_info, nullptr, &prv_Sampler);
}

void Texture::cleanup()
{
	if (prv_Data) {
		switch (prv_TextureType) {
		case TEXTURE_TYPE_STBI:
			stbi_image_free(prv_Data);
			break;
		case TEXTURE_TYPE_HEAP:
			delete[] prv_Data;
			break;
		}
		prv_Data = nullptr;
	}

	if (prv_Sampler)
	{
		vkDestroySampler(myContext.device.logical, prv_Sampler, nullptr);	  
		prv_Sampler = nullptr;
	}

	if (prv_ImageView)
	{
		vkDestroyImageView(myContext.device.logical, prv_ImageView, nullptr);
		prv_ImageView = nullptr;
	}
	if (prv_Image)
	{
		vkDestroyImage(myContext.device.logical, prv_Image, nullptr);
		prv_Image = nullptr;
	}
	if (prv_ImageMemory)
	{
		vkFreeMemory(myContext.device.logical, prv_ImageMemory, nullptr);
		prv_ImageMemory = nullptr;
	}
}

void Texture::CreateImage()
{
	//Get the image size for the texture
	VkDeviceSize image_size = prv_Width * prv_Height * sizeof(unsigned int);

	//Get the staging bugger and memory needed to allocate
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	//Create the staging buffer
	vk_create_buffer(myContext.device.physical, myContext.device.logical, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

	//Allocate the data into the buffer
	void* allocate_data = nullptr;
	vkMapMemory(myContext.device.logical, staging_buffer_memory, 0, image_size, 0, &allocate_data);
	memcpy(allocate_data, prv_Data, (unsigned int)image_size);
	vkUnmapMemory(myContext.device.logical, staging_buffer_memory);

	VkExtent3D extent = { (uint32_t)prv_Width, (uint32_t)prv_Height, 1 };
	//Create the image, using appropriate information (Mip Levels, Texture data, etc.)
	vk_create_image(myContext.device.physical, myContext.device.logical, extent, mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, prv_Image, prv_ImageMemory);

	//Transition, using memory barriers, from Undefined Layout to Transfer to Destination (Optimal)
	vk_transition_image_layout(myContext.device.logical, myContext.command_pool, myContext.device.q_graphics, mip_levels, prv_Image,
		VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	//Copy the buffer to the image
	vk_copy_buffer_to_image(myContext.device.logical, myContext.command_pool, myContext.device.q_graphics, staging_buffer, prv_Image, extent);

	//Destroy memory created from staging buffer
	vkDestroyBuffer(myContext.device.logical, staging_buffer, nullptr);
	vkFreeMemory(myContext.device.logical, staging_buffer_memory, nullptr);

	//Create the mipmaps for texture
	vk_create_mipmaps(myContext.device.logical, myContext.command_pool, myContext.device.q_graphics, prv_Image, prv_Width, prv_Height, mip_levels);

	//Create Image View
	CreateImageView();
}

void Texture::CreateImageView()
{
	//Image View Create Info
	VkImageViewCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = prv_Image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
	create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	create_info.subresourceRange.baseMipLevel = 0;
	create_info.subresourceRange.levelCount = mip_levels;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;
	create_info.components.r = prv_R;
	create_info.components.g = prv_G;
	create_info.components.b = prv_B;
	create_info.components.a = prv_A;

	//Create the Surface (With Results) [VK_SUCCESS = 0]
	vkCreateImageView(myContext.device.logical, &create_info, nullptr, &prv_ImageView), "Failed to Create Image View";
}

void Texture::CreateDefaultSampler()
{
	//Create the sampler create info
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.anisotropyEnable = VK_TRUE;
	sampler_create_info.maxAnisotropy = (float)myContext.device.msaa_support;
	sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_create_info.unnormalizedCoordinates = VK_FALSE;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.minLod = 0.0f;
	sampler_create_info.maxLod = (float)mip_levels;

	vkCreateSampler(myContext.device.logical, &sampler_create_info, nullptr, &prv_Sampler);
}
