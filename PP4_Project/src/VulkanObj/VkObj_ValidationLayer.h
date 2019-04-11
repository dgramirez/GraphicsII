#ifndef VALIDATION_LAYER_H
#define VALIDATION_LAYER_H

#include "VkObj_Shared.h"

bool vk_create_validation_layer(const VkInstance& instance, VkDebugUtilsMessengerEXT& debugger);

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* user_data
);

VkResult vk_create_debug_utils_messenger_ext(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* create_info,
	const VkAllocationCallbacks* allocator,
	VkDebugUtilsMessengerEXT* debug_messenger
);

void vk_destroy_debug_utils_messenger_ext(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debug_messenger,
	const VkAllocationCallbacks* allocator
);


#endif