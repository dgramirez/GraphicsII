#ifndef VKOBJ_DEVICES_H
#define VKOBJ_DEVICES_H

#include "VkObj_Shared.h"
#include "VkObj_WindowSetup.h"

class VkObj_DeviceProperties
{
public:
	VkObj_DeviceProperties();
	bool init(VkObj_WindowProperties &windowprops);
	void shutdown();

	const uint32_t get_device_count() { return prv_DeviceCount; }
	__declspec(property(get = get_device_count)) uint32_t device_count;
	const uint32_t get_device_index() { return prv_DeviceIndex; }
	__declspec(property(get = get_device_index)) uint32_t device_index;

	//Queue Variables
	VkQueue q_graphics;
	VkQueue q_present;
	VkStruct_QueueFamilyIndices q_family;

	//Physical Device Stuff
	VkPhysicalDevice physical;
	VkDevice logical;

	//Supports
	VkSampleCountFlagBits msaa_support;
	uint32_t current_frame;

private:
	//PreRequisites
	VkInstance *pInstance;
	VkSurfaceKHR *pSurface;
	VkObj_WindowProperties *pWindow;

	//Information
	uint32_t prv_DeviceCount;
	uint32_t prv_DeviceIndex;
	std::vector<VkPhysicalDevice> prv_PhysicalDeviceList;
	std::vector<VkPhysicalDeviceProperties> prv_DeviceProperties;
	std::vector<VkPhysicalDeviceFeatures> prv_DeviceFeatures;
	std::vector<VkFormatProperties> prv_DeviceFormatProperties;
	std::vector<VkStruct_QueueFamilyIndices> prv_DeviceQFamilies;
	std::vector<std::vector<VkExtensionProperties>> prv_DeviceExtensions;

	bool SetPhysicalDevice();
	bool CreateLogicalDevice();
	bool DeviceIsCompatible(const uint32_t &index);
	bool DeviceExtensionSupported(const uint32_t &index);
	void GetDeviceInformation(const uint32_t &index);
};

#endif