#include "VulkanObj.h"

VulkanObj::VulkanObj()	{}
VulkanObj::~VulkanObj() { cleanup(); }

bool VulkanObj::init(const char* title, GLFWwindow* window, unsigned short win_width, unsigned short win_height)
{
	if (!CreateInstance(title))						{ LOG("Create Instance Has Failed!")			return false; }
	if (!CreateValidationDebugger())				{ LOG("Create Validation Debugger Has Failed!")	return false; }
	if (!CreateSurface(window))						{ LOG("Create Surface Has Failed!")				return false; }
	if (!SetPhysicalDevice())						{ LOG("Set Physical Device Has Failed!")		return false; }
	if (!CreateLogicalDevice())						{ LOG("Create Logical Device Has Failed!")		return false; }
	if (!CreateSwapChain(win_width, win_height))	{ LOG("Create Swap Chain Has Failed!")			return false; }
	if (!CreateImageView())							{ LOG("Create Image View Has Failed!")			return false; }
	if (!CreateRenderPass())						{ LOG("Create Render Pass Has Failed!")			return false; }
	if (!CreateDescriptorSetLayout())				{ LOG("Create Descriptor Set Layout Failed!")	return false; }
	if (!CreateGraphicsPipeline())					{ LOG("Create Graphics Pipeline Has Failed!")	return false; }
	if (!CreateCommandPool())						{ LOG("Create Command Pool Has Failed!")		return false; }
	if (!CreateDepthResources())					{ LOG("Create Depth Resources Has Failed!")		return false; }
	if (!CreateFrameBuffers())						{ LOG("Create Frame Buffers Has Failed")		return false; }
	if (!CreateTextureImage())						{ LOG("Create Texture Image Has Failed!")		return false; }
	if (!CreateTextureImageView())					{ LOG("Create Texture Image View Has Failed!")	return false; }
	if (!CreateTextureSampler())					{ LOG("Create Texture Image View Has Failed!")	return false; }
	if (!CreateVertexBuffer())						{ LOG("Creating Vertex Buffer Has Failed!")		return false; }
	if (!CreateIndexBuffer())						{ LOG("Creating Index Buffer Has Failed!")		return false; }
	if (!CreateUniformBuffer())						{ LOG("Create Uniform Buffer Has Failed!")		return false; }
	if (!CreateDescriptorPool())					{ LOG("Create Descriptor Pool Has Failed!")		return false; }
	if (!CreateDescriptorSets())					{ LOG("Create Desriptor Sets Has Failed!")		return false; }
	if (!CreateCommandBuffers())					{ LOG("Create Command Buffer Has Failed!")		return false; }
	if (!SyncSemaphoreAndFences())					{ LOG("Create Semaphore Has Failed!")			return false; }
	return true;
}

void VulkanObj::idle_device()
{
	vkDeviceWaitIdle(prv_Device);
}

void VulkanObj::reset_swapchain(unsigned short win_width, unsigned short win_height)
{
	vkDeviceWaitIdle(prv_Device);

	CleanupSwapchain();

	CreateSwapChain(win_width, win_height);
	CreateImageView();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFrameBuffers();
	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffers();
}

void VulkanObj::setup_object_list(uint32_t size)
{
	prv_ObjectList.resize(size);
}

void VulkanObj::add_to_object_list(const Object & object)
{
	prv_ObjectList.push_back(object);
}

void VulkanObj::cleanup()
{
	for (unsigned int i = 0; i < MAX_FRAMES_FLIGHT; ++i)
	{
		if (prv_RenderFinishedSemaphore[i]) vkDestroySemaphore(prv_Device, prv_RenderFinishedSemaphore[i], nullptr);
		if (prv_ImageAvailableSemaphore[i]) vkDestroySemaphore(prv_Device, prv_ImageAvailableSemaphore[i], nullptr);
		if (prv_Fences[i])					vkDestroyFence(prv_Device, prv_Fences[i], nullptr);
	}

	CleanupSwapchain();
	if (prv_Sampler)				vkDestroySampler(prv_Device, prv_Sampler, nullptr);
	if (prv_TextureImageView)		vkDestroyImageView(prv_Device, prv_TextureImageView, nullptr);

	if (prv_TextureImage)			vkDestroyImage(prv_Device, prv_TextureImage, nullptr);
	if (prv_TextureImageMemory)		vkFreeMemory(prv_Device, prv_TextureImageMemory, nullptr);
	
	if (prv_DescriptorPool)			vkDestroyDescriptorPool(prv_Device, prv_DescriptorPool, nullptr);
	if (prv_DescriptorSetLayout)	vkDestroyDescriptorSetLayout(prv_Device, prv_DescriptorSetLayout, nullptr);
	
	if (prv_IndexBuffer)			vkDestroyBuffer(prv_Device, prv_IndexBuffer, nullptr);
	if (prv_IndexBufferMemory)		vkFreeMemory(prv_Device, prv_IndexBufferMemory, nullptr);
	
	if (prv_VertexBuffer)			vkDestroyBuffer(prv_Device, prv_VertexBuffer, nullptr);
	if (prv_VertexBufferMemory)		vkFreeMemory(prv_Device, prv_VertexBufferMemory, nullptr);	
	
	if (prv_CommandPool)			vkDestroyCommandPool(prv_Device, prv_CommandPool, nullptr);
	if (prv_Device)					vkDestroyDevice(prv_Device, nullptr);
	if (prv_Surface)				vkDestroySurfaceKHR(prv_Instance, prv_Surface, nullptr);
	if (validation_layers_enabled)	CustomVkDestroyDebugUtilsMessengerEXT(prv_Instance, prv_Debugger, nullptr);
	if (prv_Instance)				vkDestroyInstance(prv_Instance, nullptr);
}

