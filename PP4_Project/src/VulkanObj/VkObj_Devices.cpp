#include "pch.h"
#include "VkObj_Devices.h"
#include "VkObj_Swapchain.h"

//Device Extensions Exclusively for the Device Setup
const std::vector<const char*> req_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkObj_DeviceProperties::VkObj_DeviceProperties() {}

//Public Methods
bool VkObj_DeviceProperties::init(VkObj_WindowProperties &windowprops)
{
	pWindow = &windowprops;
	SetPhysicalDevice();
	CreateLogicalDevice();
	return true;
}

void VkObj_DeviceProperties::shutdown()
{
	if (logical) vkDestroyDevice(logical, nullptr);
}

//Important Methods
bool VkObj_DeviceProperties::SetPhysicalDevice()
{
	//Check for GPUs
	vkEnumeratePhysicalDevices(pWindow->instance, &prv_DeviceCount, nullptr);
	if (!prv_DeviceCount) {
		LOG("No GPU Available for Vulkan!");
		return false;
	}

	//Check if GPU found is compatible [Will convert to "Find" Best GPU]
	prv_PhysicalDeviceList.resize(prv_DeviceCount);
	vkEnumeratePhysicalDevices(pWindow->instance, &prv_DeviceCount, prv_PhysicalDeviceList.data());

	//Gather Physical Hardware Information
	for (uint32_t i = 0; i < prv_PhysicalDeviceList.size(); ++i)
		GetDeviceInformation(i);

	//Choose Physical Hardware
	for (uint32_t i = 0; i < prv_PhysicalDeviceList.size(); ++i)
	{
		if (DeviceIsCompatible(i)) {
			prv_DeviceIndex = i;
			q_family = prv_DeviceQFamilies[i];
			physical = prv_PhysicalDeviceList[i];
			msaa_support = get_highest_msaa_sample_count(physical);
			return true;
		}
	}

	//No GPU was compatible, failed.
	LOG("No devices were compatible with vulkan");
	return false;
}

bool VkObj_DeviceProperties::CreateLogicalDevice()
{
	//Setup Queue Families for Create Info
	VkStruct_QueueFamilyIndices indices = vk_find_queue_family(physical, pWindow->surface);
	std::set<uint32_t> unique_queue_families = { indices.graphics.value(), indices.present.value() };
	std::vector<VkDeviceQueueCreateInfo> queue_create_info_array(unique_queue_families.size());

	//Set up Create Info for all unique queue families
	float priority = 1.0f;
	for (uint32_t i : unique_queue_families) {
		VkDeviceQueueCreateInfo create_info = {};

		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		create_info.queueFamilyIndex = i;
		create_info.queueCount = 1;
		create_info.pQueuePriorities = &priority;

		queue_create_info_array[i] = create_info;
	}

	//Setup device features (Any Additional Features SHOULD BE CHECKED in DeviceCompatibility Function)
	VkPhysicalDeviceFeatures device_features = {};
	device_features.samplerAnisotropy = true;
	device_features.sampleRateShading = VK_TRUE;

	//Setup Logical device create info
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_info_array.data();
	create_info.queueCreateInfoCount = CAST(uint32_t, queue_create_info_array.size());
	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = CAST(uint32_t, prv_DeviceExtensions.size());
	create_info.ppEnabledExtensionNames = req_extensions.data();

	//Create the Surface (With Results) [VK_SUCCESS = 0]
	CHECK_VKRESULT(R, vkCreateDevice(physical, &create_info, nullptr, &logical), "Failed to create Device!");

	//If Device has been created, Setup the Device Queue for graphics and present family
	vkGetDeviceQueue(logical, indices.graphics.value(), 0, &q_graphics);
	vkGetDeviceQueue(logical, indices.present.value(), 0, &q_present);

	//Device has been created successfully!
	return true;
}

//Helper Methods
bool VkObj_DeviceProperties::DeviceExtensionSupported(const uint32_t &index)
{
	//Find all Required Extensions and find them in the available extensions.
	std::set<std::string> required_extension(req_extensions.begin(), req_extensions.end());

	for (const auto& ext : prv_DeviceExtensions[index]) {
		required_extension.erase(ext.extensionName);
	}

	//If all of the required extensions has been removed from above, Then we have all the extensions available to us.
	return required_extension.empty();
}

bool VkObj_DeviceProperties::DeviceIsCompatible(const uint32_t &index)
{
	//Look for Extension Support
	bool extension_support = DeviceExtensionSupported(index);

	//Swapchain Support Details for Compatibilities
	bool good_swapchain = false;
	if (extension_support)
	{
		VkStruct_SwapchainSupportDetails support = vk_query_swapchain_support(prv_PhysicalDeviceList[index], pWindow->surface);
		good_swapchain = !support.formats.empty() && !support.presentModes.empty();
	}

	//Return true if all of these match
	return prv_DeviceProperties[index].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& prv_DeviceFeatures[index].geometryShader
		&& prv_DeviceFeatures[index].samplerAnisotropy
		&& prv_DeviceQFamilies[index].IsComplete()
		&& extension_support
		&& good_swapchain
		&& prv_DeviceFormatProperties[index].optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT
		;
}

void VkObj_DeviceProperties::GetDeviceInformation(const uint32_t &index)
{
	//Get the Device Properties
	VkPhysicalDeviceProperties device_property;
	vkGetPhysicalDeviceProperties(prv_PhysicalDeviceList[index], &device_property);
	prv_DeviceProperties.push_back(device_property);

	//Get the Device Features
	VkPhysicalDeviceFeatures device_feature;
	vkGetPhysicalDeviceFeatures(prv_PhysicalDeviceList[index], &device_feature);
	prv_DeviceFeatures.push_back(device_feature);

	//Get the Format Properties
	VkFormatProperties format_properties;
	vkGetPhysicalDeviceFormatProperties(prv_PhysicalDeviceList[index], VK_FORMAT_R8G8B8A8_UNORM, &format_properties);
	prv_DeviceFormatProperties.push_back(format_properties);

	//Look for Family Queues
	prv_DeviceQFamilies.push_back(vk_find_queue_family(prv_PhysicalDeviceList[index], pWindow->surface));

	//Get the list of Available Extensions
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(prv_PhysicalDeviceList[index], nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> available_extension(extension_count);
	vkEnumerateDeviceExtensionProperties(prv_PhysicalDeviceList[index], nullptr, &extension_count, available_extension.data());
	prv_DeviceExtensions.push_back(available_extension);
}


