#include "pch.h"
#include "VkObj_Shared.h"

VkStruct_SwapchainSupportDetails vk_query_swapchain_support(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface)
{
	//Set the Surface Capabilities for Swapchain
	VkStruct_SwapchainSupportDetails details;
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

VkStruct_QueueFamilyIndices vk_find_queue_family(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface)
{
	//Initialize Return Value
	VkStruct_QueueFamilyIndices indices;

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

VkImageView vk_create_image_view(const VkDevice &device, const VkImage &image, const VkFormat &format, const VkImageAspectFlags &image_aspect_flags, const uint32_t &mip_level)
{
	//Image View Create Info
	VkImageViewCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = format;
	create_info.subresourceRange.aspectMask = image_aspect_flags;
	create_info.subresourceRange.baseMipLevel = 0;
	create_info.subresourceRange.levelCount = mip_level;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;

	
	//Create the Surface (With Results) [VK_SUCCESS = 0]
	VkImageView image_view;
	CHECK_VKRESULT(r, vkCreateImageView(device, &create_info, nullptr, &image_view), "Failed to Create Image View");

	//Image View has been created successfully, return it
	return image_view;
}

uint32_t vk_find_memory_type(const VkPhysicalDevice &physical_device, const uint32_t &filter, const VkMemoryPropertyFlags &property_flags)
{
	//Get the Memory Properties from the Physical Device
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	//Loop through the memory type count and see if there is a match with both the filter and property flags
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
	//Command Buffer Allocate Info
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool = command_pool;
	command_buffer_allocate_info.commandBufferCount = 1;

	//Create the command buffer and allocate the command buffer with create info
	VkCommandBuffer command_buffer;
	CHECK_VKRESULT_NO_RETURN(r, vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer), "Failed to Allocate Command Buffer!");

	//Start the command buffer's create info
	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//Begin the Command Buffer's recording process
	CHECK_VKRESULT_NO_RETURN(r2, vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info), "Failed to Begin Command Buffer!");

	//Command Buffer has been successfully started!
	return command_buffer;
}

bool vk_end_single_command(const VkDevice &device, const VkQueue &graphics_queue, const VkCommandPool &command_pool, VkCommandBuffer &command_buffer)
{
	//End the Command Buffer's recording Process
	CHECK_VKRESULT(r, vkEndCommandBuffer(command_buffer), "Failed to End Command Buffer!");

	//Create the submit info
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	//Submit The Commands Recorded into the Queue. Then wait for the Graphics Queue to be idle
	CHECK_VKRESULT(s, vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE), "Failed to Submit to Graphics Queue While Ending Command Buffer!");
	CHECK_VKRESULT(g, vkQueueWaitIdle(graphics_queue), "Failed to Idle Queue While Ending Command Buffer!");

	//Free the command buffer from memory
	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);

	//The Command Buffer has ended successfully!
	return true;
}

void vk_transition_image_layout(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const uint32_t &mip_level,
	const VkImage &image, const VkFormat &format, const VkImageLayout &previous_layout, const VkImageLayout &current_layout)
{
	//Start the command buffer
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	//Create the image memory barrier
	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.image = image;
	image_memory_barrier.oldLayout = previous_layout;
	image_memory_barrier.newLayout = current_layout;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = mip_level;
	image_memory_barrier.subresourceRange.layerCount = 1;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;

	//Setup the source and destination stage flags. Will be set based on the Old and New Layout set from outside
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
		else if (current_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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
	return format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT; //Stencil Format
}

bool vk_create_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkDeviceSize &size, const VkBufferUsageFlags &usage_flags, const VkMemoryPropertyFlags &property_flags, VkBuffer &buffer, VkDeviceMemory &buffer_memory)
{
	//Create Buffer Create Info
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;
	buffer_create_info.usage = usage_flags;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Create the buffer
	CHECK_VKRESULT(r, vkCreateBuffer(device, &buffer_create_info, nullptr, &buffer), "Failed to create the vertex buffer!");

	//Get the memory required to allocate the buffer
	VkMemoryRequirements memory_requirement;
	vkGetBufferMemoryRequirements(device, buffer, &memory_requirement);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirement.size;
	memory_allocate_info.memoryTypeIndex = vk_find_memory_type(physical_device, memory_requirement.memoryTypeBits, property_flags);

	//Allocate and Bind the buffer
	CHECK_VKRESULT(a, vkAllocateMemory(device, &memory_allocate_info, nullptr, &buffer_memory), "FAILED TO ALLOCATE TO MEMORY!");
	CHECK_VKRESULT(b, vkBindBufferMemory(device, buffer, buffer_memory, 0), "Failed to Bind Buffer Memory!");

	//The buffer has been successfully created!
	return true;
}

