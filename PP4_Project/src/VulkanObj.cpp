#include "VulkanObj.h"

VulkanObj::VulkanObj() {}
VulkanObj::~VulkanObj() { cleanup(); }

bool VulkanObj::init(const char* title, GLFWwindow* window, unsigned short win_width, unsigned short win_height)
{
	prv_VkWindow.glfw_window = window;
	if (!vk_create_instance(title, prv_VkWindow.instance)) 
		{ LOG("Create Instance Has Failed!")			return false; }
	if (!vk_create_validation_layer(prv_VkWindow.instance, prv_VkValidationLayer.console)) 
		{ LOG("Create Validation Debugger Has Failed!")	return false; }
	if (!vk_create_surface(prv_VkWindow.instance, window, prv_VkWindow.surface)) 
		{ LOG("Create Surface Has Failed!")				return false; }
	if (!vk_set_physical_device(prv_VkWindow.instance, prv_VkWindow.surface, prv_VkDevices.physical_device)) 
		{ LOG("Set Physical Device Has Failed!")		return false; }
	if (!vk_create_logical_device(prv_VkDevices.physical_device, prv_VkWindow.surface, prv_VkDevices.logical_device, prv_VkDevices.graphics_queue, prv_VkDevices.present_queue)) 
		{ LOG("Create Logical Device Has Failed!")		return false; }
	if (!vk_create_swapchain(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_VkWindow.surface, win_width, win_height,
		prv_VkSwapchain.swapchain, prv_VkSwapchain.images, prv_VkSwapchain.format, prv_VkSwapchain.extent2D)) 
		{ LOG("Create Swap Chain Has Failed!")			return false; }
 	if (!vk_create_swapchain_image_view(prv_VkDevices.logical_device, prv_VkSwapchain.images, prv_VkSwapchain.format, prv_VkSwapchain.views))
		{ LOG("Create Image View Has Failed!")			return false; }
 	if (!vk_create_render_pass(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_VkSwapchain.format, prv_RenderGraphicsPipeline.render_pass))
		{ LOG("Create Render Pass Has Failed!")			return false; }
 	if (!vk_create_descriptor_set_layout(prv_VkDevices.logical_device, prv_Buffers.descriptor_set_layout)) 
		{ LOG("Create Descriptor Set Layout Failed!")	return false; }
 	if (!vk_create_graphics_pipeline(prv_VkDevices.logical_device, prv_VkSwapchain.extent2D, prv_Buffers.descriptor_set_layout, 
		prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline)) 
		{ LOG("Create Graphics Pipeline Has Failed!")	return false; }
 	if (!vk_create_command_pool(prv_VkDevices.physical_device, prv_VkWindow.surface, prv_VkDevices.logical_device, prv_CommandObjects.command_pool))
		{ LOG("Create Command Pool Has Failed!")		return false; }
	prv_VkSwapchain.depth = 1;
 	if (!vk_create_depth_buffer(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_CommandObjects.command_pool, prv_VkDevices.graphics_queue,
		prv_VkSwapchain.extent3D, prv_TextureObjects.depth_buffer, prv_TextureObjects.depth_buffer_memory, prv_TextureObjects.depth_buffer_view)) 
		{ LOG("Create Depth Resources Has Failed!")		return false; }
 	if (!vk_create_swapchain_frame_buffer(prv_VkDevices.logical_device, prv_RenderGraphicsPipeline.render_pass, prv_VkSwapchain.views,
		prv_VkSwapchain.extent2D, prv_TextureObjects.depth_buffer_view, prv_VkSwapchain.frame_buffers))
		{ LOG("Create Frame Buffers Has Failed")		return false; }
 	if (!vk_create_texture_image(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_CommandObjects.command_pool, prv_VkDevices.graphics_queue,
		prv_ObjectList, { prv_ObjectList[0].texture->width,prv_ObjectList[0].texture->height, 1 }, prv_TextureObjects.image, prv_TextureObjects.image_memory))
		{ LOG("Create Texture Image Has Failed!")		return false; }
 	if (!vk_create_texture_image_view(prv_VkDevices.logical_device, prv_TextureObjects.image, VK_IMAGE_ASPECT_COLOR_BIT, prv_ObjectList[0].texture->mip_levels, prv_TextureObjects.image_view))
		{ LOG("Create Texture Image View Has Failed!")	return false; }
 	if (!vk_create_texture_sampler(prv_VkDevices.logical_device, prv_TextureObjects.sampler))
		{ LOG("Create Texture Image View Has Failed!")	return false; }
 	if (!vk_create_vertex_buffer(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_CommandObjects.command_pool, prv_VkDevices.graphics_queue,
		prv_ObjectList, prv_Buffers.vertex, prv_Buffers.vertex_memory))
		{ LOG("Creating Vertex Buffer Has Failed!")		return false; }
	if (!vk_create_index_buffer(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_CommandObjects.command_pool, prv_VkDevices.graphics_queue,
		prv_ObjectList, prv_Buffers.index, prv_Buffers.index_memory))
		{ LOG("Creating Index Buffer Has Failed!")		return false; }
 	if (!vk_create_uniform_buffer(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_VkSwapchain.images, 
		prv_Buffers.uniform, prv_Buffers.uniform_memory))
		{ LOG("Create Uniform Buffer Has Failed!")		return false; }
 	if (!vk_create_descriptor_pool(prv_VkDevices.logical_device, prv_VkSwapchain.images, prv_Buffers.descriptor_pool))
		{ LOG("Create Descriptor Pool Has Failed!")		return false; }
 	if (!vk_create_descriptor_sets(prv_VkDevices.logical_device, prv_VkSwapchain.images, prv_Buffers.descriptor_pool, prv_Buffers.uniform, 
		prv_TextureObjects.image_view, prv_TextureObjects.sampler, prv_Buffers.descriptor_set_layout, prv_Buffers.descriptor_sets))
		{ LOG("Create Desriptor Sets Has Failed!")		return false; }
 	if (!vk_create_command_buffer(prv_VkDevices.logical_device, prv_CommandObjects.command_pool, prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
		prv_RenderGraphicsPipeline.graphics_pipeline, prv_VkSwapchain.extent2D, prv_VkSwapchain.frame_buffers, prv_Buffers.descriptor_sets, prv_Buffers.vertex, 0, prv_Buffers.index,
		prv_ObjectList, prv_CommandObjects.command_buffers)) 
		{ LOG("Create Command Buffer Has Failed!")		return false; }
 	if (!vk_sync_semaphore_and_fences(prv_VkDevices.logical_device, prv_SemaphoreAndFences.image_available_semaphores, prv_SemaphoreAndFences.render_finished_semaphores,
		prv_SemaphoreAndFences.fences)) 
	{ LOG("Create Semaphore Has Failed!")			return false; }
	return true;
}

