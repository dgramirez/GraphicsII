#ifndef VKOBJ_TEXTUREOBJECTS_H
#define VKOBJ_TEXTUREOBJECTS_H

#include "VkObj_Shared.h"
#include "../Object.h"

bool vk_create_color_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const uint32_t &mip_levels,
	const VkSampleCountFlagBits &msaa_sample, const VkExtent3D &swapchain_extent, const VkFormat &swapchain_format, VkImage &color_image, VkDeviceMemory &color_image_memory, VkImageView &color_image_view);
bool vk_create_depth_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkExtent3D &swapchain_extent_3d, const VkSampleCountFlagBits &msaa_sample,
	VkImage &depth_buffer, VkDeviceMemory &depth_buffer_memory, VkImageView &depth_buffer_view);

#endif