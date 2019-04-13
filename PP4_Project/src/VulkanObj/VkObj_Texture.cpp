#include "VkObj_Texture.h"

bool vk_create_texture_image(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue graphics_queue,
std::vector<Object3D> &object_list, const VkExtent3D &texture_extent, VkSampleCountFlagBits &msaa_sample, VkImage &texture_image, VkDeviceMemory &texture_image_memory)
{
	VkDeviceSize image_size = object_list[0].texture->width * object_list[0].texture->height * sizeof(unsigned int);

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	vk_create_buffer(physical_device, device, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

	std::vector<unsigned int> converted_pixels(object_list[0].texture->width * object_list[0].texture->height);
	for (uint32_t i = 0; i < converted_pixels.size(); ++i)
	{
		Color current_pixel(object_list[0].texture->data[i]);
		Color new_pixel = (current_pixel.a << 24) | (current_pixel.b << 16) | (current_pixel.g << 8) | current_pixel.r;
		converted_pixels[i] = new_pixel.color;
	}

	void* data = nullptr;
	vkMapMemory(device, staging_buffer_memory, 0, image_size, 0, &data);
	memcpy(data, converted_pixels.data(), (unsigned int)image_size);
	vkUnmapMemory(device, staging_buffer_memory);

	vk_create_image(physical_device, device, texture_extent, object_list[0].texture->mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image, texture_image_memory);

	vk_transition_image_layout(device, command_pool, graphics_queue, object_list[0].texture->mip_levels, texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vk_copy_buffer_to_image(device, command_pool, graphics_queue, staging_buffer, texture_image, texture_extent);
	
	vkDestroyBuffer(device, staging_buffer, nullptr);
	vkFreeMemory(device, staging_buffer_memory, nullptr);
	
	vk_create_mipmaps(device, command_pool, graphics_queue, texture_image, object_list[0].texture->width, object_list[0].texture->height, object_list[0].texture->mip_levels);

	return true;
}

bool vk_create_texture_image_view(const VkDevice &device, const VkImage &texture_image, const VkImageAspectFlags &aspect_flag, const uint32_t &mip_level, VkImageView &texture_image_view)
{
	texture_image_view = vk_create_image_view(device,texture_image, VK_FORMAT_R8G8B8A8_UNORM, aspect_flag, mip_level);
	return true;
}

bool vk_create_texture_sampler(const VkDevice &device, VkSampler &sampler)
{
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
	sampler_create_info.minLod = 0.0f;
	sampler_create_info.maxLod = 0.0f;

	VkResult r = vkCreateSampler(device, &sampler_create_info, nullptr, &sampler);
	if (r) {
		LOG("Sampler Has Failed to be Created! Error Code: " << r)
			return false;
	}

	return true;
}

bool vk_create_color_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const uint32_t &mip_levels, 
	const VkSampleCountFlagBits &msaa_sample, const VkExtent3D &swapchain_extent, const VkFormat &swapchain_format, VkImage &color_image, VkDeviceMemory &color_image_memory, VkImageView &color_image_view)
{
	VkFormat color_format = swapchain_format;

	vk_create_image(physical_device, device, swapchain_extent, mip_levels, msaa_sample, color_format, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		color_image, color_image_memory);

	color_image_view = vk_create_image_view(device, color_image, color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	vk_transition_image_layout(device, command_pool, graphics_queue, mip_levels, color_image, color_format,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	return true;
}

bool vk_create_depth_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkExtent3D &swapchain_extent_3d, const VkSampleCountFlagBits &msaa_sample, 
	VkImage &depth_buffer, VkDeviceMemory &depth_buffer_memory, VkImageView &depth_buffer_view)
{
	VkFormat depth_format = vk_get_depth_format(physical_device);

	vk_create_image(physical_device, device, swapchain_extent_3d, 1, msaa_sample, depth_format, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_buffer, depth_buffer_memory);
	depth_buffer_view = vk_create_image_view(device, depth_buffer, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	vk_transition_image_layout(device, command_pool, graphics_queue, 1, depth_buffer, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	return true;
}

bool vk_create_image(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkExtent3D &extent, const uint32_t &mip_levels, const VkSampleCountFlagBits &msaa_sample, const VkFormat &format, const VkImageTiling &tiling, 
	const VkImageUsageFlags &usage_flags, const VkMemoryPropertyFlags &memory_property_flags, VkImage &texture_image, VkDeviceMemory &texture_image_memory)
{
	VkImageCreateInfo image_create_info = {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.extent = extent;
	image_create_info.mipLevels = mip_levels;
	image_create_info.arrayLayers = 1;
	image_create_info.format = format;
	image_create_info.tiling = tiling;
	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_create_info.usage = usage_flags;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_create_info.samples = msaa_sample;
	image_create_info.flags = 0;

	VkResult r = vkCreateImage(device, &image_create_info, nullptr, &texture_image);
	if (r) {
		LOG("Failed to Create Image! Error Code: " << r);
		return false;
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, texture_image, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = vk_find_memory_type(physical_device, memory_requirements.memoryTypeBits, memory_property_flags);

	r = vkAllocateMemory(device, &memory_allocate_info, nullptr, &texture_image_memory);
	if (r) {
		LOG("Failed to Allocate Memory in Create Image! Error Code: " << r);
		return false;
	}

	r = vkBindImageMemory(device, texture_image, texture_image_memory, 0);
	if (r) {
		LOG("Failed to Bind Image Memory! Error Code: " << r);
	}

	return true;
}

bool vk_create_mipmaps(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, 
	const VkImage &texture_image, const uint32_t &texture_width, const uint32_t &texture_height, const uint32_t &mip_levels)
{
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.image = texture_image;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	image_memory_barrier.subresourceRange.layerCount = 1;
	image_memory_barrier.subresourceRange.levelCount = 1;

	int32_t mip_width = texture_width;
	int32_t mip_height = texture_height;
	for (uint32_t i = 1; i < mip_levels; ++i) {
		image_memory_barrier.subresourceRange.baseMipLevel = i - 1;
		image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr, 0, nullptr, 1, &image_memory_barrier);

		VkImageBlit image_blit = {};
		image_blit.srcOffsets[0] = { 0, 0, 0 };
		image_blit.srcOffsets[1] = { mip_width, mip_height, 1 };
		image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit.srcSubresource.mipLevel = i - 1;
		image_blit.srcSubresource.baseArrayLayer = 0;
		image_blit.srcSubresource.layerCount = 1;

		image_blit.dstOffsets[0] = image_blit.srcOffsets[0];
		image_blit.dstOffsets[1] = { mip_width > 1 ? mip_width >> 1 : 1 , mip_height > 1 ? mip_height >> 1 : 1, 1 };
		image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit.dstSubresource.mipLevel = i;
		image_blit.dstSubresource.baseArrayLayer = 0;
		image_blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(command_buffer, texture_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &image_blit, VK_FILTER_LINEAR);

		image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

		if (mip_width > 1) { mip_width >>= 1; }
		if (mip_height > 1) { mip_height >>= 1; }
	}

	image_memory_barrier.subresourceRange.baseMipLevel = mip_levels - 1;
	image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

	if (!vk_end_single_command(device, graphics_queue, command_pool, command_buffer)) {
		LOG("Failed to End Single Command in Mipmapping Texture.");
		return false;
	}

	return true;
}