void VulkanObj::idle_device()
{
	vkDeviceWaitIdle(prv_VkDevices.logical_device);
}

void VulkanObj::reset_swapchain(unsigned short win_width, unsigned short win_height)
{
	vkDeviceWaitIdle(prv_VkDevices.logical_device);

	CleanupSwapchain();

	if (!vk_create_swapchain(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_VkWindow.surface, win_width, win_height,
		prv_VkSwapchain.swapchain, prv_VkSwapchain.images, prv_VkSwapchain.format, prv_VkSwapchain.extent2D))
		{ LOG("Create Swap Chain Has Failed!") }
 	if (!vk_create_swapchain_image_view(prv_VkDevices.logical_device, prv_VkSwapchain.images, prv_VkSwapchain.format, prv_VkSwapchain.views))
		{ LOG("Create Image View Has Failed!") }
 	if (!vk_create_render_pass(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_VkSwapchain.format, prv_RenderGraphicsPipeline.render_pass))
		{ LOG("Create Render Pass Has Failed!") }
 	if (!vk_create_graphics_pipeline(prv_VkDevices.logical_device, prv_VkSwapchain.extent2D, prv_Buffers.descriptor_set_layout, 
		prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline)) 
		{ LOG("Create Graphics Pipeline Has Failed!") }
 	if (!vk_create_depth_buffer(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_CommandObjects.command_pool, prv_VkDevices.graphics_queue,
		prv_VkSwapchain.extent3D, prv_TextureObjects.depth_buffer, prv_TextureObjects.depth_buffer_memory, prv_TextureObjects.depth_buffer_view)) 
		{ LOG("Create Depth Resources Has Failed!") }
 	if (!vk_create_swapchain_frame_buffer(prv_VkDevices.logical_device, prv_RenderGraphicsPipeline.render_pass, prv_VkSwapchain.views,
		prv_VkSwapchain.extent2D, prv_TextureObjects.depth_buffer_view, prv_VkSwapchain.frame_buffers))
		{ LOG("Create Frame Buffers Has Failed") }
 	if (!vk_create_uniform_buffer(prv_VkDevices.physical_device, prv_VkDevices.logical_device, prv_VkSwapchain.images, 
		prv_Buffers.uniform, prv_Buffers.uniform_memory))
		{ LOG("Create Uniform Buffer Has Failed!") }
 	if (!vk_create_descriptor_pool(prv_VkDevices.logical_device, prv_VkSwapchain.images, prv_Buffers.descriptor_pool))
		{ LOG("Create Descriptor Pool Has Failed!") }
 	if (!vk_create_descriptor_sets(prv_VkDevices.logical_device, prv_VkSwapchain.images, prv_Buffers.descriptor_pool, prv_Buffers.uniform, 
		prv_TextureObjects.image_view, prv_TextureObjects.sampler, prv_Buffers.descriptor_set_layout, prv_Buffers.descriptor_sets))
		{ LOG("Create Desriptor Sets Has Failed!") }
 	if (!vk_create_command_buffer(prv_VkDevices.logical_device, prv_CommandObjects.command_pool, prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
		prv_RenderGraphicsPipeline.graphics_pipeline, prv_VkSwapchain.extent2D, prv_VkSwapchain.frame_buffers, prv_Buffers.descriptor_sets, prv_Buffers.vertex, 0, prv_Buffers.index,
		prv_ObjectList, prv_CommandObjects.command_buffers)) 
		{ LOG("Create Command Buffer Has Failed!") }
}

