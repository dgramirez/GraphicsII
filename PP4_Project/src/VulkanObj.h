#ifndef VULKANOBJ_H
#define VULKANOBJ_H

#pragma region VULKAN_INCLUDE_AND_DEFINES

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#ifdef _DEBUG
#include <iostream>
#define LOG(x) std::cout << x << std::endl;
#else
#define LOG(x) 
#endif

#pragma endregion This contains defines and includes with GLFW and Vulkan

#define MAX_FRAMES_FLIGHT 2

#include <optional>
#include <set>
#include <algorithm>
#include <cstring>
#include "vulkan/vulkan.h"
#include <vector>
#include <fstream>
#include <array>

class VulkanObj
{
public:
	VulkanObj();
	~VulkanObj();

	bool init(const char* title, GLFWwindow* window, unsigned short win_width, unsigned short win_height);
	void draw_frames();
	void cleanup();
	void idle_device();
	void reset_swapchain(unsigned short win_width, unsigned short win_height);

private:

	uint32_t prv_Frame = 0;

#pragma region Instance
	VkInstance prv_Instance;
	std::vector<const char*> GetRequiredExtensions();

	bool CreateInstance(const char* title);
	bool CheckValidationLayerSupport();

#pragma endregion

#pragma region Validation Debugger

	const std::vector<const char*> validation_layers = {
	"VK_LAYER_LUNARG_standard_validation"
	};

#ifdef NDEBUG
	const bool validation_layers_enabled = false;
#else
	const bool validation_layers_enabled = true;
#endif

	VkDebugUtilsMessengerEXT prv_Debugger;
	bool CreateValidationDebugger();

	static VKAPI_ATTR VkBool32 VKAPI_CALL Callback_Debug(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data
	);

	VkResult CustomVkCreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* create_info,
		const VkAllocationCallbacks* allocator,
		VkDebugUtilsMessengerEXT* debug_messenger
	);

	void CustomVkDestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks* allocator
	);

#pragma endregion 
	
#pragma region Surface

	VkSurfaceKHR prv_Surface;
	bool CreateSurface(GLFWwindow* window);

#pragma endregion
	
#pragma region Physical Device

	struct QueueFamilyIndices {
		std::optional<uint32_t> Family_Graphics;
		std::optional<uint32_t> Family_Present;

		bool IsComplete() { return Family_Graphics.has_value() && Family_Present.has_value(); }
	};

	VkPhysicalDevice prv_PhysicalDevice;
	
	bool SetPhysicalDevice();

	bool DeviceCompatible(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

#pragma endregion

#pragma region Logical Device

	VkQueue prv_QueueGraphics;
	VkQueue prv_QueuePresent;
	VkDevice prv_Device;

	bool CreateLogicalDevice();

#pragma endregion

#pragma region Swapchain | Image View | Frame Buffers

	const std::vector<const char*> prv_DeviceExt =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	VkSwapchainKHR prv_Swapchain;
	std::vector<VkImage> prv_SwapchainImages;
	std::vector<VkImageView> prv_SwapchainImageViews;
	std::vector<VkFramebuffer> prv_SwapchainFrameBuffers;
	VkFormat prv_SwapchainFormat;
	VkExtent2D prv_SwapchainExtent;


	bool CreateSwapChain(unsigned short win_width, unsigned short win_height);
	bool CreateImageView();

	bool CreateFrameBuffers();	//Usage comes AFTER Render Pass
								//and Graphics Pipeline

	void CleanupSwapchain();
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& mode);
	VkExtent2D SelectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, unsigned short win_width, unsigned short win_height);

#pragma endregion

#pragma region Render Pass | Graphics Pipeline

	VkRenderPass prv_RenderPass;
	VkPipelineLayout prv_PipelineLayout;
	VkPipeline prv_GraphicsPipeline;

	bool CreateRenderPass();
	bool CreateGraphicsPipeline();
	static std::vector<char> ReadShaderFile(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& shader);

#pragma endregion

#pragma region  Command Pool | Command Buffer

	VkCommandPool prv_CommandPool;
	std::vector<VkCommandBuffer> prv_CommandBuffers;

	bool CreateCommandPool();
	bool CreateCommandBuffers();

#pragma endregion

#pragma region Semaphore | Fences

	std::vector<VkSemaphore> prv_ImageAvailableSemaphore;
	std::vector<VkSemaphore> prv_RenderFinishedSemaphore;
	std::vector<VkFence> prv_Fences;

	bool SyncSemaphoreAndFences();

#pragma endregion

};

#endif