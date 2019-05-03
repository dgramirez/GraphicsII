#include "VkObj_Swapchain.h"

//Swapchain Creation
bool vk_create_swapchain(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkSurfaceKHR &surface, const uint32_t &window_width, const uint32_t &window_height,
	VkSwapchainKHR &swapchain, std::vector<VkImage> &swapchain_images, VkFormat &swapchain_format, VkExtent2D& swapchain_extent)
{
	//Getting Swapchain Information for Create Info
	VkStruct_SwapchainSupportDetails support = vk_query_swapchain_support(physical_device, surface);

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
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	create_info.preTransform = support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	//Create Info for Swapchain KHR [Part 2: Queue Families]
	VkStruct_QueueFamilyIndices QFamilyIndices = vk_find_queue_family(physical_device, surface);
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
	CHECK_VKRESULT(r, vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain), "Failed To Create Swap Chain!");

	//Swapchain Image Setup
	CHECK_VKRESULT(r2, vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr), "Failed to get Swapchain Images Count");
	swapchain_images.resize(image_count);
	CHECK_VKRESULT(r3, vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data()), "Failed to get Swapchain Images");

	swapchain_format = surface_format.format;
	swapchain_extent = extent;

	return true;
}

bool vk_create_swapchain_image_view(const VkDevice &device, const std::vector<VkImage>& swapchain_image, const VkFormat& format, std::vector<VkImageView>& swapchain_image_view)
{
	//Create Info for Image View
	swapchain_image_view.resize(swapchain_image.size());
	for (size_t i = 0; i < swapchain_image.size(); ++i)
		swapchain_image_view[i] = vk_create_image_view(device, swapchain_image[i], format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	//Swapchain Image View has been created successfully
	return true;
}

bool vk_create_swapchain_frame_buffer(const VkDevice &device, const VkRenderPass &render_pass, const std::vector<VkImageView> &swapchain_image_views, const VkExtent2D &swapchain_extent, 
	const VkImageView &color_image_view, const VkImageView& depth_buffer_view, std::vector<VkFramebuffer> &swapchain_frame_buffer)
{
	//Resize the Swapchain Frame Buffer Vector
	swapchain_frame_buffer.resize(swapchain_image_views.size());

	//Loop through the Swapchain Frame Buffers and set their create info
	for (unsigned int i = 0; i < swapchain_frame_buffer.size(); ++i) {
		// Create an array of image attachments for create info (NOTE: There is only 1 Color Image View and Depth Buffer!)
		std::array<VkImageView, 3> image_attachments = {
			color_image_view,
			depth_buffer_view,
			swapchain_image_views[i]
		};

		//Frame Buffer's Create Info
		VkFramebufferCreateInfo frame_buffer_create_info = {};
		frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_create_info.renderPass = render_pass;
		frame_buffer_create_info.attachmentCount = CAST(uint32_t, image_attachments.size());
		frame_buffer_create_info.pAttachments = image_attachments.data();
		frame_buffer_create_info.width = swapchain_extent.width;
		frame_buffer_create_info.height = swapchain_extent.height;
		frame_buffer_create_info.layers = 1;


		//Create the Surface (With Results) [VK_SUCCESS = 0]
		CHECK_VKRESULT(r, vkCreateFramebuffer(device, &frame_buffer_create_info, nullptr, &swapchain_frame_buffer[i]), "Failed to create Frame buffer at index" << i);
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
	if (capability.currentExtent.width != MAX_UINT32)
		return capability.currentExtent;
	else
		return { window_width, window_height };
}

VkObj_Swapchain::VkObj_Swapchain(VkObj_WindowProperties &window_properties, VkObj_DeviceProperties &device_properties)
	: pWindowProperties(&window_properties), pDeviceProperties(&device_properties) {}

bool VkObj_Swapchain::init()
{
	depth = 1;
	CreateSwapchain();
	CreateSwapchainViews();
	return true;
}

bool VkObj_Swapchain::CreateSwapchain()
{
	//Get Extent Info
	int32_t width, height;
	glfwGetFramebufferSize(pWindowProperties->window, &width, &height);
	
	//Getting Swapchain Information for Create Info
	VkStruct_SwapchainSupportDetails support = vk_query_swapchain_support(pDeviceProperties->physical, pWindowProperties->surface);

	VkSurfaceFormatKHR surface_format = vk_select_swapchain_surface_format(support.formats);
	VkPresentModeKHR present_mode = vk_select_swapchain_present_mode(support.presentModes);
	VkExtent2D extent = select_swapchain_extent(support.capabilities, width, height);

	uint32_t image_count = support.capabilities.minImageCount + 1;
	if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount)
		image_count = support.capabilities.maxImageCount;

	//Create Info for SwapchainKHR [Part 1]
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = pWindowProperties->surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	create_info.preTransform = support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	//Create Info for Swapchain KHR [Part 2: Queue Families]
	uint32_t queue_family_indices[] = { pDeviceProperties->q_family.graphics.value(), pDeviceProperties->q_family.present.value() };
	if (pDeviceProperties->q_family.graphics != pDeviceProperties->q_family.present)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Create the Swapchain [Part 3: Putting it Together] {VK_SUCCESS = 0}
	CHECK_VKRESULT(r, vkCreateSwapchainKHR(pDeviceProperties->logical, &create_info, nullptr, &me), "Failed To Create Swap Chain!");

	//Swapchain Image Setup
	CHECK_VKRESULT(r2, vkGetSwapchainImagesKHR(pDeviceProperties->logical, me, &image_count, nullptr), "Failed to get Swapchain Images Count");
	images.resize(image_count);
	CHECK_VKRESULT(r3, vkGetSwapchainImagesKHR(pDeviceProperties->logical, me, &image_count, images.data()), "Failed to get Swapchain Images");

	format = surface_format.format;
	extent = extent;

	return true;
}

bool VkObj_Swapchain::CreateSwapchainViews()
{
	//Create Info for Image View
	image_views.resize(images.size());
	for (size_t i = 0; i < images.size(); ++i)
		image_views[i] = vk_create_image_view(pDeviceProperties->logical, images[i], format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	//Swapchain Image View has been created successfully
	return true;
}

