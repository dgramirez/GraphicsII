#ifndef VKOBJ_DEVICES_H
#define VKOBJ_DEVICES_H

#include "VkObj_Shared.h"

struct VkObj_Devices
{
	//Queue Variables
	QueueFamilyIndices family_queue;
	VkQueue graphics_queue;
	VkQueue present_queue;

	//Device Variables
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
};

bool vk_set_physical_device(const VkInstance& instance, const VkSurfaceKHR& surface, VkPhysicalDevice& physical_device);
bool vk_create_logical_device(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, VkDevice& device, VkQueue& graphics_queue, VkQueue& present_queue);

bool vk_device_is_compatible(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);
bool vk_device_extension_supported(const VkPhysicalDevice& physical_device);

VkSampleCountFlags get_highest_msaa_sample_count();

#endif