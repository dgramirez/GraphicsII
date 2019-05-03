#include "VkObj_Pools.h"

VkObj_Pools::VkObj_Pools(VkDevice &device, VkStruct_QueueFamilyIndices &q_family_indices)
	:pDevice(&device), pQFamilyIndices(q_family_indices), command(nullptr), descriptor(nullptr) {}

bool VkObj_Pools::CreateQueryPool()
{
	VkQueryPoolCreateInfo query_pool_create_info = {};
	query_pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	query_pool_create_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
	query_pool_create_info.queryCount = 16; //TODO: FIGURE OUT HOW MANY QUERY COUNTS I SHOULD HAVE!

	queries.resize(MAX_FRAMES);
	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		vkCreateQueryPool(*pDevice, &query_pool_create_info, nullptr, &queries[i]);

	return true;
}

bool VkObj_Pools::CreateCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info = {};
	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.queueFamilyIndex = pQFamilyIndices.graphics.value();

	CHECK_VKRESULT(r, vkCreateCommandPool(*pDevice, &command_pool_create_info, nullptr, &command), "Failed to create Command Pool!");
	return true;
}

void VkObj_Pools::shutdown()
{
	if (descriptor)		vkDestroyDescriptorPool(*pDevice, descriptor, nullptr);
	if (command)		vkDestroyCommandPool(*pDevice, command, nullptr);
	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		if (queries[i])	vkDestroyQueryPool(*pDevice, queries[i], nullptr);
}
