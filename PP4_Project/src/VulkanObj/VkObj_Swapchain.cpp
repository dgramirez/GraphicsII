#include "VkObj_Swapchain.h"

//Swapchain Creation
bool vk_create_swapchain(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkSurfaceKHR &surface, const uint32_t &window_width, const uint32_t &window_height,
	VkSwapchainKHR &swapchain, std::vector<VkImage> &swapchain_images, VkFormat &swapchain_format, VkExtent2D& swapchain_extent)
{
	//Getting Swapchain Information for Create Info
	SwapChainSupportDetails support = vk_query_swapchain_support(physical_device, surface);

	VkSurfaceFormatKHR surface_format = vk_select_swapchain_surface_format(support.formats);
	VkPresentModeKHR present_mode = vk_select_swapchain_present_mode(support.presentModes);
	VkExtent2D extent = select_swapchain_extent(support.capabilities, window_width, window_height);

	uint32_t image_count = support.capabilities.minImageCount + 1;
	if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount)
		image_count = support.capabilities.maxImageCount;


	//Create Info for SwapchainKHR [Part 1]
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	create_info.preTransform = support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = true;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	//Create Info for Swapchain KHR [Part 2: Queue Families]
	QueueFamilyIndices QFamilyIndices = vk_find_queue_family(physical_device, surface);
	uint32_t queue_family_indices[] = { QFamilyIndices.graphics.value(), QFamilyIndices.present.value() };
	if (QFamilyIndices.graphics != QFamilyIndices.present)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Create the Swapchain [Part 3: Putting it Together] {VK_SUCCESS = 0}
	VkResult r = vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain);
	if (r) {
		LOG("Failed To Create Swap Chain! Error Code: " << r)
		return false;
	}

	//Swapchain Image Setup
	vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
	swapchain_images.resize(image_count);
	vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());

	swapchain_format = surface_format.format;
	swapchain_extent = extent;

	return true;
}

bool vk_create_swapchain_image_view(const VkDevice &device, const std::vector<VkImage>& swapchain_image, const VkFormat& format, std::vector<VkImageView>& swapchain_image_view)
{
	//Create Info for Image View
	swapchain_image_view.resize(swapchain_image.size());
	for (uint32_t i = 0; i < (uint32_t)swapchain_image.size(); ++i)
		swapchain_image_view[i] = vk_create_image_view(device, swapchain_image[i], format, VK_IMAGE_ASPECT_COLOR_BIT, 0);

	//Swapchain Image View has been created successfully
	return true;
}

bool vk_create_swapchain_frame_buffer(const VkDevice &device, const VkRenderPass &render_pass, const std::vector<VkImageView> &swapchain_image_views, const VkExtent2D &swapchain_extent, 
	const VkImageView& depth_buffer_view, std::vector<VkFramebuffer> &swapchain_frame_buffer)
{
	//Resize the Swapchain Frame Buffer Vector
	swapchain_frame_buffer.resize(swapchain_image_views.size());

	//Loop through the Swapchain Frame Buffers and set their create info
	for (unsigned int i = 0; i < swapchain_frame_buffer.size(); ++i) {
		// Create an array of image attachments for create info (swapchain image views and depth buffer view)
		std::array<VkImageView, 2> image_attachments = {
			swapchain_image_views[i],
			depth_buffer_view
		};

		//Frame Buffer's Create Info
		VkFramebufferCreateInfo frame_buffer_create_info = {};
		frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_create_info.renderPass = render_pass;
		frame_buffer_create_info.attachmentCount = (uint32_t)image_attachments.size();
		frame_buffer_create_info.pAttachments = image_attachments.data();
		frame_buffer_create_info.width = swapchain_extent.width;
		frame_buffer_create_info.height = swapchain_extent.height;
		frame_buffer_create_info.layers = 1;


		//Create the Surface (With Results) [VK_SUCCESS = 0]
		VkResult r = vkCreateFramebuffer(device, &frame_buffer_create_info, nullptr, &swapchain_frame_buffer[i]);
		if (r) {
			LOG("Failed to create Frame buffer at index" << i);
			return false;
		}

	}

	//Swapchain's Frame Buffers has been created successfully
	return true;
}


//Swapchain Helper Functions
VkSurfaceFormatKHR vk_select_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR> &formats)
{
	//Find or Determine Best Surface Format
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (unsigned int i = 0; i < formats.size(); ++i) {
		if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return formats[i];
	}

	//If no best format is found, choose the first index
	return formats[0];
}

VkPresentModeKHR vk_select_swapchain_present_mode(const std::vector<VkPresentModeKHR> &present_modes)
{
	//Find or Determine Best Surface Present Mode
	VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;

	for (int i = 0; i < present_modes.size(); ++i) {
		if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return present_modes[i];
		else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			best_mode = present_modes[i];
	}

	//Return the best mode
	return best_mode;
}

VkExtent2D select_swapchain_extent(const VkSurfaceCapabilitiesKHR &capability, const uint32_t &window_width, const uint32_t &window_height)
{
	//Find or Determine Best Surface Swap Extent
	if (capability.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capability.currentExtent;
	else
		return { window_width, window_height };
}

