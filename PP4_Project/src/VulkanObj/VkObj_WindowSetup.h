#ifndef VKOBJ_WINDOWSETUP
#define VKOBJ_WINDOWSETUP

#include "VkObj_Shared.h"

class VkObj_WindowProperties {
public:
	VkObj_WindowProperties();
	bool init(const char *title, SDL_Window *win = nullptr);
	void shutdown();

	VkInstance instance;
	VkSurfaceKHR surface;
	SDL_Window* window;

private:

	//Window Related
	bool CreateInstance(const char* title);
	bool CreateValidationLayer();
	bool CreateSurface();
	std::vector<const char*> GetRequiredExtensions();
	
	//Validation Layer Related
	VkDebugUtilsMessengerEXT validation_layer;
	bool CheckValidationLayerSupport();
	static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data
	);

	VkResult vkCreateDebugUtilsMessengerExt(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* create_info,
		const VkAllocationCallbacks* allocator,
		VkDebugUtilsMessengerEXT* debug_messenger
	);

	void vkDestroyDebugUtilsMessengerExt(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks* allocator
	);
};

#endif