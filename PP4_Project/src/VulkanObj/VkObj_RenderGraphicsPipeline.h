#ifndef VKOBJ_GRAPHICSPIPELINE_H
#define VKOBJ_GRAPHICSPIPELINE_H

#include "VkObj_Shared.h"
#include "../Object.h"

struct VkObj_RenderGraphicsPipeline {
	VkRenderPass render_pass;
	VkPipelineLayout graphics_pipeline_layout;
	VkPipeline graphics_pipeline;
};

bool vk_create_render_pass(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkFormat &swapchain_format, const VkSampleCountFlagBits &msaa, VkRenderPass &render_pass);
bool vk_create_graphics_pipeline(const VkDevice &device, const VkExtent2D &swapchain_extent, const VkDescriptorSetLayout &descriptor_set_layout, const VkSampleCountFlagBits &msaa,
	const VkRenderPass &render_pass, VkPipelineLayout &graphics_pipeline_layout, VkPipeline &graphics_pipelinee);

// static std::vector<char> vk_read_shader_file(const std::string& filename);
// VkShaderModule vk_create_shader_module(const VkDevice &device, const std::vector<char>& shader);

#endif