void vk_copy_buffer(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkBuffer &source_buffer, const VkBuffer &destination_buffer, const VkDeviceSize &device_size)
{
	//Start the command buffer
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	//Copy the buffer sent through
	VkBufferCopy copy_buffer = {};
	copy_buffer.srcOffset = 0;
	copy_buffer.dstOffset = 0;
	copy_buffer.size = device_size;
	vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &copy_buffer);

	//End the command buffer
	vk_end_single_command(device, graphics_queue, command_pool, command_buffer);
}

void vk_copy_buffer_to_image(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkBuffer &buffer, const VkImage &image, const VkExtent3D &extent)
{
	//Start Command Buffer
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	//Setup The Buffer Image Copy
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

	//Send Command to Copy Buffer to Image
	vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);

	//End the Command buffer
	vk_end_single_command(device, graphics_queue, command_pool, command_buffer);
}

VkFormat vk_get_depth_format(const VkPhysicalDevice &physical_device)
{
	//Set the Depth View Formats for Depth View Support
	std::vector<VkFormat> formats{
	VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
	};

	//Return if the format found was supported
	return vk_find_supported_formats(physical_device, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat vk_find_supported_formats(const VkPhysicalDevice &physical_device, const std::vector<VkFormat> &formats, const VkImageTiling &tiling, const VkFormatFeatureFlags &format_features_flags)
{
	//Loop through all the device formats and find the proper format requested
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
	}

	//Failed to find a format! Send Undefined
	LOG("Failed to find a supported format! Sending back VK_FORMAT_UNDEFINED");
	return VK_FORMAT_UNDEFINED;
}

VkMemoryPropertyFlags vk_find_memory_type_index(const VkPhysicalDevice &physical_device, const uint64_t &memory_type_bits, const uint64_t &usage)
{
	VkPhysicalDeviceMemoryProperties pdevice_memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &pdevice_memory_properties);

	//Set the Required and Preferred flags
	VkMemoryPropertyFlags preferred = 0;
	VkMemoryPropertyFlags required = 0;

	if (usage == VKDEFINE_MEMORY_USAGE_GPU_ONLY)
		preferred = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	else if (usage == VKDEFINE_MEMORY_USAGE_GPU_TO_CPU)
	{
		required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	}
	else if (usage == VKDEFINE_MEMORY_USAGE_CPU_ONLY)
		required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	else if (usage == VKDEFINE_MEMORY_USAGE_CPU_TO_GPU)
	{
		required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT| VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	}

	//Search for both required and preferred
	for (uint32_t i = 0; i < pdevice_memory_properties.memoryTypeCount; ++i)
	{
		if (((memory_type_bits >> i) & 1) == 0)
			continue;

		VkMemoryPropertyFlags memory_property_flags = pdevice_memory_properties.memoryTypes[i].propertyFlags;
		if ((memory_property_flags & required) != required ||
			(memory_property_flags & preferred) != preferred)
			continue;

		return i;
	}

	//Search only for required
	for (uint32_t i = 0; i < pdevice_memory_properties.memoryTypeCount; ++i)
	{
		if (((memory_type_bits >> i) & 1) == 0)
			continue;

		VkMemoryPropertyFlags memory_property_flags = pdevice_memory_properties.memoryTypes[i].propertyFlags;
		if ((memory_property_flags & required) != required)
			continue;

		return i;
	}

	return MAX_UINT32;
}

