#ifndef VKOBJ_WINDOWSETUP
#define VKOBJ_WINDOWSETUP

#include "VkObj_Shared.h"

struct VkObj_WindowSetup {
	VkInstance instance;
	VkSurfaceKHR surface;
	GLFWwindow* glfw_window;
};

bool vk_create_instance(const char* title, VkInstance& instance);

bool vk_create_surface(const VkInstance& instance, GLFWwindow* window, VkSurfaceKHR& surface);

bool vk_check_validation_layer_support();
std::vector<const char*> vk_get_required_extensions();

#endif