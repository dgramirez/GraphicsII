#ifndef VKOBJ_DEVICES_H
#define VKOBJ_DEVICES_H

#include "VkObj_Shared.h"

struct VkObj_Devices
{
	//Queue Variables
	VkQueue q_graphics;
	VkQueue q_present;

	//Device Variables
	VkPhysicalDevice physical;
	VkDevice logical;
};

bool vk_set_physical_device(const VkInstance& instance, const VkSurfaceKHR& surface, VkPhysicalDevice& physical_device, VkSampleCountFlagBits &msaa);
bool vk_create_logical_device(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, VkDevice& device, VkQueue& graphics_queue, VkQueue& present_queue);

bool vk_device_is_compatible(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
bool vk_device_extension_supported(const VkPhysicalDevice& physical_device);

VkSampleCountFlagBits get_highest_msaa_sample_count(const VkPhysicalDevice &physical_device);

#endif