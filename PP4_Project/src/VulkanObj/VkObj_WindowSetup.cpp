#include "VkObj_WindowSetup.h"

/*//////////////////////////////////////////
 *
 *	Validation Layer Setup
 *
 *//////////////////////////////////////////
#ifdef NDEBUG
static const bool validation_layers_enabled = false;
#else
static const bool validation_layers_enabled = true;
#endif

const std::vector<const char*> validation_layers = {
"VK_LAYER_LUNARG_standard_validation"
};

/*//////////////////////////////////////////
 *
 *	Window Instance and Surface Creation
 *
 *//////////////////////////////////////////

VkObj_WindowProperties::VkObj_WindowProperties()
	:instance(nullptr), surface(nullptr), window(nullptr) {}

bool VkObj_WindowProperties::init(const char *title, SDL_Window *win)
{
	if (!window)
		window = win;
	CreateInstance(title);
	CreateValidationLayer();
	CreateSurface();
	return true;
}

void VkObj_WindowProperties::shutdown()
{
	if (surface)			vkDestroySurfaceKHR(instance, surface, nullptr);
	if (validation_layer)	vkDestroyDebugUtilsMessengerExt(instance, validation_layer, nullptr);
	if (instance)			vkDestroyInstance(instance, nullptr);
}

bool VkObj_WindowProperties::CreateInstance(const char* title)
{
	//Check for Validation Layer Support
#ifdef _DEBUG
	if (validation_layers_enabled && !CheckValidationLayerSupport())
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
	std::vector<const char*> extensions = GetRequiredExtensions();
	create_info.enabledExtensionCount = CAST(uint32_t, extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	//Application's Create Info [Part 3: Validation Layer Check]
	if (validation_layers_enabled) {
		create_info.enabledLayerCount = CAST(uint32_t, validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;

	//Create the Instance (With Results) [VK_SUCCESS = 0]
	CHECK_VKRESULT(r, vkCreateInstance(&create_info, nullptr, &instance), "Failed to Create Instance!");

	//Instance has been created successfully
	return true;
}

bool VkObj_WindowProperties::CreateSurface()
{
	//Create the Surface (With Results) [VK_SUCCESS = 0]
	SDL_Vulkan_CreateSurface(window, instance, &surface);

	//Surface has been created successfully
	return true;
}

bool VkObj_WindowProperties::CheckValidationLayerSupport()
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

std::vector<const char*> VkObj_WindowProperties::GetRequiredExtensions()
{
	uint32_t extensions_count;
	SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, nullptr);
	std::vector<const char*> extensions_vector(extensions_count);
	SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, extensions_vector.data());

	if (validation_layers_enabled)
		extensions_vector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions_vector;
}


/*//////////////////////////////////////////
 *
 *	Validation Layer Creation
 *
 *//////////////////////////////////////////

bool VkObj_WindowProperties::CreateValidationLayer()
{
	//Check to see if Validation Layer is enabled
	if (!validation_layers_enabled)
		return true; //Return true to continue down the initialization, not break it.

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
	create_info.pfnUserCallback = vkDebugCallback;
	create_info.pUserData = nullptr;


	//Create the Validation Layer (With Results)
	CHECK_VKRESULT(r, vkCreateDebugUtilsMessengerExt(instance, &create_info, nullptr, &validation_layer), "Failed to create Validation Layer");

	//Validation Layer has been created successfully
	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VkObj_WindowProperties::vkDebugCallback(
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

VkResult VkObj_WindowProperties::vkCreateDebugUtilsMessengerExt(
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

void VkObj_WindowProperties::vkDestroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
{
	auto address = (PFN_vkDestroyDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (address)
		address(instance, debug_messenger, allocator);
}