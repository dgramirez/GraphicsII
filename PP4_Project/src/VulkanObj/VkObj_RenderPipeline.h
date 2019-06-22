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
#define PIPELINE_HUD 6
#define PIPELINE_PLANET_NORMALMAPPED 7
#define PIPELINE_PLANET_EARTH 8

#define PMASK_COLORMAP 0x01
#define PMASK_NORMALMAP 0x02
#define PMASK_AMBIENTMAP 0x04
#define PMASK_SPECMAP 0x08
#define PMASK_GEOSHADER 0x10
#define PMASK_TESSELATION 0x20

#define PMASK_NODEPTH 0x100
#define PMASK_NOCULLING 0x200
#define PMASK_WIREFRAME	0x8000

class VkObj_RenderPipeline
{
public:
	VkObj_RenderPipeline() = default;

	void init(VkObj_DeviceProperties &device, VkObj_Swapchain &swapchain);
//	void create_pipeline(const char *vertex_shader, const char *fragment_shader, bool enable_depth = true, VkCompareOp depth_op = VK_COMPARE_OP_LESS, bool enable_culling = true);
	void create_pipeline(const uint32_t &pipeline_mask, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader = nullptr);
//	void create_pipeline_normalmaps(const char *vertex_shader, const char *fragment_shader, bool enable_depth = true, VkCompareOp depth_op = VK_COMPARE_OP_LESS, bool enable_culling = true);
	void shutdown();
	void clean_pipeline();
	void reset_pipeline();

	std::vector<VkStruct_Pipeline> pipelines;
private:
	void CreateDescriptorPool(const uint32_t &pipeline_mask, VkDescriptorPool &descriptor_pool);
	void CreateDescriptorSetLayout(const uint32_t &pipeline_mask, VkDescriptorSetLayout &descriptor_set_layout);
	void CreateGraphicsPipeline(const uint32_t & pipeline_mask, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader, VkDescriptorSetLayout &descriptor_set_layout, VkPipelineLayout &pipeline_layout, VkPipeline &pipeline);

	void CreateDescriptorPool_NM(VkDescriptorPool &descriptor_pool);
	void CreateDescriptorSetLayout_NM(VkDescriptorSetLayout &descriptor_set_layout);
	
	VkObj_DeviceProperties *pDevice;
	VkObj_Swapchain *pSwapchain;
};

#endif