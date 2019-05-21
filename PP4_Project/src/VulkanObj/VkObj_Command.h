#ifndef VKOBJ_COMMANDOBJECTS_H
#define VKOBJ_COMMANDOBJECTS_H

#include "VkObj_Shared.h"
#include "../Object.h"

struct VkObj_Commands
{
	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffers;
};

bool vk_create_command_pool(const VkPhysicalDevice &physical_device, const VkSurfaceKHR& surface, const VkDevice &device, VkCommandPool &command_pool);
bool vk_create_command_buffer(const VkDevice &device, const VkCommandPool &command_pool,
	const VkRenderPass &render_pass, const VkPipelineLayout &graphics_pipeline_layout, const VkPipeline &graphics_pipeline,
	const VkExtent2D &swapchain_extent, const std::vector<VkFramebuffer> &swapchain_frame_buffers, const std::vector<VkDescriptorSet>& descriptor_sets,
	const VkBuffer &vertex_buffers, const VkDeviceSize &vertex_buffer_offset, const VkBuffer &index_buffers, std::vector<Object3D> &object_list,
	std::vector<VkCommandBuffer> &command_buffers);

#endif