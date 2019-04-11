#ifndef VKOBJ_BUFFERS_H
#define VKOBJ_BUFFERS_H

#include "VkObj_Shared.h"

struct VkObj_Buffers
{
	VkBuffer vertex;
	VkDeviceMemory vertex_memory;

	VkBuffer index;
	VkDeviceMemory index_memory;

	std::vector<VkBuffer> uniform;
	std::vector<VkDeviceMemory> uniform_memory;

	VkDescriptorSetLayout descriptor_set_layout;
	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;
};

bool vk_create_vertex_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue,
	std::vector<Object3D> &object_list, VkBuffer &vertex_buffer, VkDeviceMemory &vertex_buffer_memor);
bool vk_create_index_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue,
	std::vector<Object3D> &object_list, VkBuffer &index_buffer, VkDeviceMemory &index_buffer_memory);
bool vk_create_uniform_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const std::vector<VkImage> &swapchain_images,
	std::vector<VkBuffer> &uniform_buffers, std::vector<VkDeviceMemory> &uniform_buffers_memory);
bool vk_update_uniform_buffer(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, 
	std::vector<VkDeviceMemory> &uniform_buffer_memory);

bool vk_create_descriptor_set_layout(const VkDevice &device, VkDescriptorSetLayout &descriptor_set_layout);
bool vk_create_descriptor_pool(const VkDevice &device, const std::vector<VkImage> &swapchain_images, VkDescriptorPool &descriptor_pool);
bool vk_create_descriptor_sets(const VkDevice &device, const std::vector<VkImage> &swapchain_images, VkDescriptorPool &descriptor_pool,
	const std::vector<VkBuffer> &uniform_buffers, const VkImageView &texture_image_view, const VkSampler &sampler,
	const VkDescriptorSetLayout &descriptor_set_layout, std::vector<VkDescriptorSet> &descriptor_sets);

#endif