bool vk_check_page(const VkDeviceSize &a_memory_offset, const VkDeviceSize &b_memory_offset, const VkDeviceSize &a_size, const VkDeviceSize &buffer_image_granularity, const uint64_t &operation)
{
	//In Vulkan 1.0.107 Specification, Underneath the "vkBindImageMemory" contains information about Buffer-Image Granularity.
	//This is based on the function provided
	if ((a_memory_offset + a_size <= b_memory_offset) && a_size > 0 && buffer_image_granularity > 0) {
		LOG("Check Resource Page: There is a good possibilty that A overlaps B, and thats not good.");
	}

	VkDeviceSize big_negate = ~(buffer_image_granularity - 1);
	VkDeviceSize a_end = a_memory_offset + a_size - 1;
	VkDeviceSize a_end_page = a_end & big_negate;
	VkDeviceSize b_start = b_memory_offset;
	VkDeviceSize b_start_page = b_start & big_negate;

	if (operation  == VKDEFINE_OPERATION_TYPE_LESS)
		return a_end_page < b_start_page;
	else if (operation == VKDEFINE_OPERATION_TYPE_EQUAL)
		return a_end_page == b_start_page;
	else if (operation == VKDEFINE_OPERATION_TYPE_GREATER)
		return a_end_page > b_start_page;
	else return false;
}

bool vk_granularity_conflict(const uint64_t &type_1, const uint64_t &type_2)
{
	uint64_t t1 = max(type_1, type_2);
	uint64_t t2 = min(type_1, type_2);

	if (t1 == VKDEFINE_ALLOCATION_TYPE_FREE)
		return false;
	if (t1 == VKDEFINE_ALLOCATION_TYPE_BUFFER)
		return (t2 == VKDEFINE_ALLOCATION_TYPE_IMAGE || t2 == VKDEFINE_ALLOCATION_TYPE_IMAGE_OPTIMAL);
	if (t1 == VKDEFINE_ALLOCATION_TYPE_IMAGE)
		return (t2 == VKDEFINE_ALLOCATION_TYPE_IMAGE || t2 == VKDEFINE_ALLOCATION_TYPE_IMAGE_LINEAR || t2 == VKDEFINE_ALLOCATION_TYPE_IMAGE_OPTIMAL);
	if (t1 == VKDEFINE_ALLOCATION_TYPE_IMAGE_LINEAR)
		return (t2 == VKDEFINE_ALLOCATION_TYPE_IMAGE_OPTIMAL);
	if (t1 == VKDEFINE_ALLOCATION_TYPE_IMAGE_OPTIMAL)
		return false;

	return true;
}

