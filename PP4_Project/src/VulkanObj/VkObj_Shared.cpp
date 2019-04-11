#include "VkObj_Shared.h"

SwapChainSupportDetails vk_query_swapchain_support(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface)
{
	//Set the Surface Capabilities for Swapchain
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);

	//Set the format(s) and Present Mode(s) for Swapchain
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);

	if (format_count)
	{
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details.formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);

	if (present_mode_count)
	{
		details.presentModes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details.presentModes.data());
	}

	//Return the details found
	return details;
}

QueueFamilyIndices vk_find_queue_family(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface)
{
	//Initialize Return Value
	QueueFamilyIndices indices;

	//Gather Queue Family Count
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
	std::vector<VkQueueFamilyProperties>queue_family_property_list(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_property_list.data());

	//Set the Graphics and Present Family
	for (unsigned int i = 0; i < queue_family_count; ++i)
	{
		if (queue_family_property_list[i].queueCount > 0 &&
			queue_family_property_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphics = i;

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
			if (queue_family_property_list[i].queueCount > 0 && present_support) {
				indices.present = i;
			}
		}

		if (indices.IsComplete())
			break;
	}

	//Return Indices
	return indices;
}

VkImageView vk_create_image_view(const VkDevice &device, const VkImage &image, const VkFormat &format, const VkImageAspectFlags &image_aspect_flags, const uint32_t &base_mip_level)
{
	//Image View Create Info
	VkImageViewCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = format;
	create_info.subresourceRange.aspectMask = image_aspect_flags;
	create_info.subresourceRange.baseMipLevel = base_mip_level;
	create_info.subresourceRange.levelCount = 1;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;

	
	//Create the Surface (With Results) [VK_SUCCESS = 0]
	VkImageView image_view;
	VkResult r = vkCreateImageView(device, &create_info, nullptr, &image_view);
	if (r) {
		LOG("Failed to Create Image View");
		return false;
	}

	//Image View has been created successfully, return it
	return image_view;
}

uint32_t vk_find_memory_type(const VkPhysicalDevice &physical_device, const uint32_t &filter, const VkMemoryPropertyFlags &property_flags)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if ((filter & (1 << i)) &&
			(memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
			return i;
	}

	LOG("FAILED TO GET MEMORY TYPE!");

	return -1;
}

VkCommandBuffer vk_start_single_command(const VkDevice &device, const VkCommandPool &command_pool)
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool = command_pool;
	command_buffer_allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	VkResult r = vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer);
	if (r) {
		LOG("Failed to Allocate Command Buffer! Error Code: " << r);
	}

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	r = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
	if (r) {
		LOG("Failed to Begin Command Buffer! Error Code: " << r)
	}

	return command_buffer;
}

void vk_end_single_command(const VkDevice &device, const VkQueue &graphics_queue, const VkCommandPool &command_pool, VkCommandBuffer &command_buffer)
{
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics_queue);

	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void vk_transition_image_layout(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, 
	const VkImage &image, const VkFormat &format, const VkImageLayout &previous_layout, const VkImageLayout &current_layout)
{
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.image = image;
	image_memory_barrier.oldLayout = previous_layout;
	image_memory_barrier.newLayout = current_layout;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.layerCount = 1;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;

	VkPipelineStageFlags source_stage;
	VkPipelineStageFlags destrination_stage;

	if (current_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (vk_contains_stencil_format(format))
			image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	if (previous_layout == VK_IMAGE_LAYOUT_UNDEFINED)
	{
		if (current_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (current_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
	}
	else if (previous_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && current_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destrination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		LOG("No Transition Image Layout Avialble! May Cause Problems!")
	}

	vkCmdPipelineBarrier(command_buffer, source_stage, destrination_stage, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

	vk_end_single_command(device, graphics_queue, command_pool, command_buffer);
}

bool vk_contains_stencil_format(VkFormat format) {
	return format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}

bool vk_create_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkDeviceSize &size, const VkBufferUsageFlags &usage_flags, const VkMemoryPropertyFlags &property_flags, VkBuffer &buffer, VkDeviceMemory &buffer_memory)
{
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;
	buffer_create_info.usage = usage_flags;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult r = vkCreateBuffer(device, &buffer_create_info, nullptr, &buffer);
	if (r) {
		LOG("Failed to create the vertex buffer! Error Code: " << r)
		return false;
	}

	VkMemoryRequirements memory_requirement;
	vkGetBufferMemoryRequirements(device, buffer, &memory_requirement);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirement.size;
	memory_allocate_info.memoryTypeIndex = vk_find_memory_type(physical_device, memory_requirement.memoryTypeBits, property_flags);

	r = vkAllocateMemory(device, &memory_allocate_info, nullptr, &buffer_memory);
	if (r) {
		LOG("FAILED TO ALLOCATE TO MEMORY! Error Code: " << r)
		return false;
	}

	r = vkBindBufferMemory(device, buffer, buffer_memory, 0);
	if (r) {
		LOG("Failed to Bind Buffer Memory! Error Code: " << r)
		return false;
	}

	return true;
}

void vk_copy_buffer(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkBuffer &source_buffer, const VkBuffer &destination_buffer, const VkDeviceSize &device_size)
{
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	VkBufferCopy copy_buffer = {};
	copy_buffer.srcOffset = 0;
	copy_buffer.dstOffset = 0;
	copy_buffer.size = device_size;
	vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &copy_buffer);

	vk_end_single_command(device, graphics_queue, command_pool, command_buffer);
}

void vk_copy_buffer_to_image(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkBuffer &buffer, const VkImage &image, const VkExtent3D &extent)
{
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	VkBufferImageCopy buffer_image_copy = {};
	buffer_image_copy.bufferOffset = 0;
	buffer_image_copy.bufferRowLength = 0;
	buffer_image_copy.bufferImageHeight = 0;
	buffer_image_copy.imageSubresource.layerCount = 1;
	buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	buffer_image_copy.imageSubresource.mipLevel = 0;
	buffer_image_copy.imageSubresource.baseArrayLayer = 0;
	buffer_image_copy.imageOffset = { 0,0,0 };
	buffer_image_copy.imageExtent = extent;

	vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);

	vk_end_single_command(device, graphics_queue, command_pool, command_buffer);
}

VkFormat vk_get_depth_format(const VkPhysicalDevice &physical_device)
{
	std::vector<VkFormat> formats{
	VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
	};

	return vk_find_supported_formats(physical_device, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat vk_find_supported_formats(const VkPhysicalDevice &physical_device, const std::vector<VkFormat> &formats, const VkImageTiling &tiling, const VkFormatFeatureFlags &format_features_flags)
{
	for (uint32_t i = 0; i < formats.size(); ++i)
	{
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, formats[i], &format_properties);

		if (tiling == VK_IMAGE_TILING_LINEAR &&
			(format_properties.linearTilingFeatures & format_features_flags) == format_features_flags)
		{
			return formats[i];
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
			(format_properties.optimalTilingFeatures & format_features_flags) == format_features_flags)
		{
			return formats[i];
		}
		else
		{
			LOG("Failed to find a supported format! Sending back VK_FORMAT_UNDEFINED");
		}
	}

	return VK_FORMAT_UNDEFINED;
}
