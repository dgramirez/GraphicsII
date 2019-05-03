#ifndef VKOBJ_POOLS_H
#define VKOBJ_POOLS_H

#include "VkObj_Shared.h"

class VkObj_Pools
{
public:
	VkObj_Pools(VkDevice &device, VkStruct_QueueFamilyIndices &q_family_indices);

	bool CreateQueryPool();
	bool CreateCommandPool();
	bool CreateDescriptorPool();

	void shutdown();

	VkDevice *pDevice;
	VkStruct_QueueFamilyIndices &pQFamilyIndices;
	VkCommandPool command;
	std::vector<VkQueryPool> queries;
	VkDescriptorPool descriptor;

private:

};

#endif