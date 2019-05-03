#ifndef VKOBJ_SHARED_H
#define VKOBJ_SHARED_H

#include "../Defines.h"
#include "../Object.h"

struct VkStruct_QueueFamilyIndices {
	std::optional<uint32_t> graphics;		//Graphics Queue
	std::optional<uint32_t> present;		//Present Queue

	bool IsComplete() { return graphics.has_value() && present.has_value(); }
};

struct VkStruct_SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct vkStruct_Buffer {
	VkBuffer me;
	VkDeviceMemory memory;
};

struct vkStruct_Images {
	VkImage me;
	VkImageView view;
	VkDeviceMemory memory;
};

VkStruct_QueueFamilyIndices vk_find_queue_family(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
VkStruct_SwapchainSupportDetails vk_query_swapchain_support(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
VkImageView vk_create_image_view(const VkDevice &device, const VkImage &image, const VkFormat &format, const VkImageAspectFlags &image_aspect_flags, const uint32_t &mip_level);
uint32_t vk_find_memory_type(const VkPhysicalDevice &physical_device, const uint32_t &filter, const VkMemoryPropertyFlags &property_flags);

VkCommandBuffer vk_start_single_command(const VkDevice &device, const VkCommandPool &command_pool);
void vk_transition_image_layout(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const uint32_t &mip_level,
 const VkImage &image, const VkFormat &format, const VkImageLayout &previous_layout, const VkImageLayout &current_layout);
bool vk_end_single_command(const VkDevice &device, const VkQueue &graphics_queue, const VkCommandPool &command_pool, VkCommandBuffer &command_buffer);

bool vk_contains_stencil_format(VkFormat format);

bool vk_create_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkDeviceSize &size, const VkBufferUsageFlags &usage_flags, const VkMemoryPropertyFlags &property_flags, VkBuffer &buffer, VkDeviceMemory &buffer_memory);
void vk_copy_buffer(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkBuffer &source_buffer, const VkBuffer &destination_buffer, const VkDeviceSize &device_size);
void vk_copy_buffer_to_image(const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue, const VkBuffer &buffer, const VkImage &image, const VkExtent3D &extent);

VkFormat vk_get_depth_format(const VkPhysicalDevice &physical_device);
VkFormat vk_find_supported_formats(const VkPhysicalDevice &physical_device, const std::vector<VkFormat> &formats, const VkImageTiling &tiling, const VkFormatFeatureFlags &format_features_flags);
VkMemoryPropertyFlags vk_find_memory_type_index(const VkPhysicalDevice &physical_device, const uint64_t &memory_type_bits, const uint64_t &usage);

bool vk_check_page(const VkDeviceSize &a_memory_offset, const VkDeviceSize &b_memory_offset, const VkDeviceSize &a_size, const VkDeviceSize &buffer_image_granularity, const uint64_t &operation);
bool vk_granularity_conflict(const uint64_t &type_1, const uint64_t &type_2);

VkSampleCountFlagBits get_highest_msaa_sample_count(const VkPhysicalDevice &physical_device);

#endif