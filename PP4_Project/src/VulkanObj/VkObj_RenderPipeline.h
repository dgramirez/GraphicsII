#ifndef VKOBJ_RENDERPIPELINE
#define VKOBJ_RENDERPIPELINE

#include "VkObj_Shared.h"
#include "VkObj_Devices.h"
#include "VkObj_Swapchain.h"
#include <vector>

#define PIPELINE_TEXTURE 0
#define PIPELINE_PLANETS 1
#define PIPELINE_GRID 2
#define PIPELINE_SKYBOX 3
#define PIPELINE_PHONG 4
#define PIPELINE_FLAG 5

class VkObj_RenderPipeline
{
public:
	VkObj_RenderPipeline() = default;

	void init(VkObj_DeviceProperties &device, VkObj_Swapchain &swapchain);
	void create_pipeline(const char *vertex_shader, const char *fragment_shader, bool enable_depth = true, bool enable_culling = true);
	void shutdown();
	void clean_pipeline();
	void reset_pipeline();

	std::vector<VkStruct_Pipeline> pipelines;
private:
	void CreateDescriptorPool(VkDescriptorPool &descriptor_pool);
	void CreateDescriptorSetLayout(VkDescriptorSetLayout &descriptor_set_layout);
	void CreateGraphicsPipeline(const char *vertex_shader, const char *fragment_shader, VkDescriptorSetLayout &descriptor_set_layout, VkPipelineLayout &pipeline_layout, VkPipeline &pipeline, bool enable_depth, bool enable_culling);

	VkObj_DeviceProperties *pDevice;
	VkObj_Swapchain *pSwapchain;
};

#endif