void VulkanObj::setup_object_list(uint32_t size)
{
	prv_ObjectList.resize(size);
}

void VulkanObj::add_to_object_list(const Object3D & object)
{
	prv_ObjectList.push_back(object);
}

void VulkanObj::CleanupSwapchain()
{
	if (prv_TextureObjects.depth_buffer)					vkDestroyImage(prv_VkDevices.logical_device, prv_TextureObjects.depth_buffer, nullptr);
	if (prv_TextureObjects.depth_buffer_memory)				vkFreeMemory(prv_VkDevices.logical_device, prv_TextureObjects.depth_buffer_memory, nullptr);
	if (prv_TextureObjects.depth_buffer_view)				vkDestroyImageView(prv_VkDevices.logical_device, prv_TextureObjects.depth_buffer_view, nullptr);

	for (unsigned int i = 0; i < prv_VkSwapchain.frame_buffers.size(); ++i)
		vkDestroyFramebuffer(prv_VkDevices.logical_device, prv_VkSwapchain.frame_buffers[i], nullptr);


	vkFreeCommandBuffers(prv_VkDevices.logical_device, prv_CommandObjects.command_pool, (uint32_t)prv_CommandObjects.command_buffers.size(), prv_CommandObjects.command_buffers.data());

	if (prv_Buffers.descriptor_pool)							vkDestroyDescriptorPool(prv_VkDevices.logical_device, prv_Buffers.descriptor_pool, nullptr);
	if (prv_RenderGraphicsPipeline.graphics_pipeline)			vkDestroyPipeline(prv_VkDevices.logical_device, prv_RenderGraphicsPipeline.graphics_pipeline, nullptr);
	if (prv_RenderGraphicsPipeline.graphics_pipeline_layout)	vkDestroyPipelineLayout(prv_VkDevices.logical_device, prv_RenderGraphicsPipeline.graphics_pipeline_layout, nullptr);
	if (prv_RenderGraphicsPipeline.render_pass)					vkDestroyRenderPass(prv_VkDevices.logical_device, prv_RenderGraphicsPipeline.render_pass, nullptr);

	for (unsigned int i = 0; i < prv_VkSwapchain.images.size(); ++i)
		vkDestroyImageView(prv_VkDevices.logical_device, prv_VkSwapchain.views[i], nullptr);

	if (prv_VkSwapchain.swapchain)								vkDestroySwapchainKHR(prv_VkDevices.logical_device, prv_VkSwapchain.swapchain, nullptr);

	for (uint32_t i = 0; i < prv_VkSwapchain.images.size(); ++i)
	{
		vkDestroyBuffer(prv_VkDevices.logical_device, prv_Buffers.uniform[i], nullptr);
		vkFreeMemory(prv_VkDevices.logical_device, prv_Buffers.uniform_memory[i], nullptr);
	}
}