void VulkanObj::draw_frames()
{
	vkWaitForFences(prv_Device, 1, &prv_Fences[prv_Frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t image_index;
	VkResult frame_result = vkAcquireNextImageKHR(prv_Device, prv_Swapchain, std::numeric_limits<uint64_t>::max(), 
		prv_ImageAvailableSemaphore[prv_Frame], VK_NULL_HANDLE, &image_index);
	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR)
		return;
	else if (frame_result != VK_SUCCESS && frame_result != VK_SUBOPTIMAL_KHR)
	{
		LOG("CANNOT DRAW FRAME!!!!!");
		return;
	}

	UpdateUniformBuffer(image_index);

	VkSemaphore wait_semaphores[] = { prv_ImageAvailableSemaphore[prv_Frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphore[] = { prv_RenderFinishedSemaphore[prv_Frame] };


	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &prv_CommandBuffers[image_index];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;

	vkResetFences(prv_Device, 1, &prv_Fences[prv_Frame]);

	if (vkQueueSubmit(prv_QueueGraphics, 1, &submit_info, prv_Fences[prv_Frame]))
	{
		LOG("Failed to Submit Queue");
	}

	VkSwapchainKHR swapchains[] = { prv_Swapchain };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphore; //<--- I don't get it! why signal
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	frame_result = vkQueuePresentKHR(prv_QueuePresent, &present_info);

	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		LOG("Swapchain Out of Date!")
	}
	else if (frame_result == VK_SUBOPTIMAL_KHR)
	{
		LOG("Present Queue is Suboptimal!")
	}

	prv_Frame = (prv_Frame + 1) % MAX_FRAMES_FLIGHT;
}



bool VulkanObj::CreateInstance(const char* title)
{
#pragma region Validation Layer Check

#ifdef _DEBUG
	if (validation_layers_enabled && !CheckValidationLayerSupport())
		LOG("Validation Layers is true, but no support!")
#endif
#pragma endregion

#pragma region APPLICATION INFO

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = title;
	app_info.applicationVersion = 1;
	app_info.pEngineName = "";
	app_info.engineVersion = 0;
	app_info.apiVersion = VK_API_VERSION_1_0;

#pragma endregion Filling out application information

#pragma region CREATE INFO

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	std::vector<const char*> extensions = GetRequiredExtensions();
	create_info.enabledExtensionCount = (uint32_t)extensions.size();
	create_info.ppEnabledExtensionNames = extensions.data();

	if (validation_layers_enabled)
	{
		create_info.enabledLayerCount = (uint32_t)validation_layers.size();
		create_info.ppEnabledLayerNames = validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;

	vkCreateInstance(&create_info, nullptr, &prv_Instance);

#pragma endregion Filling out Instance Information
	
	return true;
}

bool VulkanObj::CreateValidationDebugger()
{
	if (!validation_layers_enabled)
		return true;

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
	create_info.pfnUserCallback = Callback_Debug;
	create_info.pUserData = nullptr;

	if (CustomVkCreateDebugUtilsMessengerEXT(prv_Instance, &create_info, nullptr, &prv_Debugger))
	{
		LOG("Failed to create Validation Layer")
		return false;
	}

	return true;
}

bool VulkanObj::CreateSurface(GLFWwindow* window)
{
#pragma region Creating GLFW Window Surface

	VkResult r = glfwCreateWindowSurface(prv_Instance, window, nullptr, &prv_Surface);
	if (r)
	{
		LOG("Failed to create Surface");
		return false;
	}

#pragma endregion

	return true;
}

bool VulkanObj::SetPhysicalDevice()
{
#pragma region Device Count

	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(prv_Instance, &device_count, nullptr);
	if (!device_count){ 
		LOG("No GPU Available for Vulkan!");
		return false;
	}

	#pragma endregion

#pragma region Filling Device List

	std::vector<VkPhysicalDevice> physical_device_list(device_count);
	vkEnumeratePhysicalDevices(prv_Instance, &device_count, physical_device_list.data());
	for (size_t i = 0; i < physical_device_list.size(); ++i)
		if (DeviceCompatible(physical_device_list[i])) {
			prv_PhysicalDevice = physical_device_list[i];
			return true;
		}

#pragma endregion

	LOG("No devices were compatible with vulkan");
	return false;
}

bool VulkanObj::CreateLogicalDevice()
{

#pragma region Setup Queue Families for Create Info

	QueueFamilyIndices indices = FindQueueFamilies(prv_PhysicalDevice);
	std::set<uint32_t> unique_queue_families = { indices.Family_Graphics.value(), indices.Family_Present.value() };
	std::vector<VkDeviceQueueCreateInfo> queue_create_info_array(unique_queue_families.size());

#pragma endregion

#pragma region Setting up Create Info for Queue Families

	float priority = 1.0f;

	for (uint32_t i : unique_queue_families)
	{
		VkDeviceQueueCreateInfo create_info = {};

		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		create_info.queueFamilyIndex = i;
		create_info.queueCount = 1;
		create_info.pQueuePriorities = &priority;

		queue_create_info_array[i] = create_info;
	}

#pragma endregion

#pragma region Setup for Logical Device

	VkPhysicalDeviceFeatures device_features = {};
	device_features.samplerAnisotropy = true;
	
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_info_array.data();
	create_info.queueCreateInfoCount = (uint32_t)queue_create_info_array.size();
	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = (unsigned int)prv_DeviceExt.size();
	create_info.ppEnabledExtensionNames = prv_DeviceExt.data();

	if (vkCreateDevice(prv_PhysicalDevice, &create_info, nullptr, &prv_Device))
	{
		LOG("Failed to create Device");
		return false;
	}
	
	vkGetDeviceQueue(prv_Device, indices.Family_Graphics.value(), 0, &prv_QueueGraphics);
	vkGetDeviceQueue(prv_Device, indices.Family_Present.value(), 0, &prv_QueuePresent);

#pragma endregion

	return true;
}

bool VulkanObj::CreateSwapChain(unsigned int win_width, unsigned int win_height)
{
#pragma region Getting Swapchain Information for Create Info

	SwapChainSupportDetails support = QuerySwapChainSupport(prv_PhysicalDevice);

	VkSurfaceFormatKHR surface_format = SelectSwapSurfaceFormat(support.formats);
	VkPresentModeKHR present_mode = SelectSwapPresentMode(support.presentModes);
	VkExtent2D extent = SelectSwapExtent(support.capabilities, win_width, win_height);

	uint32_t image_count = support.capabilities.minImageCount + 1;
	if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount)
		image_count = support.capabilities.maxImageCount;

#pragma endregion

#pragma region Create Info for SwapchainKHR [Part 1]

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = prv_Surface;
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

#pragma endregion
	
#pragma region Create Info for Swapchain KHR [Part 2: Queue Families]

	QueueFamilyIndices QFamilyIndices = FindQueueFamilies(prv_PhysicalDevice);
	uint32_t queue_family_indices[] = { QFamilyIndices.Family_Graphics.value(), QFamilyIndices.Family_Present.value() };
	if (QFamilyIndices.Family_Graphics != QFamilyIndices.Family_Present)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateSwapchainKHR(prv_Device, &create_info, nullptr, &prv_Swapchain))
	{
		LOG("Failed to create Swap Chain!");
		return false;
	}

#pragma endregion

#pragma region Swapchain Image Setup

	vkGetSwapchainImagesKHR(prv_Device, prv_Swapchain, &image_count, nullptr);
	prv_SwapchainImages.resize(image_count);
	vkGetSwapchainImagesKHR(prv_Device, prv_Swapchain, &image_count, prv_SwapchainImages.data());

	prv_SwapchainFormat = surface_format.format;
	prv_SwapchainExtent = extent;

#pragma endregion
	
	return true;
}

bool VulkanObj::CreateImageView()
{
#pragma region Create Info for Image View

	prv_SwapchainImageViews.resize(prv_SwapchainImages.size());
	for (unsigned int i = 0; i < (unsigned int)prv_SwapchainImages.size(); ++i)
		prv_SwapchainImageViews[i] = CreateImageViewObject(prv_SwapchainImages[i], prv_SwapchainFormat);

#pragma endregion

	return true;
}

bool VulkanObj::CreateTextureSampler()
{
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler_create_info.anisotropyEnable = false;
	sampler_create_info.maxAnisotropy = 1;
	sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_create_info.unnormalizedCoordinates = false;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.minLod = 0.0f;
	sampler_create_info.maxLod = 0.0f;

	if (vkCreateSampler(prv_Device, &sampler_create_info, nullptr, &prv_Sampler))
	{
		LOG("Sampler Has Failed to be Created!")
		return false;
	}

	return true;
}

bool VulkanObj::CreateDepthResources()
{
	VkFormat depth_format = GetDepthFormat();

	CreateImage({ prv_SwapchainExtent.width, prv_SwapchainExtent.height, 1 }, 0, depth_format,  VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, prv_DepthBuffer, prv_DepthBufferMemory);
	prv_DepthBufferView = CreateImageViewObject(prv_DepthBuffer, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
	
	TransitionImageLayout(prv_DepthBuffer, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	return true;
}

VkFormat VulkanObj::FindSupportedFormats(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags format_features_flags)
{
	for (uint32_t i = 0; i < formats.size(); ++i)
	{
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(prv_PhysicalDevice, formats[i], &format_properties);

		if (tiling == VK_IMAGE_TILING_LINEAR &&
			(format_properties.linearTilingFeatures & format_features_flags) == format_features_flags)
		{
			return formats[i];
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
			(format_properties.optimalTilingFeatures & format_features_flags) == format_features_flags)
		{
			return formats[i];
		}
		else
		{
			LOG("Failed to find a supported format! Sending back VK_FORMAT_UNDEFINED");
		}
	}

	return VK_FORMAT_UNDEFINED;
}

VkFormat VulkanObj::GetDepthFormat()
{
	std::vector<VkFormat> formats{
		VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
	};

	return FindSupportedFormats(formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool VulkanObj::ContainsStencilFormat(VkFormat format)
{
	return format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}

bool VulkanObj::CreateTextureImageView()
{
	prv_TextureImageView = CreateImageViewObject(prv_TextureImage, VK_FORMAT_R8G8B8A8_UNORM);
	return true;
}

bool VulkanObj::CreateRenderPass()
{
	VkAttachmentDescription color_attachment_description = {};
	color_attachment_description.format = prv_SwapchainFormat;
	color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_reference = {};
	color_attachment_reference.attachment = 0;
	color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_attachment_description = {};
	depth_attachment_description.format = GetDepthFormat();
	depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_reference = {};
	depth_attachment_reference.attachment = 1;
	depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass_description = {};
	subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_attachment_reference;
	subpass_description.pDepthStencilAttachment = &depth_attachment_reference;

	VkSubpassDependency subpass_dependency = {};
	subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass = 0;
	subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstAccessMask =
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = {color_attachment_description, depth_attachment_description};
	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = (uint32_t)attachments.size();
	render_pass_create_info.pAttachments = attachments.data();
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pSubpasses = &subpass_description;
	render_pass_create_info.dependencyCount = 1;
	render_pass_create_info.pDependencies = &subpass_dependency;

	if (vkCreateRenderPass(prv_Device, &render_pass_create_info, nullptr, &prv_RenderPass))
	{
		LOG("Failed to create Render Pass");
		return false;
	}

	return true;
}

bool VulkanObj::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding mvp_layout_binding = {};
	mvp_layout_binding.binding = 0;
	mvp_layout_binding.descriptorCount = 1;
	mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvp_layout_binding.pImmutableSamplers = nullptr;
	mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding sampler_layout_binding = {};
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = 1;
	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.pImmutableSamplers = nullptr;
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { mvp_layout_binding, sampler_layout_binding };
	VkDescriptorSetLayoutCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	create_info.bindingCount = (uint32_t)bindings.size();
	create_info.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(prv_Device, &create_info, nullptr, &prv_DescriptorSetLayout))
	{
		LOG("Failed to create Descriptor Set Layout");
		return false;
	}



	return true;
}

bool VulkanObj::CreateGraphicsPipeline()
{
#pragma region Check and copy shader files

	std::vector<char> shader_vertex_file = ReadShaderFile("src/shaders/vert.spv");
	std::vector<char> shader_fragment_file = ReadShaderFile("src/shaders/frag.spv");

	if (shader_vertex_file.empty() || shader_fragment_file.empty())
	{
		LOG("Failed to Load Shaders.");
		return false;
	}

	VkShaderModule shader_vertex_module = CreateShaderModule(shader_vertex_file);
	VkShaderModule shader_fragment_module = CreateShaderModule(shader_fragment_file);

#pragma endregion In case I forget, glslangValidator.exe is NOT in the project. its with the actual SDK

#pragma region Shaders Create Info

	VkPipelineShaderStageCreateInfo vertex_shader_create_info = {};
	vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_shader_create_info.module = shader_vertex_module;
	vertex_shader_create_info.pName = "main";

	VkPipelineShaderStageCreateInfo fragment_shader_create_info = {};
	fragment_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_shader_create_info.module = shader_fragment_module;
	fragment_shader_create_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_create_info, fragment_shader_create_info };

#pragma endregion

#pragma region Assembly and Vertex Input

	VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
	assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assembly_create_info.primitiveRestartEnable = false;

	auto vertex_binding_description = Vertex::get_binding_description();
	auto vertex_attribute_description = Vertex::get_attribute_description();

	VkPipelineVertexInputStateCreateInfo input_vertex_info = {};
	input_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_vertex_info.vertexBindingDescriptionCount = 1;
	input_vertex_info.pVertexBindingDescriptions = &vertex_binding_description;
	input_vertex_info.vertexAttributeDescriptionCount = (uint32_t)vertex_attribute_description.size();
	input_vertex_info.pVertexAttributeDescriptions = vertex_attribute_description.data();

#pragma endregion

#pragma region Viewport and Scissor Setup and Create Info

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)prv_SwapchainExtent.width;
	viewport.height = (float)prv_SwapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = prv_SwapchainExtent;

	VkPipelineViewportStateCreateInfo viewport_create_info = {};
	viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_create_info.viewportCount = 1;
	viewport_create_info.pViewports = &viewport;
	viewport_create_info.scissorCount = 1;
	viewport_create_info.pScissors = &scissor;

#pragma endregion

#pragma region Rasterizer

	VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
	rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_create_info.rasterizerDiscardEnable = false;
	rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_create_info.lineWidth = 1.0f;
	rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_create_info.depthClampEnable = false;
	rasterization_create_info.depthBiasEnable = false;
	rasterization_create_info.depthBiasClamp = 0.0f;
	rasterization_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_create_info.depthBiasSlopeFactor = 0.0f;

#pragma endregion

#pragma region Multisampling

	VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
	multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_create_info.sampleShadingEnable = false;
	multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_create_info.minSampleShading = 1.0f;
	multisample_create_info.pSampleMask = nullptr;
	multisample_create_info.alphaToCoverageEnable = false;
	multisample_create_info.alphaToOneEnable = false;

#pragma endregion

#pragma region Depth and Stencil

	VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
	depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_create_info.depthTestEnable = true;
	depth_stencil_create_info.depthWriteEnable = true;
	depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_create_info.depthBoundsTestEnable = false;
	depth_stencil_create_info.minDepthBounds = 0.0f;
	depth_stencil_create_info.maxDepthBounds = 1.0f;
	depth_stencil_create_info.stencilTestEnable = false;

#pragma endregion (Note: Nothing Going on here. using nullptr. Used for reference)

#pragma region Color Blending

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
	color_blend_attachment_state.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment_state.blendEnable = false;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
	color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_create_info.logicOpEnable = false;
	color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
	color_blend_create_info.attachmentCount = 1;
	color_blend_create_info.pAttachments = &color_blend_attachment_state;
	color_blend_create_info.blendConstants[0] = 0.0f;
	color_blend_create_info.blendConstants[1] = 0.0f;
	color_blend_create_info.blendConstants[2] = 0.0f;
	color_blend_create_info.blendConstants[3] = 0.0f;

#pragma endregion

#pragma region Dynamic State

// 	VkDynamicState dynamicStates[] = {
// 		VK_DYNAMIC_STATE_VIEWPORT,
// 		VK_DYNAMIC_STATE_LINE_WIDTH
// 	};
// 
// 	VkPipelineDynamicStateCreateInfo dynamic_create_info;
// 	dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
// 	dynamic_create_info.dynamicStateCount = 2;
// 	dynamic_create_info.pDynamicStates = dynamicStates;

#pragma endregion (Note: Nothing going on here. using nullptr. Used for Reference)

#pragma region Pipeline Layout

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &prv_DescriptorSetLayout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(prv_Device, &pipeline_layout_create_info, nullptr, &prv_PipelineLayout))
	{
		LOG("Failed to create Pipeline Layout!");
		return false;
	}

#pragma endregion

#pragma region Pipeline Creation

	VkGraphicsPipelineCreateInfo pipeline_create_info = {};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.stageCount = 2;
	pipeline_create_info.pStages = shader_stages;
	pipeline_create_info.pInputAssemblyState = &assembly_create_info;
	pipeline_create_info.pVertexInputState = &input_vertex_info;
	pipeline_create_info.pViewportState = &viewport_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_create_info;
	pipeline_create_info.pMultisampleState = &multisample_create_info;
	pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
	pipeline_create_info.pColorBlendState = &color_blend_create_info;
	pipeline_create_info.pDynamicState = nullptr;

	pipeline_create_info.layout = prv_PipelineLayout;
	pipeline_create_info.renderPass = prv_RenderPass;
	pipeline_create_info.subpass = 0;

	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(prv_Device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &prv_GraphicsPipeline))
	{
		LOG("Pipeline Creation Has Failed!");
		return false;
	}

