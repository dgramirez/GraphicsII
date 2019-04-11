#ifndef VKOBJ_SWAPCHAIN_H
#define VKOBJ_SWAPCHAIN_H

#include "VkObj_Shared.h"

struct VkObj_Swapchain
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

bool vk_create_swapchain(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkSurfaceKHR &surface, const uint32_t &window_width, const uint32_t &window_height, 
	VkSwapchainKHR &swapchain, std::vector<VkImage> &swapchain_images, VkFormat &swapchain_format, VkExtent2D& swapchain_extent);
bool vk_create_swapchain_image_view(const VkDevice &device, const std::vector<VkImage>& swapchain_image, const VkFormat& format, std::vector<VkImageView>& swapchain_image_view);

//Usage comes AFTER Render Pass and Graphics Pipeline
bool vk_create_swapchain_frame_buffer(const VkDevice &device, const VkRenderPass &render_pass, const std::vector<VkImageView> &swapchain_image_views, const VkExtent2D &swapchain_extent,
	const VkImageView& depth_buffer_view, std::vector<VkFramebuffer> &swapchain_frame_buffer);
											

VkSurfaceFormatKHR vk_select_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR> &formats);
VkPresentModeKHR vk_select_swapchain_present_mode(const std::vector<VkPresentModeKHR> &present_modes);
VkExtent2D select_swapchain_extent(const VkSurfaceCapabilitiesKHR &capability, const uint32_t &window_width, const uint32_t &window_height);

#endif