void VulkanObj::cleanup()
{
	for (unsigned int i = 0; i < MAX_FRAMES_FLIGHT; ++i)
	{
		if (prv_SemaphoreAndFences.render_finished_semaphores[i])	vkDestroySemaphore(prv_VkDevices.logical_device, prv_SemaphoreAndFences.render_finished_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.image_available_semaphores[i])	vkDestroySemaphore(prv_VkDevices.logical_device, prv_SemaphoreAndFences.image_available_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.fences[i])						vkDestroyFence(prv_VkDevices.logical_device, prv_SemaphoreAndFences.fences[i], nullptr);
	}

	CleanupSwapchain();

	if (prv_Buffers.descriptor_set_layout)	vkDestroyDescriptorSetLayout(prv_VkDevices.logical_device, prv_Buffers.descriptor_set_layout, nullptr);
	if (prv_TextureObjects.sampler)			vkDestroySampler(prv_VkDevices.logical_device, prv_TextureObjects.sampler, nullptr);
	if (prv_TextureObjects.image_view)		vkDestroyImageView(prv_VkDevices.logical_device, prv_TextureObjects.image_view, nullptr);

	if (prv_TextureObjects.image)			vkDestroyImage(prv_VkDevices.logical_device, prv_TextureObjects.image, nullptr);
	if (prv_TextureObjects.image_memory)	vkFreeMemory(prv_VkDevices.logical_device, prv_TextureObjects.image_memory, nullptr);

	if (prv_Buffers.index)					vkDestroyBuffer(prv_VkDevices.logical_device, prv_Buffers.index, nullptr);
	if (prv_Buffers.index_memory)			vkFreeMemory(prv_VkDevices.logical_device, prv_Buffers.index_memory, nullptr);

	if (prv_Buffers.vertex)					vkDestroyBuffer(prv_VkDevices.logical_device, prv_Buffers.vertex, nullptr);
	if (prv_Buffers.vertex_memory)			vkFreeMemory(prv_VkDevices.logical_device, prv_Buffers.vertex_memory, nullptr);

	if (prv_CommandObjects.command_pool)	vkDestroyCommandPool(prv_VkDevices.logical_device, prv_CommandObjects.command_pool, nullptr);
	if (prv_VkDevices.logical_device)		vkDestroyDevice(prv_VkDevices.logical_device, nullptr);
	if (prv_VkWindow.surface)				vkDestroySurfaceKHR(prv_VkWindow.instance, prv_VkWindow.surface, nullptr);
	if (VkObj_ValidationLayer::validation_layers_enabled)	
											vk_destroy_debug_utils_messenger_ext(prv_VkWindow.instance, prv_VkValidationLayer.console, nullptr);
	if (prv_VkWindow.instance)				vkDestroyInstance(prv_VkWindow.instance, nullptr);
}

void VulkanObj::draw_frames()
{
	vkWaitForFences(prv_VkDevices.logical_device, 1, &prv_SemaphoreAndFences.fences[prv_Frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t image_index;
	VkResult frame_result = vkAcquireNextImageKHR(prv_VkDevices.logical_device, prv_VkSwapchain.swapchain, std::numeric_limits<uint64_t>::max(),
		prv_SemaphoreAndFences.image_available_semaphores[prv_Frame], VK_NULL_HANDLE, &image_index);
	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR)
		return;
	else if (frame_result != VK_SUCCESS && frame_result != VK_SUBOPTIMAL_KHR)
	{
		LOG("CANNOT DRAW FRAME!!!!!");
		return;
	}

	vk_update_uniform_buffer(prv_VkDevices.logical_device, prv_VkSwapchain.extent3D, image_index, prv_Buffers.uniform_memory);

	VkSemaphore wait_semaphores[] = { prv_SemaphoreAndFences.image_available_semaphores[prv_Frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphore[] = { prv_SemaphoreAndFences.render_finished_semaphores[prv_Frame] };


	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &prv_CommandObjects.command_buffers[image_index];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;

	vkResetFences(prv_VkDevices.logical_device, 1, &prv_SemaphoreAndFences.fences[prv_Frame]);

	if (vkQueueSubmit(prv_VkDevices.graphics_queue, 1, &submit_info, prv_SemaphoreAndFences.fences[prv_Frame]))
	{
		LOG("Failed to Submit Queue");
	}

	VkSwapchainKHR swapchains[] = { prv_VkSwapchain.swapchain };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphore; //<--- I don't get it! why signal
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	frame_result = vkQueuePresentKHR(prv_VkDevices.present_queue, &present_info);

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
