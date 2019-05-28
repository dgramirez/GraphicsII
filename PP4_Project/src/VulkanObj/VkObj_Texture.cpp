#include "VkObj_Texture.h"

bool vk_create_texture_image(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue graphics_queue,
std::vector<Object3D> &object_list, const VkExtent3D &texture_extent, VkSampleCountFlagBits &msaa_sample, VkImage &texture_image, VkDeviceMemory &texture_image_memory)
{
// 	//Get the image size for the texture
// 	VkDeviceSize image_size = object_list[0].texture->width * object_list[0].texture->height * sizeof(unsigned int);
// 
// 	//Get the staging bugger and memory needed to allocate
// 	VkBuffer staging_buffer;
// 	VkDeviceMemory staging_buffer_memory;
// 
// 	//Create the buffer needed for the texture
// 	vk_create_buffer(physical_device, device, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
// 		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
// 
// // 	Change the Color Format from ARGB to ABGR [Reads from Left to Right, TGA is BGRA while Vulkan is SRGB]
// 	std::vector<unsigned int> converted_pixels(object_list[0].texture->width * object_list[0].texture->height);
// 	for (uint32_t i = 0; i < converted_pixels.size(); ++i)
// 	{
// 		Color current_pixel(object_list[0].texture->data_h[i]);
// 		Color new_pixel = (current_pixel.a << 24) | (current_pixel.b << 16) | (current_pixel.g << 8) | current_pixel.r;
// 		converted_pixels[i] = new_pixel.color;
// 	}
// 
// 	//Allocate the data into the buffer
// 	void* data = nullptr;
// 	vkMapMemory(device, staging_buffer_memory, 0, image_size, 0, &data);
// 	memcpy(data, converted_pixels.data(), (unsigned int)image_size);
// 	vkUnmapMemory(device, staging_buffer_memory);
// 
// 	//Create the image, using appropriate information (Mip Levels, Texture data, etc.)
// 	vk_create_image(physical_device, device, texture_extent, object_list[0].texture->mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
// 		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image, texture_image_memory);
// 
// 	//Transition, using memory barriers, from Undefined Layout to Transfer to Destination (Optimal)
// 	vk_transition_image_layout(device, command_pool, graphics_queue, object_list[0].texture->mip_levels, texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
// 	
// 	//Copy the buffer to the image
// 	vk_copy_buffer_to_image(device, command_pool, graphics_queue, staging_buffer, texture_image, texture_extent);
// 	
// 	//Destroy memory created from staging buffer
// 	vkDestroyBuffer(device, staging_buffer, nullptr);
// 	vkFreeMemory(device, staging_buffer_memory, nullptr);
// 	
// 	//Create the mipmaps for texture
// 	vk_create_mipmaps(device, command_pool, graphics_queue, texture_image, object_list[0].texture->width, object_list[0].texture->height, object_list[0].texture->mip_levels);
// 
// 	//Texture Image was created successfuly!
 	return true;
}

bool vk_create_texture_image_view(const VkDevice &device, const VkImage &texture_image, const VkImageAspectFlags &aspect_flag, const uint32_t &mip_level, VkImageView &texture_image_view)
{
	//Create the texture view
	texture_image_view = vk_create_image_view(device,texture_image, VK_FORMAT_R8G8B8A8_UNORM, aspect_flag, mip_level);

	//Texture image view was created successfully!
	return true;
}

bool vk_create_texture_sampler(const VkDevice &device, VkSampler &sampler)
{
	//Create the sampler create info
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.anisotropyEnable = false;
	sampler_create_info.maxAnisotropy = 1;
	sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_create_info.unnormalizedCoordinates = false;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.minLod = 5.0f;
	sampler_create_info.maxLod = 10.0f;

	//Create the Sampler (Using Validation)
	CHECK_VKRESULT(r, vkCreateSampler(device, &sampler_create_info, nullptr, &sampler), "Sampler Has Failed to be Created!");

	//Sampler was created successfully!
	return true;
}

bool vk_create_color_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const uint32_t &mip_levels, 
	const VkSampleCountFlagBits &msaa_sample, const VkExtent3D &swapchain_extent, const VkFormat &swapchain_format, VkImage &color_image, VkDeviceMemory &color_image_memory, VkImageView &color_image_view)
{
	//Get the color format based on the swapchain format
	VkFormat color_format = swapchain_format;

	//Create the image and image view for MSAA
	vk_create_image(physical_device, device, swapchain_extent, mip_levels, msaa_sample, color_format, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		color_image, color_image_memory);
	color_image_view = vk_create_image_view(device, color_image, color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	//Transition the image layout from Undefined to Color Attachment (Optimal)
	vk_transition_image_layout(device, command_pool, graphics_queue, mip_levels, color_image, color_format,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	return true;
}

bool vk_create_depth_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkExtent3D &swapchain_extent_3d, 
	const VkSampleCountFlagBits &msaa_sample, VkImage &depth_buffer, VkDeviceMemory &depth_buffer_memory, VkImageView &depth_buffer_view)
{
	//Create the format for depth buffer
	VkFormat depth_format = vk_get_depth_format(physical_device);

	//Create the image and view for the depth buffer
	vk_create_image(physical_device, device, swapchain_extent_3d, 1, msaa_sample, depth_format, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_buffer, depth_buffer_memory);
	depth_buffer_view = vk_create_image_view(device, depth_buffer, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	//Transition image layout from Undefined to Depth Stencil Attachment (Optimal)
	vk_transition_image_layout(device, command_pool, graphics_queue, 1, depth_buffer, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	return true;
}


