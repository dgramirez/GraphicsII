#ifndef VKOBJ_BUFFERS_H
#define VKOBJ_BUFFERS_H

#include "VkObj_Shared.h"
#include "VkObj_WindowSetup.h"
#include "VkObj_Devices.h"
#include "VkObj_Swapchain.h"
#include "VkObj_Pools.h"

struct VkObj_Buffer
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

struct VkObj_CommandBuffer
{
	VkCommandBuffer me;
	std::vector<VkFence> fence;
};

class VkObj_Buffers
{
public:
	VkObj_Buffers(VkObj_WindowProperties &window_properties, VkObj_DeviceProperties &device_properties, VkObj_Swapchain &swapchain, VkObj_Pools &pools);

	bool CreateCommandBuffer();
	bool CreateDepthAndMSAA();
	bool CreateSwapchainFrameBuffers(const VkImageView &color_image_view, const VkImageView &depth_buffer_view, const VkRenderPass &render_pass);

	VkObj_WindowProperties *pWindowProperties;
	VkObj_DeviceProperties *pDeviceProperties;
	VkObj_Swapchain *pSwapchain;
	VkObj_Pools *pPools;
	VkObj_CommandBuffer command;
private:

};

class VkObj_UniformBuffer
{
	VkObj_UniformBuffer();


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