VkSampleCountFlagBits get_highest_msaa_sample_count(const VkPhysicalDevice &physical_device)
{
	VkPhysicalDeviceProperties physical_device_properties;
	vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

	VkSampleCountFlags flags = min(physical_device_properties.limits.framebufferColorSampleCounts,
		physical_device_properties.limits.framebufferDepthSampleCounts);

	if (flags & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (flags & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (flags & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (flags & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (flags & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (flags & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

bool vk_create_image(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkExtent3D &extent, const uint32_t &mip_levels, const VkSampleCountFlagBits &msaa_sample, const VkFormat &format, const VkImageTiling &tiling,
	const VkImageUsageFlags &usage_flags, const VkMemoryPropertyFlags &memory_property_flags, VkImage &texture_image, VkDeviceMemory &texture_image_memory)
{
	//Create image info
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

	//Create the image
	CHECK_VKRESULT(r1, vkCreateImage(device, &image_create_info, nullptr, &texture_image), "Failed to Create Image!");

	//Create the memory required for any the image passed into this function
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, texture_image, &memory_requirements);

	//Memory Allocate Info
	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = vk_find_memory_type(physical_device, memory_requirements.memoryTypeBits, memory_property_flags);

	//Allocate the memory created
	CHECK_VKRESULT(r2, vkAllocateMemory(device, &memory_allocate_info, nullptr, &texture_image_memory), "Failed to Allocate Memory in Create Image!");

	//Bind the memory created
	CHECK_VKRESULT(r3, vkBindImageMemory(device, texture_image, texture_image_memory, 0), "Failed to Bind Image Memory! Error Code: ");

	//Image Creation has been successful!
	return true;
}

bool vk_create_mipmaps(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue,
	const VkImage &texture_image, const uint32_t &texture_width, const uint32_t &texture_height, const uint32_t &mip_levels)
{
	//Start the command buffer
	VkCommandBuffer command_buffer = vk_start_single_command(device, command_pool);

	//Create the Image Memory Barrier for Mipmapping
	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.image = texture_image;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	image_memory_barrier.subresourceRange.layerCount = 1;
	image_memory_barrier.subresourceRange.levelCount = 1;

	//Save the texture width and height for Mip levels
	int32_t mip_width = texture_width;
	int32_t mip_height = texture_height;

	//Loop for every Mip levels. NOTE: (i-1) is the current mip level, while (i) is the next mip level
	for (uint32_t i = 1; i < mip_levels; ++i) {
		//Setup the current mip level for blitting the image
		image_memory_barrier.subresourceRange.baseMipLevel = i - 1;
		image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		//Transfer the layout and Access Mask Information
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr, 0, nullptr, 1, &image_memory_barrier);

		//Create the Blit Image. Src is (i-1), or current mip level. dst is (i), or next mip level.
		VkImageBlit image_blit = {};
		image_blit.srcSubresource.mipLevel = i - 1;
		image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit.srcSubresource.baseArrayLayer = 0;
		image_blit.srcSubresource.layerCount = 1;
		image_blit.srcOffsets[0] = { 0, 0, 0 };
		image_blit.srcOffsets[1] = { mip_width, mip_height, 1 };

		image_blit.dstSubresource.mipLevel = i;
		image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit.dstSubresource.baseArrayLayer = 0;
		image_blit.dstSubresource.layerCount = 1;
		image_blit.dstOffsets[0] = image_blit.srcOffsets[0];
		image_blit.dstOffsets[1] = { mip_width > 1 ? (mip_width >> 1) : 1 , mip_height > 1 ? (mip_height >> 1) : 1, 1 };

		//Blit the texture
		vkCmdBlitImage(command_buffer, texture_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &image_blit, VK_FILTER_LINEAR);

		//Set the layout and Access Mask (again) for the shader to read
		image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//Transfer the layout and Access Mask Information (Again, based on above values)
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

		//Reduce the Mip level down by 1 level [By cutting width and height in half]
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

	//End the command
	CHECK_RESULT(vk_end_single_command(device, graphics_queue, command_pool, command_buffer), "Failed to End Single Command in Mipmapping Texture.");

	//Mipmapping has been created successfully!
	return true;
}

bool vk_sync_semaphore_and_fences(const VkDevice &device, std::vector<VkSemaphore> &image_available_semaphore, std::vector<VkSemaphore> &render_finished_semaphore, std::vector<VkFence> &fences)
{
	//Resize based on the max frames (Double Buffer)
	image_available_semaphore.resize(MAX_FRAMES);
	render_finished_semaphore.resize(MAX_FRAMES);
	fences.resize(MAX_FRAMES);

	//Semaphore Info Create
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Fence Info Create
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Create all the Semaphores and Fences needed
	for (unsigned int i = 0; i < MAX_FRAMES; ++i)
	{
		CHECK_VKRESULT(ias, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphore[i]), "Semaphore Creation for Image Available Semaphore has Failed at index " << i << "!");
		CHECK_VKRESULT(rfs, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphore[i]), "Semaphore Creation for Render Finished Semaphore has Failed at index " << i << "!");
		CHECK_VKRESULT(f, vkCreateFence(device, &fence_create_info, nullptr, &fences[i]), "Fence Creation has Failed at index " << i << "!");
	}

	//Semaphores and Fences has been created successfully!
	return true;
}

bool vk_create_semaphore(const VkDevice &device, std::vector<VkSemaphore> &image_available_semaphore, std::vector<VkSemaphore> &render_finished_semaphore)
{
	//Resize based on the max frames (Double Buffer)
	image_available_semaphore.resize(MAX_FRAMES);
	render_finished_semaphore.resize(MAX_FRAMES);

	//Semaphore Info Create
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Create all the Semaphores and Fences needed
	for (unsigned int i = 0; i < MAX_FRAMES; ++i)
	{
		CHECK_VKRESULT(ias, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphore[i]), "Semaphore Creation for Image Available Semaphore has Failed at index " << i << "!");
		CHECK_VKRESULT(rfs, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphore[i]), "Semaphore Creation for Render Finished Semaphore has Failed at index " << i << "!");
	}

	return true;
}
