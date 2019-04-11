#ifndef VKOBJ_TEXTUREOBJECTS_H
#define VKOBJ_TEXTUREOBJECTS_H

#include "VkObj_Shared.h"

struct VkObj_Texture {
	VkImage image;
	VkDeviceMemory image_memory;
	VkImageView image_view;
	VkSampler sampler;

	VkImage depth_buffer;
	VkDeviceMemory depth_buffer_memory;
	VkImageView depth_buffer_view;
};

bool vk_create_texture_image(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue graphics_queue,
	std::vector<Object3D> &object_list, const VkExtent3D &texture_extent, VkImage &texture_image, VkDeviceMemory &texture_image_memory);
bool vk_create_texture_image_view(const VkDevice &device, const VkImage &texture_image, const VkImageAspectFlags &aspect_flag, const uint32_t &mip_level, VkImageView &texture_image_view);
bool vk_create_texture_sampler(const VkDevice &device, VkSampler &sampler);

bool vk_create_depth_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkExtent3D &swapchain_extent_3d, VkImage &depth_buffer, VkDeviceMemory &depth_buffer_memory, VkImageView &depth_buffer_view);


bool vk_create_image(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkExtent3D &extent, const uint32_t &mip_levels, const VkFormat &format, const VkImageTiling &tiling,
	const VkImageUsageFlags &usage_flags, const VkMemoryPropertyFlags &memory_property_flags, VkImage &image, VkDeviceMemory &image_memory);

#endif