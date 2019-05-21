#ifndef VKOBJ_SWAPCHAIN_H
#define VKOBJ_SWAPCHAIN_H

#include "VkObj_Shared.h"
#include "VkObj_WindowSetup.h"
#include "VkObj_Devices.h"

struct VkObj_Swapchains
{
	VkSwapchainKHR swapchain;
	std::vector<VkImage> images;
	std::vector<VkImageView> views;
	std::vector<VkFramebuffer> frame_buffers;
	VkFormat format;
	union
	{
		VkExtent3D extent3D;
		struct { VkExtent2D extent2D; uint32_t depth; };
	};
};

class VkObj_Swapchain
{
public:
	VkObj_Swapchain();
	bool init(VkObj_WindowProperties &window_properties, VkObj_DeviceProperties &device_properties, VkCommandPool &pools);

	union {
		VkExtent3D extent3D;
		struct { VkExtent2D extent2D; uint32_t depth; };
	};
	VkFormat format;
	std::vector<VkImage> images;
	std::vector<VkImageView> image_views;
	std::vector<VkFramebuffer> frame_buffers;
	VkSwapchainKHR me;

	VkRenderPass render_pass;
	VkRenderPass render_pass_no_clear;

	struct VkStruct_Image{
		VkImage image;
		VkDeviceMemory memory;
		VkImageView view;
	};

	VkStruct_Image msaa;
	VkStruct_Image zbuffer;
	uint32_t image_index;

	static uint32_t swapchain_size;
	std::vector<VkCommandBuffer> command_buffer;

private:
	bool CreateSwapchain();
	bool CreateRenderPass();
	bool CreateRenderPassNoClear();
	bool CreateMSAA();
	bool CreateDepthBuffer();
	bool CreateFrameBuffer();
	bool CreateCommandBuffer();
	VkObj_WindowProperties *pWindowProperties;
	VkObj_DeviceProperties *pDeviceProperties;
	VkCommandPool *pCommandPool;
};

bool vk_create_swapchain(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkSurfaceKHR &surface, const uint32_t &window_width, const uint32_t &window_height, 
	VkSwapchainKHR &swapchain, std::vector<VkImage> &swapchain_images, VkFormat &swapchain_format, VkExtent2D& swapchain_extent);
bool vk_create_swapchain_image_view(const VkDevice &device, const std::vector<VkImage>& swapchain_image, const VkFormat& format, std::vector<VkImageView>& swapchain_image_view);

//Usage comes AFTER Render Pass and Graphics Pipeline
bool vk_create_swapchain_frame_buffer(const VkDevice &device, const VkRenderPass &render_pass, const std::vector<VkImageView> &swapchain_image_views, const VkExtent2D &swapchain_extent,
	const VkImageView &color_image_view, const VkImageView& depth_buffer_view, std::vector<VkFramebuffer> &swapchain_frame_buffer);
											

VkSurfaceFormatKHR vk_select_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR> &formats);
VkPresentModeKHR vk_select_swapchain_present_mode(const std::vector<VkPresentModeKHR> &present_modes);
VkExtent2D select_swapchain_extent(const VkSurfaceCapabilitiesKHR &capability, const uint32_t &window_width, const uint32_t &window_height);

#endif