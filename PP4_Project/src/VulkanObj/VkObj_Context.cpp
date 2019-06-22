#include "pch.h"
#include "../Maths.h"
#include "VkObj_Context.h"

VkObj_Context myContext;

VkObj_Context::VkObj_Context()
{

}

bool VkObj_Context::CreateQueryPool()
{
	VkQueryPoolCreateInfo query_pool_create_info = {};
	query_pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	query_pool_create_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
	query_pool_create_info.queryCount = 16; //TODO: FIGURE OUT HOW MANY QUERY COUNTS I SHOULD HAVE!

	query_pool.resize(MAX_FRAMES);
	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		vkCreateQueryPool(device.logical, &query_pool_create_info, nullptr, &query_pool[i]);

	return true;
}

bool VkObj_Context::CreateCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info = {};
	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.queueFamilyIndex = device.q_family.graphics.value();

	CHECK_VKRESULT(r, vkCreateCommandPool(device.logical, &command_pool_create_info, nullptr, &command_pool), "Failed to create Command Pool!");
	return true;
}

void VkObj_Context::shutdown()
{	
	if (command_pool)	vkDestroyCommandPool(device.logical, command_pool, nullptr);
	
	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
	{
		if (query_pool[i])
			vkDestroyQueryPool(device.logical, query_pool[i], nullptr);
	}

	device.shutdown();
	window.shutdown();
}

bool VkObj_Context::init(SDL_Window *win)
{
	//Create Instance, Validation Layer, and Surface
	window.init("New Vulkan Application", win);

	//Create Physical and Logical Device
	device.init(window);

	//Create the Pools
	CreateQueryPool();
	CreateCommandPool();

	//Create the swapchain
	swapchain.init(window, device, command_pool); 

	//Create the pipelines
	const uint32_t pmask_normal = PMASK_COLORMAP;
	const uint32_t pmask_nocull = PMASK_COLORMAP | PMASK_NOCULLING;
	const uint32_t pmask_skybox = pmask_nocull | PMASK_NODEPTH;
	const uint32_t pmask_normalmap = PMASK_COLORMAP | PMASK_NORMALMAP;
	const uint32_t pmask_earth = pmask_normal | PMASK_AMBIENTMAP | PMASK_SPECMAP;

	pipelines.init(device, swapchain);
	pipelines.create_pipeline(pmask_normal, "shaders/Spir-V/BasicTexture.vert.spv", "shaders/Spir-V/BasicTexture.frag.spv");			//PIPELINE_TEXTURE
	pipelines.create_pipeline(pmask_normal, "shaders/Spir-V/PlanetsBasic.vert.spv", "shaders/Spir-V/PlanetsBasic.frag.spv");			//PIPELINE_PLANETS
	pipelines.create_pipeline(pmask_nocull, "shaders/Spir-V/BasicTexture.vert.spv", "shaders/Spir-V/BasicTexture.frag.spv");			//PIPELINE_GRID
	pipelines.create_pipeline(pmask_skybox, "shaders/Spir-V/BasicTexture.vert.spv", "shaders/Spir-V/BasicTexture.frag.spv");			//PIPELINE_SKYBOX
	pipelines.create_pipeline(pmask_normal, "shaders/Spir-V/Ships.vert.spv", "shaders/Spir-V/Ships.frag.spv");							//PIPELINE_PHONG
	pipelines.create_pipeline(pmask_nocull, "shaders/Spir-V/Flag.vert.spv", "shaders/Spir-V/Flag.frag.spv");							//PIPELINE_FLAG
	pipelines.create_pipeline(pmask_nocull, "shaders/Spir-V/BasicTexture.vert.spv", "shaders/Spir-V/BasicTexture.frag.spv");			//PIPELINE_HUD
	pipelines.create_pipeline(pmask_normalmap, "shaders/Spir-V/PlanetsNormalMap.vert.spv", "shaders/Spir-V/PlanetsNormalMap.frag.spv");	//PIPELINE_PLANET_NORMALMAPPED

	return true;
}