#pragma endregion

	//Destroy Module
	vkDestroyShaderModule(prv_Device, shader_vertex_module, nullptr);
	vkDestroyShaderModule(prv_Device, shader_fragment_module, nullptr);

	return true;
}

bool VulkanObj::CreateFrameBuffers()
{
	prv_SwapchainFrameBuffers.resize(prv_SwapchainImageViews.size());

	for (unsigned int i = 0; i < prv_SwapchainFrameBuffers.size(); ++i)
	{
		std::array<VkImageView,2> image_attachments = {
			prv_SwapchainImageViews[i],
			prv_DepthBufferView
		};

		VkFramebufferCreateInfo frame_buffer_create_info = {};
		frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_create_info.renderPass = prv_RenderPass;
		frame_buffer_create_info.attachmentCount = (uint32_t)image_attachments.size();
		frame_buffer_create_info.pAttachments = image_attachments.data();
		frame_buffer_create_info.width = prv_SwapchainExtent.width;
		frame_buffer_create_info.height = prv_SwapchainExtent.height;
		frame_buffer_create_info.layers = 1;

		if (vkCreateFramebuffer(prv_Device, &frame_buffer_create_info, nullptr, &prv_SwapchainFrameBuffers[i]))
		{
			LOG("Failed to create Frame buffer at index" << i);
			return false;
		}

	}

	return true;
}

