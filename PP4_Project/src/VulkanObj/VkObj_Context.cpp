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
	pipelines.init(device, swapchain);
	pipelines.create_pipeline("shaders/Spir-V/vertBasic.spv", "shaders/Spir-V/fragBasic.spv");										//PIPELINE_TEXTURE
	pipelines.create_pipeline("shaders/Spir-V/vertBasicTexture.spv", "shaders/Spir-V/fragBasicTexture.spv");						//PIPELINE_PLANETS
	pipelines.create_pipeline("shaders/Spir-V/vertBasic.spv", "shaders/Spir-V/fragBasic.spv", true, VK_COMPARE_OP_LESS, false);		//PIPELINE_GRID
	pipelines.create_pipeline("shaders/Spir-V/vertBasic.spv", "shaders/Spir-V/fragBasic.spv", false);								//PIPELINE_SKYBOX
	pipelines.create_pipeline("shaders/Spir-V/vertShips.spv", "shaders/Spir-V/fragShips.spv");										//PIPELINE_PHONG
	pipelines.create_pipeline("shaders/Spir-V/vertFlag.spv", "shaders/Spir-V/fragFlag.spv", true, VK_COMPARE_OP_LESS, false);		//PIPELINE_FLAG
	pipelines.create_pipeline("shaders/Spir-V/vertBasic.spv", "shaders/Spir-V/fragBasic.spv", true, VK_COMPARE_OP_LESS, false);		//PIPELINE_HYD
	pipelines.create_pipeline_normalmaps("shaders/Spir-V/NormalMapped.vert.spv", "shaders/Spir-V/NormalMapped.frag.spv");			//PIPELINE_EARTH_NORMAL

	return true;
}


