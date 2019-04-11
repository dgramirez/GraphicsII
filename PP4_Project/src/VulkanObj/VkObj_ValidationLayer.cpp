#include "VkObj_ValidationLayer.h"

const std::vector<const char*> validation_layers = {
"VK_LAYER_LUNARG_standard_validation"
};

bool vk_create_validation_layer(const VkInstance& instance, VkDebugUtilsMessengerEXT& debugger)
{
	//Check to see if Validation Layer is enabled
	if (!VkObj_ValidationLayer::validation_layers_enabled)
		return true;

	//Create info for Debugger
	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = vk_debug_callback;
	create_info.pUserData = nullptr;


	//Create the Validation Layer (With Results) [VK_SUCCESS = 0]
	VkResult r = vk_create_debug_utils_messenger_ext(instance, &create_info, nullptr, &debugger);
	if (r) {
		LOG("Failed to create Validation Layer")
		return false;
	}

	//Validation Layer has been created successfully
	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT * callback_data,
	void * user_data)
{
#ifdef _DEBUG
	std::cerr << "Validation Layer: " << callback_data->pMessage << std::endl;
#endif
	return VK_FALSE;
}

VkResult vk_create_debug_utils_messenger_ext(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT * create_info,
	const VkAllocationCallbacks * allocator,
	VkDebugUtilsMessengerEXT * debug_messenger)
{
	auto address = (PFN_vkCreateDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	if (address)
		return address(instance, create_info, allocator, debug_messenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vk_destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
{
	auto address = (PFN_vkDestroyDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (address)
		address(instance, debug_messenger, allocator);
}