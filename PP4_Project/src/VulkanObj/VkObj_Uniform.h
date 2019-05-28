#ifndef VKOBJ_UNIFORM_H
#define VKOBJ_UNIFORM_H

#include "VkObj_Shared.h"
#include "VkObj_Devices.h"
#include "VkObj_Swapchain.h"
#include "../Maths.h"

// class VkObj_Uniform
// {
// public:
// 	VkObj_Uniform();
// 
// 	void init(VkObj_DeviceProperties &device, VkObj_Swapchain &swapchain, VkCommandPool &command_pool);
// 	bool CreateUniformBuffer(std::vector<VkBuffer> &buffer, std::vector<VkDeviceMemory> &memory);
// 
// 	std::vector<VkBuffer> buffer;
// 	std::vector<VkDeviceMemory> memory;
// 
// 	VkDescriptorSetLayout prv_DescriptorSetLayout;
// 	std::vector<VkDescriptorSet> prv_DescriptorSets;
// private:
// 	VkObj_DeviceProperties *pDevice;
// 	VkObj_Swapchain *pSwapchain;
// 	VkCommandPool *pCommandPool;
// 
// 	bool CreateDescriptorSetLayout();
//};

#endif VKOBJ_UNIFORM_H