bool VulkanObj::CreateCommandPool()
{
	QueueFamilyIndices queue_family_indices = FindQueueFamilies(prv_PhysicalDevice);
	
	VkCommandPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_create_info.queueFamilyIndex = queue_family_indices.Family_Graphics.value();

	if (vkCreateCommandPool(prv_Device, &pool_create_info, nullptr, &prv_CommandPool))
	{
		LOG("Failed to create Command Pool");
		return false;
	}

	return true;
}

bool VulkanObj::CreateVertexBuffer()
{
		VkDeviceSize buffer_size = sizeof(prv_ObjectList[0].vertices[0]) * prv_ObjectList[0].vertices.size();
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;

		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			staging_buffer, staging_buffer_memory);

		void* data;
		vkMapMemory(prv_Device, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, prv_ObjectList[0].vertices.data(), (uint32_t)buffer_size);
		vkUnmapMemory(prv_Device, staging_buffer_memory);

		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			prv_VertexBuffer, prv_VertexBufferMemory);

		CopyBuffer(staging_buffer, prv_VertexBuffer, buffer_size);

		vkDestroyBuffer(prv_Device, staging_buffer, nullptr);
		vkFreeMemory(prv_Device, staging_buffer_memory, nullptr);

		return true;
}

bool VulkanObj::CreateIndexBuffer()
{
		VkDeviceSize buffer_size = sizeof(prv_ObjectList[0].indices[0]) * prv_ObjectList[0].indices.size();
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;

		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			staging_buffer, staging_buffer_memory);

		void* data;
		vkMapMemory(prv_Device, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, prv_ObjectList[0].indices.data(), (uint32_t)buffer_size);
		vkUnmapMemory(prv_Device, staging_buffer_memory);

		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			prv_IndexBuffer, prv_IndexBufferMemory);

		CopyBuffer(staging_buffer, prv_IndexBuffer, buffer_size);

		vkDestroyBuffer(prv_Device, staging_buffer, nullptr);
		vkFreeMemory(prv_Device, staging_buffer_memory, nullptr);

	return true;
}

