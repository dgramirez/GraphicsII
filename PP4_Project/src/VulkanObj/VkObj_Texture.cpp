#include "VkObj_Texture.h"

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


