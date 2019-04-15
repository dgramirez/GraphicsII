#include "VkObj_Devices.h"
#include "VkObj_Swapchain.h"

//Device Extensions Exclusively for the Device Setup
const std::vector<const char*> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


//Setup or Creations of Devices
bool vk_set_physical_device(const VkInstance& instance, const VkSurfaceKHR& surface, VkPhysicalDevice& physical_device, VkSampleCountFlagBits &msaa)
{
	//Check for GPUs
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	if (!device_count) {
		LOG("No GPU Available for Vulkan!");
		return false;
	}

	//Check if GPU found is compatible [Will convert to "Find" Best GPU]
	std::vector<VkPhysicalDevice> physical_device_list(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, physical_device_list.data());
	for (size_t i = 0; i < physical_device_list.size(); ++i)
		if (vk_device_is_compatible(physical_device_list[i], surface)) {
			physical_device = physical_device_list[i];
			msaa = get_highest_msaa_sample_count(physical_device);
			return true;
		}

	//No GPU was compatible, failed.
	LOG("No devices were compatible with vulkan");
	return false;
}

bool vk_create_logical_device(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, VkDevice& device, VkQueue& graphics_queue, VkQueue& present_queue)
{
	//Setup Queue Families for Create Info
	QueueFamilyIndices indices = vk_find_queue_family(physical_device, surface);
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

	//Setup Logical device create info
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_info_array.data();
	create_info.queueCreateInfoCount = (uint32_t)queue_create_info_array.size();
	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = (uint32_t)device_extensions.size();
	create_info.ppEnabledExtensionNames = device_extensions.data();

	//Create the Surface (With Results) [VK_SUCCESS = 0]
	CHECK_VKRESULT(R, vkCreateDevice(physical_device, &create_info, nullptr, &device), "Failed to create Device!");

	//If Device has been created, Setup the Device Queue for graphics and present family
	vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.present.value(), 0, &present_queue);

	//Device has been created successfully!
	return true;
}


//Helper Functions for Setup/Creation of Devices
bool vk_device_is_compatible(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface)
{
	//Check for Device Properties and Features
	VkPhysicalDeviceProperties device_property;
	vkGetPhysicalDeviceProperties(physical_device, &device_property);

	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceFeatures(physical_device, &device_features);

	VkFormatProperties format_properties;
	vkGetPhysicalDeviceFormatProperties(physical_device, VK_FORMAT_R8G8B8A8_UNORM, &format_properties);

	//Look for Family Queues
	QueueFamilyIndices indices = vk_find_queue_family(physical_device, surface);
	bool extension_support = vk_device_extension_supported(physical_device);

	//Swapchain Support Details for Compatibilities
	bool good_swapchain = false;
	if (extension_support)
	{
		SwapChainSupportDetails support = vk_query_swapchain_support(physical_device, surface);
		good_swapchain = !support.formats.empty() && !support.presentModes.empty();
	}

	//Return true if all of these match
	return device_property.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& device_features.geometryShader
		&& device_features.samplerAnisotropy
		&& indices.IsComplete()
		&& extension_support
		&& good_swapchain
		&& format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT
		;
}

bool vk_device_extension_supported(const VkPhysicalDevice& physical_device)
{
	//Get the list of Available Extensions
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extension(extension_count);
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extension.data());

	//Find all Required Extensions and find them in the available extensions.
	std::set<std::string> required_extension(device_extensions.begin(), device_extensions.end());

	for (const auto& ext : available_extension) {
		required_extension.erase(ext.extensionName);
	}

	//If all of the required extensions has been removed from above, Then we have all the extensions available to us.
	return required_extension.empty();
}

VkSampleCountFlagBits get_highest_msaa_sample_count(const VkPhysicalDevice &physical_device)
{
	VkPhysicalDeviceProperties physical_device_properties;
	vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

	VkSampleCountFlags flags = std::min(physical_device_properties.limits.framebufferColorSampleCounts,
		physical_device_properties.limits.framebufferDepthSampleCounts);

	if (flags & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (flags & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (flags & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (flags & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (flags & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (flags & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}
