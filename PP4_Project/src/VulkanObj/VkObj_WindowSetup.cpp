#include "VkObj_WindowSetup.h"

const std::vector<const char*> validation_layers = {
"VK_LAYER_LUNARG_standard_validation"
};

bool vk_create_instance(const char *title, VkInstance &instance)
{
	//Check for Validation Layer Support
	#ifdef _DEBUG
		if (VkObj_ValidationLayer::validation_layers_enabled && !vk_check_validation_layer_support())
			LOG("Validation Layers is true, but no support!")
	#endif

	//Fill out Application Info
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = title;
	app_info.applicationVersion = 1;
	app_info.pEngineName = "N/A";
	app_info.engineVersion = 0;
	app_info.apiVersion = VK_API_VERSION_1_0;

	//Application's Create Info [Part 1: Basics]
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	//Application's Create Info [Part 2: Extensions]
	std::vector<const char*> extensions = vk_get_required_extensions();
	create_info.enabledExtensionCount = (uint32_t)extensions.size();
	create_info.ppEnabledExtensionNames = extensions.data();

	//Application's Create Info [Part 3: Validation Layer Check]
	if (VkObj_ValidationLayer::validation_layers_enabled) {
		create_info.enabledLayerCount = (uint32_t)validation_layers.size();
		create_info.ppEnabledLayerNames = validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;

	//Create the Instance (With Results) [VK_SUCCESS = 0]
	CHECK_VKRESULT(r, vkCreateInstance(&create_info, nullptr, &instance), "Failed to Create Instance!");

	//Instance has been created successfully
	return true;
}

bool vk_create_surface(const VkInstance &instance, GLFWwindow *window, VkSurfaceKHR &surface)
{
	//Create the Surface (With Results) [VK_SUCCESS = 0]
	CHECK_VKRESULT(r, glfwCreateWindowSurface(instance, window, nullptr, &surface), "Failed to create Surface");

	//Surface has been created successfully
	return true;
}

bool vk_check_validation_layer_support()
{
	//Find all available layers (For Validation Layer)
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
	std::vector<VkLayerProperties> available_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

	//Find the validation layers within the available layers
	unsigned int layer_found_count = 0;
	for (size_t i = 0; i < validation_layers.size(); ++i)
		for (size_t j = 0; j < available_layers.size(); ++j)
			if (!strcmp(validation_layers[i], available_layers[j].layerName)) {
				++layer_found_count;
				break;
			}

	if (layer_found_count == validation_layers.size())
		return true;

	return false;
}

std::vector<const char*> vk_get_required_extensions()
{
	uint32_t extensions_count;
	const char** extensions;
	extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
	std::vector<const char*> extensions_vector(extensions, extensions + extensions_count);

	if (VkObj_ValidationLayer::validation_layers_enabled)
		extensions_vector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions_vector;
}