bool VulkanObj::CreateUniformBuffer()
{
	VkDeviceSize buffer_size = sizeof(Mvp_object);

	prv_UniformBuffers.resize(prv_SwapchainImages.size());
	prv_UniformBuffersMemory.resize(prv_SwapchainImages.size());

	for (uint32_t i = 0; i < prv_SwapchainImages.size(); ++i)
	{
		CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			prv_UniformBuffers[i], prv_UniformBuffersMemory[i]);
	}

	return true;
}

bool VulkanObj::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize,2> descriptor_pool_size = {};
	descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_pool_size[0].descriptorCount = (uint32_t)prv_SwapchainImages.size();
	descriptor_pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_pool_size[1].descriptorCount = (uint32_t)prv_SwapchainImages.size();

	VkDescriptorPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = (uint32_t)descriptor_pool_size.size();
	create_info.pPoolSizes = descriptor_pool_size.data();
	create_info.maxSets = (uint32_t)prv_SwapchainImages.size();

	if (vkCreateDescriptorPool(prv_Device, &create_info, nullptr, &prv_DescriptorPool))
	{
		LOG("Failed to create a Desriptor Pool");
		return false;
	}

	return true;
}

bool VulkanObj::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> descriptor_set_layout(prv_SwapchainImages.size(), prv_DescriptorSetLayout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate_info.descriptorSetCount = (uint32_t)prv_SwapchainImages.size();
	descriptor_set_allocate_info.descriptorPool = prv_DescriptorPool;
	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout.data();

	prv_DescriptorSets.resize(prv_SwapchainImages.size());
	if (vkAllocateDescriptorSets(prv_Device, &descriptor_set_allocate_info, prv_DescriptorSets.data()))
	{
		LOG("Failed to Allocate Descriptor Set");
		return false;
	}

	for (uint32_t i = 0; i < prv_SwapchainImages.size(); ++i)
	{
		VkDescriptorBufferInfo descriptor_buffer_info = {};
		descriptor_buffer_info.buffer = prv_UniformBuffers[i];
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = sizeof(Mvp_object);

		VkDescriptorImageInfo descriptor_image_info = {};
		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_image_info.imageView = prv_TextureImageView;
		descriptor_image_info.sampler = prv_Sampler;


		std::array<VkWriteDescriptorSet,2> write_descriptor_set = {};
		write_descriptor_set[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[0].dstSet = prv_DescriptorSets[i];
		write_descriptor_set[0].dstBinding = 0;
		write_descriptor_set[0].dstArrayElement = 0;
		write_descriptor_set[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor_set[0].descriptorCount = 1;
		write_descriptor_set[0].pBufferInfo = &descriptor_buffer_info;
		write_descriptor_set[0].pImageInfo = nullptr;
		write_descriptor_set[0].pTexelBufferView = nullptr;

		write_descriptor_set[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[1].dstSet = prv_DescriptorSets[i];
		write_descriptor_set[1].dstBinding = 1;
		write_descriptor_set[1].dstArrayElement = 0;
		write_descriptor_set[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptor_set[1].descriptorCount = 1;
		write_descriptor_set[1].pBufferInfo = nullptr;
		write_descriptor_set[1].pImageInfo = &descriptor_image_info;
		write_descriptor_set[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(prv_Device, (uint32_t)write_descriptor_set.size(), write_descriptor_set.data(), 0, nullptr);
	}

	return true;
}

bool VulkanObj::CreateTextureImage()
{
	VkDeviceSize image_size = celestial_width * celestial_height * sizeof(unsigned int);

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	CreateBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

	std::vector<unsigned int> converted_pixels(celestial_width * celestial_height);
	for (uint32_t i = 0; i < converted_pixels.size(); ++i)
	{
		Color current_pixel(celestial_pixels[i]);
		Color new_pixel = (current_pixel.a << 24) | (current_pixel.b << 16) | (current_pixel.g << 8) | current_pixel.r;
		converted_pixels[i] = new_pixel.color;
	}

	void* data = nullptr;
	vkMapMemory(prv_Device, staging_buffer_memory, 0, image_size, 0, &data);
	memcpy(data, converted_pixels.data(), (unsigned int)image_size);
	vkUnmapMemory(prv_Device, staging_buffer_memory);

	CreateImage({ celestial_width, celestial_height, 1 }, celestial_numlevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, prv_TextureImage, prv_TextureImageMemory);

	TransitionImageLayout(prv_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(staging_buffer, prv_TextureImage, { celestial_width, celestial_height, 1 });
	TransitionImageLayout(prv_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(prv_Device, staging_buffer, nullptr);
	vkFreeMemory(prv_Device, staging_buffer_memory, nullptr);

	return true;
}

VkCommandBuffer VulkanObj::StartSingleCommand()
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool = prv_CommandPool;
	command_buffer_allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(prv_Device, &command_buffer_allocate_info, &command_buffer);

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

	return command_buffer;
}

void VulkanObj::EndSingleCommand(VkCommandBuffer command_buffer)
{
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(prv_QueueGraphics, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(prv_QueueGraphics);

	vkFreeCommandBuffers(prv_Device, prv_CommandPool, 1, &command_buffer);
}

void VulkanObj::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout previous_layout, VkImageLayout current_layout)
{
	VkCommandBuffer command_buffer = StartSingleCommand();

	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.image = image;
	image_memory_barrier.oldLayout = previous_layout;
	image_memory_barrier.newLayout = current_layout;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.layerCount = 1;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;

	VkPipelineStageFlags source_stage;
	VkPipelineStageFlags destrination_stage;

	if (current_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (ContainsStencilFormat(format))
			image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	if (previous_layout == VK_IMAGE_LAYOUT_UNDEFINED)
	{
		if (current_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (current_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
	}
	else if (previous_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && current_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destrination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		LOG("No Transition Image Layout Avialble! May Cause Problems!")
	}

	vkCmdPipelineBarrier(command_buffer, source_stage, destrination_stage, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

	EndSingleCommand(command_buffer);
}

void VulkanObj::CopyBufferToImage(VkBuffer buffer, VkImage image, VkExtent3D extent)
{
	VkCommandBuffer command_buffer = StartSingleCommand();

	VkBufferImageCopy buffer_image_copy = {};
	buffer_image_copy.bufferOffset = 0;
	buffer_image_copy.bufferRowLength = 0;
	buffer_image_copy.bufferImageHeight = 0;
	buffer_image_copy.imageSubresource.layerCount = 1;
	buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	buffer_image_copy.imageSubresource.mipLevel = 0;
	buffer_image_copy.imageSubresource.baseArrayLayer = 0;
	buffer_image_copy.imageOffset = { 0,0,0 };
	buffer_image_copy.imageExtent = extent;

	vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);

	EndSingleCommand(command_buffer);

}


bool VulkanObj::CreateCommandBuffers()
{
	prv_CommandBuffers.resize(prv_SwapchainFrameBuffers.size());

	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = prv_CommandPool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = (uint32_t)prv_CommandBuffers.size();

	if (vkAllocateCommandBuffers(prv_Device, &command_buffer_allocate_info, prv_CommandBuffers.data()))
	{
		LOG("Failed to Allocate Command Buffer!");
		return false;
	}
	for (unsigned int i = 0; i < prv_CommandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo command_buffer_begin_info = {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		command_buffer_begin_info.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(prv_CommandBuffers[i], &command_buffer_begin_info))
		{
			LOG("Failed to Begin Command Buffer at index " << i);
			return false;
		}

		VkRenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = prv_RenderPass;
		render_pass_begin_info.framebuffer = prv_SwapchainFrameBuffers[i];
		render_pass_begin_info.renderArea.offset = { 0,0 };
		render_pass_begin_info.renderArea.extent = prv_SwapchainExtent;
		
		std::array<VkClearValue, 2> clear_color = {};
		clear_color[0] = { 0.0f,0.0f,0.0f,1.0f };
		clear_color[1] = { 1.0f, 0.0f };

		render_pass_begin_info.clearValueCount = (uint32_t)clear_color.size();
		render_pass_begin_info.pClearValues = clear_color.data();

		vkCmdBeginRenderPass(prv_CommandBuffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(prv_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, prv_GraphicsPipeline);

			VkBuffer vertex_buffers[] = { prv_VertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(prv_CommandBuffers[i], 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(prv_CommandBuffers[i], prv_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(prv_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, prv_PipelineLayout, 0, 1, &prv_DescriptorSets[i], 0, nullptr);

			vkCmdDrawIndexed(prv_CommandBuffers[i], (uint32_t)prv_ObjectList[0].indices.size(), 1, 0, 0, 0);
		vkCmdEndRenderPass(prv_CommandBuffers[i]);

		if (vkEndCommandBuffer(prv_CommandBuffers[i]))
		{
			LOG("Failed to End Command Buffer at index " << i);
			return false;
		}
	}

	return true;
}

bool VulkanObj::SyncSemaphoreAndFences()
{
	prv_ImageAvailableSemaphore.resize(MAX_FRAMES_FLIGHT);
	prv_RenderFinishedSemaphore.resize(MAX_FRAMES_FLIGHT);
	prv_Fences.resize(MAX_FRAMES_FLIGHT);

	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (unsigned int i = 0; i < MAX_FRAMES_FLIGHT; ++i)
	{
		if (vkCreateSemaphore(prv_Device, &semaphore_create_info, nullptr, &prv_ImageAvailableSemaphore[i]))
		{
			LOG("Semaphore Creation for Image Available Semaphore has Failed!");
			return false;
		}
		if (vkCreateSemaphore(prv_Device, &semaphore_create_info, nullptr, &prv_RenderFinishedSemaphore[i]))
		{
			LOG("Semaphore Creation for Render Finished Semaphore has Failed!");
			return false;
		}
		if (vkCreateFence(prv_Device, &fence_create_info, nullptr, &prv_Fences[i]))
		{
			LOG("Fence Creation has Failed!");
			return false;
		}
	}

	return true;
}

bool VulkanObj::UpdateUniformBuffer(uint32_t current_image)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	Mvp_object mvp;

#if USE_PYRAMID
	mvp.model = glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
#else
	mvp.model = glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
#endif
	mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.projection = glm::perspective(glm::radians(45.0f), prv_SwapchainExtent.width / (float)prv_SwapchainExtent.height, 0.1f, 10.0f);

	mvp.projection[1][1] = -mvp.projection[1][1];

	void* data;
	vkMapMemory(prv_Device, prv_UniformBuffersMemory[current_image], 0, sizeof(Mvp_object), 0, &data);
		memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(prv_Device, prv_UniformBuffersMemory[current_image]);

	return true;
}



bool VulkanObj::CheckValidationLayerSupport()
{
#pragma region Get Available Layers

	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
	std::vector<VkLayerProperties> available_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

#pragma endregion

#pragma region Find the validation layers within the available layers

	unsigned int layer_found_count = 0;
	for (size_t i = 0; i < validation_layers.size(); ++i)
	{
		for (size_t j = 0; j < available_layers.size(); ++j)
		{
			if (!strcmp(validation_layers[i], available_layers[j].layerName))
			{
				++layer_found_count;
				break;
			}
		}
	}

#pragma endregion

	if (layer_found_count == validation_layers.size())
		return true;

	return false;
}

bool VulkanObj::DeviceCompatible(VkPhysicalDevice device)
{
#pragma region Device Compatibilities Extras

	VkPhysicalDeviceProperties device_property;
 	vkGetPhysicalDeviceProperties(device, &device_property);
 
 	VkPhysicalDeviceFeatures device_features;
 	vkGetPhysicalDeviceFeatures(device, &device_features);

#pragma endregion

#pragma region Queue Family Indices Setup

	QueueFamilyIndices indices = FindQueueFamilies(device);
	bool extension_support = CheckDeviceExtensionSupport(device);

#pragma endregion

#pragma region Swapchain Extension Support Validation

	bool good_swapchain = false;
	if (extension_support)
	{
		SwapChainSupportDetails support = QuerySwapChainSupport(device);
		good_swapchain = !support.formats.empty() && !support.presentModes.empty();
	}

#pragma endregion

	return device_property.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& device_features.geometryShader
		&& device_features.samplerAnisotropy
		&& indices.IsComplete()
		&& extension_support
		&& good_swapchain;
}

bool VulkanObj::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
#pragma region Available Device Extension Setup

	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extension(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extension.data());

#pragma endregion

#pragma region Required Device Extension Setup

	std::set<std::string> required_extension(prv_DeviceExt.begin(), prv_DeviceExt.end());

	for (const auto& ext : available_extension)
	{
		required_extension.erase(ext.extensionName);
	}

#pragma endregion

	//Reason for empty check: If true, then we have all the required device extensions.
	//If False, then we do NOT have all the required device extension
	return required_extension.empty();
}



std::vector<const char*> VulkanObj::GetRequiredExtensions()
{
	uint32_t extensions_count;
	const char** extensions;
	extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
	std::vector<const char*> extensions_vector(extensions, extensions + extensions_count);

	if (validation_layers_enabled)
		extensions_vector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions_vector;
}

VulkanObj::SwapChainSupportDetails VulkanObj::QuerySwapChainSupport(VkPhysicalDevice device)
{
#pragma region Swapchain capabilities setup
	
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, prv_Surface, &details.capabilities);

#pragma endregion SwapChainSupportDetails is a struct created to get Surface Capabilities, Format and Present Mode

#pragma region Swapchain Present Mode and Format Setup
	
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, prv_Surface, &format_count, nullptr);
	
	if (format_count)
	{
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, prv_Surface, &format_count, details.formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, prv_Surface, &present_mode_count, nullptr);

	if (present_mode_count)
	{
		details.presentModes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, prv_Surface, &present_mode_count, details.presentModes.data());
	}

#pragma endregion

	return details;
}

VkSurfaceFormatKHR VulkanObj::SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{

#pragma region Find or Determine Best Surface Format
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (unsigned int i = 0; i < formats.size(); ++i)
	{
		if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return formats[i];
	}
#pragma endregion

	return formats[0];
}

VkPresentModeKHR VulkanObj::SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& mode)
{
#pragma region Find or Determine Best Surface Present Mode

	VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;

	for (int i = 0; i < mode.size(); ++i)
	{
		if (mode[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return mode[i];
		else if (mode[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			best_mode = mode[i];
	}

#pragma endregion
	return best_mode;
}

VkExtent2D VulkanObj::SelectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, unsigned short win_width, unsigned short win_height)
{
#pragma region Find or Determine Best Surface Swap Extent

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;
	else
	{
		return { win_width, win_height };
	}

#pragma endregion
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanObj::Callback_Debug(
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

VkResult VulkanObj::CustomVkCreateDebugUtilsMessengerEXT(
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

void VulkanObj::CustomVkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
{
	auto address = (PFN_vkDestroyDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (address)
		address(instance, debug_messenger, allocator);
}

std::vector<char> VulkanObj::ReadShaderFile(const std::string & filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		LOG("FAILED TO GET FILENAME");
		return std::vector<char>();
	}

	unsigned int file_size = (unsigned int)file.tellg();
	std::vector<char> content(file_size);

	file.seekg(0);
	file.read(content.data(), file_size);

	file.close();

	return content;
}

VkShaderModule VulkanObj::CreateShaderModule(const std::vector<char>& shader)
{
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = shader.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(shader.data());
	
	VkShaderModule shader_module;
	if (vkCreateShaderModule(prv_Device, &create_info, nullptr, &shader_module))
	{
		LOG("Failed to create Shader Module");
	}

	return shader_module;
}

VulkanObj::QueueFamilyIndices VulkanObj::FindQueueFamilies(VkPhysicalDevice device)
{
#pragma region Queue Family Setup

	QueueFamilyIndices indices;

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
	std::vector<VkQueueFamilyProperties>queue_family_property_list(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_property_list.data());

#pragma endregion

#pragma region Loop and set Graphics and Present Family

	for (unsigned int i = 0; i < queue_family_count; ++i)
	{
		if (queue_family_property_list[i].queueCount > 0 &&
			queue_family_property_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.Family_Graphics = i;

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, prv_Surface, &present_support);
			if (queue_family_property_list[i].queueCount > 0 && present_support) {
				indices.Family_Present = i;
			}
		}

		if (indices.IsComplete())
			break;
	}

#pragma endregion

	return indices;
}

void VulkanObj::CleanupSwapchain()
{
	if (prv_DepthBuffer)					vkDestroyImage(prv_Device, prv_DepthBuffer, nullptr);
	if (prv_DepthBufferMemory)				vkFreeMemory(prv_Device, prv_DepthBufferMemory, nullptr);
	if (prv_DepthBufferView)				vkDestroyImageView(prv_Device, prv_DepthBufferView, nullptr);

	for (unsigned int i = 0; i < prv_SwapchainFrameBuffers.size(); ++i)
		vkDestroyFramebuffer(prv_Device, prv_SwapchainFrameBuffers[i], nullptr);

	vkFreeCommandBuffers(prv_Device, prv_CommandPool, (uint32_t)prv_CommandBuffers.size(), prv_CommandBuffers.data());

	if (prv_GraphicsPipeline)		vkDestroyPipeline(prv_Device, prv_GraphicsPipeline, nullptr);
	if (prv_PipelineLayout)			vkDestroyPipelineLayout(prv_Device, prv_PipelineLayout, nullptr);
	if (prv_RenderPass)				vkDestroyRenderPass(prv_Device, prv_RenderPass, nullptr);

	for (unsigned int i = 0; i < prv_SwapchainImageViews.size(); ++i)
		vkDestroyImageView(prv_Device, prv_SwapchainImageViews[i], nullptr);

	if (prv_Swapchain)				vkDestroySwapchainKHR(prv_Device, prv_Swapchain, nullptr);
	
	for (uint32_t i = 0; i < prv_SwapchainImages.size(); ++i)
	{
		vkDestroyBuffer(prv_Device, prv_UniformBuffers[i], nullptr);
		vkFreeMemory(prv_Device, prv_UniformBuffersMemory[i], nullptr);
	}
}

uint32_t VulkanObj::FindMemoryType(uint32_t filter, VkMemoryPropertyFlags property_flags)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(prv_PhysicalDevice, &memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if ( (filter & (1 << i) ) &&
			(memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
			return i;
	}

	LOG("FAILED TO GET MEMORY TYPE!");

	return -1;
}

void VulkanObj::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags property_flags, VkBuffer & buffer, VkDeviceMemory & buffer_memory)
{
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;
	buffer_create_info.usage = usage_flags;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(prv_Device, &buffer_create_info, nullptr, &buffer))
	{
		LOG("Failed to create the vertex buffer!")
	}

	VkMemoryRequirements memory_requirement;
	vkGetBufferMemoryRequirements(prv_Device, buffer, &memory_requirement);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirement.size;
	memory_allocate_info.memoryTypeIndex = FindMemoryType(memory_requirement.memoryTypeBits, property_flags);

	if (vkAllocateMemory(prv_Device, &memory_allocate_info, nullptr, &buffer_memory))
	{
		LOG("FAILED TO ALLOCATE ")
	}

	vkBindBufferMemory(prv_Device, buffer, buffer_memory, 0);
}

void VulkanObj::CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer, VkDeviceSize device_size)
{
	VkCommandBuffer command_buffer = StartSingleCommand();

	VkBufferCopy copy_buffer = {};
	copy_buffer.srcOffset = 0;
	copy_buffer.dstOffset = 0;
	copy_buffer.size = device_size;
	vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &copy_buffer);

	EndSingleCommand(command_buffer);

}

void VulkanObj::CreateImage(VkExtent3D extent, uint32_t mip_levels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, VkImage& image, VkDeviceMemory& image_memory)
{
	VkImageCreateInfo image_create_info = {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.extent = extent;
	image_create_info.mipLevels = 1;
	image_create_info.arrayLayers = 1;
	image_create_info.format = format;
	image_create_info.tiling = tiling;
	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_create_info.usage = usage_flags;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create_info.flags = 0;

	if (vkCreateImage(prv_Device, &image_create_info, nullptr, &image))
	{
		LOG("Failed to Create Image!");
		image = nullptr;
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(prv_Device, image, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, memory_property_flags);

	if (vkAllocateMemory(prv_Device, &memory_allocate_info, nullptr, &image_memory))
	{
		LOG("Failed to Allocate Memory in Create Image!");
		image_memory = nullptr;
	}

	vkBindImageMemory(prv_Device, image, image_memory, 0);

}

VkImageView VulkanObj::CreateImageViewObject(VkImage image, VkFormat format, VkImageAspectFlags image_aspect_flags, uint32_t base_mip_level)
{
	VkImageViewCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = format;
	create_info.subresourceRange.aspectMask = image_aspect_flags;
	create_info.subresourceRange.baseMipLevel = base_mip_level;
	create_info.subresourceRange.levelCount = 1;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;

	VkImageView image_view;
	if (vkCreateImageView(prv_Device, &create_info, nullptr, &image_view))
	{
		LOG("Failed to Create Image View");
		return false;
	}

	return image_view;
}
