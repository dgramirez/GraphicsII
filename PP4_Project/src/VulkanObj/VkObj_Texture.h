#ifndef VKOBJ_TEXTUREOBJECTS_H
#define VKOBJ_TEXTUREOBJECTS_H

#include "VkObj_Shared.h"
#include "../Object.h"

struct VkObj_Texture {
	VkImage texture_image;
	VkDeviceMemory texture_image_memory;
	VkImageView texture_image_view;
	VkSampler sampler;

	VkImage color_image;
	VkDeviceMemory color_image_memory;
	VkImageView color_image_view;

	VkImage depth_buffer;
	VkDeviceMemory depth_buffer_memory;
	VkImageView depth_buffer_view;

	VkSampleCountFlagBits msaa_sample;
};

bool vk_create_texture_image(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue graphics_queue,
	std::vector<Object3D> &object_list, const VkExtent3D &texture_extent, VkSampleCountFlagBits &msaa_sample, VkImage &texture_image, VkDeviceMemory &texture_image_memory);
bool vk_create_texture_image_view(const VkDevice &device, const VkImage &texture_image, const VkImageAspectFlags &aspect_flag, const uint32_t &mip_level, VkImageView &texture_image_view);
bool vk_create_texture_sampler(const VkDevice &device, VkSampler &sampler);

bool vk_create_color_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const uint32_t &mip_levels,
	const VkSampleCountFlagBits &msaa_sample, const VkExtent3D &swapchain_extent, const VkFormat &swapchain_format, VkImage &color_image, VkDeviceMemory &color_image_memory, VkImageView &color_image_view);
bool vk_create_depth_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkExtent3D &swapchain_extent_3d, const VkSampleCountFlagBits &msaa_sample,
	VkImage &depth_buffer, VkDeviceMemory &depth_buffer_memory, VkImageView &depth_buffer_view);

#endif