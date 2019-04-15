#include "VulkanObj.h"

VulkanObj::VulkanObj() {}
VulkanObj::~VulkanObj() { cleanup(); }

bool VulkanObj::init(const char* title, GLFWwindow* window, unsigned short win_width, unsigned short win_height)
{
	prv_Window.glfw_window = window;
	prv_Swapchain.depth = 1;

	CHECK_RESULT(vk_create_instance(title, prv_Window.instance), "Create Instance Has Failed!");
	CHECK_RESULT(vk_create_validation_layer(prv_Window.instance, prv_ValidationLayer.console), "Create Validation Debugger Has Failed!");
	CHECK_RESULT(vk_create_surface(prv_Window.instance, window, prv_Window.surface), "Create Surface Has Failed!");
	CHECK_RESULT(vk_set_physical_device(prv_Window.instance, prv_Window.surface, prv_Devices.physical, 
		prv_Texture.msaa_sample), "Set Physical Device Has Failed!");
	CHECK_RESULT(vk_create_logical_device(prv_Devices.physical, prv_Window.surface, prv_Devices.logical, 
		prv_Devices.q_graphics, prv_Devices.q_present), "Create Logical Device Has Failed!");
	CHECK_RESULT(vk_create_swapchain(prv_Devices.physical, prv_Devices.logical, prv_Window.surface, win_width, win_height,
		prv_Swapchain.swapchain, prv_Swapchain.images, prv_Swapchain.format, prv_Swapchain.extent2D), "Create Swap Chain Has Failed!");
	CHECK_RESULT(vk_create_swapchain_image_view(prv_Devices.logical, prv_Swapchain.images, prv_Swapchain.format,
		prv_Swapchain.views), "Create Image View Has Failed!");
	CHECK_RESULT(vk_create_render_pass(prv_Devices.physical, prv_Devices.logical, prv_Swapchain.format,
		prv_Texture.msaa_sample, prv_RenderGraphicsPipeline.render_pass), "Create Render Pass Has Failed!");
	CHECK_RESULT(vk_create_descriptor_set_layout(prv_Devices.logical, prv_Buffers.descriptor_set_layout), "Create Descriptor Set Layout Failed!");
	CHECK_RESULT(vk_create_graphics_pipeline(prv_Devices.logical, prv_Swapchain.extent2D, prv_Buffers.descriptor_set_layout,
		prv_Texture.msaa_sample, prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
		prv_RenderGraphicsPipeline.graphics_pipeline), "Create Graphics Pipeline Has Failed!");
	CHECK_RESULT(vk_create_command_pool(prv_Devices.physical, prv_Window.surface, prv_Devices.logical, 
		prv_Command.command_pool), "Create Command Pool Has Failed!");
	CHECK_RESULT(vk_create_color_buffer(prv_Devices.physical, prv_Devices.logical, prv_Command.command_pool, 
		prv_Devices.q_graphics, 1, prv_Texture.msaa_sample, prv_Swapchain.extent3D, prv_Swapchain.format, 
		prv_Texture.color_image, prv_Texture.color_image_memory, prv_Texture.color_image_view), "Create Color Buffer Has Failed!");
	CHECK_RESULT(vk_create_depth_buffer(prv_Devices.physical, prv_Devices.logical, prv_Command.command_pool, 
		prv_Devices.q_graphics, prv_Swapchain.extent3D, prv_Texture.msaa_sample, prv_Texture.depth_buffer, 
		prv_Texture.depth_buffer_memory, prv_Texture.depth_buffer_view), "Create Depth Resources Has Failed!");
	CHECK_RESULT(vk_create_swapchain_frame_buffer(prv_Devices.logical, prv_RenderGraphicsPipeline.render_pass, 
		prv_Swapchain.views, prv_Swapchain.extent2D, prv_Texture.color_image_view, prv_Texture.depth_buffer_view, 
		prv_Swapchain.frame_buffers), "Create Frame Buffers Has Failed");
	CHECK_RESULT(vk_create_texture_image(prv_Devices.physical, prv_Devices.logical, prv_Command.command_pool, 
		prv_Devices.q_graphics, prv_ObjectList, { prv_ObjectList[0].texture->width,prv_ObjectList[0].texture->height, 1 }, 
		prv_Texture.msaa_sample, prv_Texture.texture_image, prv_Texture.texture_image_memory), "Create Texture Image Has Failed!");
	CHECK_RESULT(vk_create_texture_image_view(prv_Devices.logical, prv_Texture.texture_image, VK_IMAGE_ASPECT_COLOR_BIT, 
		prv_ObjectList[0].texture->mip_levels, prv_Texture.texture_image_view), "Create Texture Image View Has Failed!");
	CHECK_RESULT(vk_create_texture_sampler(prv_Devices.logical, prv_Texture.sampler), "Create Texture Image View Has Failed!");
	CHECK_RESULT(vk_create_vertex_buffer(prv_Devices.physical, prv_Devices.logical, prv_Command.command_pool,
		prv_Devices.q_graphics, prv_ObjectList, prv_Buffers.vertex, prv_Buffers.vertex_memory), "Creating Vertex Buffer Has Failed!");
	CHECK_RESULT(vk_create_index_buffer(prv_Devices.physical, prv_Devices.logical, prv_Command.command_pool, 
		prv_Devices.q_graphics, prv_ObjectList, prv_Buffers.index, prv_Buffers.index_memory), "Creating Index Buffer Has Failed!");
	CHECK_RESULT(vk_create_uniform_buffer(prv_Devices.physical, prv_Devices.logical, prv_Swapchain.images,
		prv_Buffers.uniform, prv_Buffers.uniform_memory), "Create Uniform Buffer Has Failed!");
	CHECK_RESULT(vk_create_descriptor_pool(prv_Devices.logical, prv_Swapchain.images, prv_Buffers.descriptor_pool), "Create Descriptor Pool Has Failed!");
	CHECK_RESULT(vk_create_descriptor_sets(prv_Devices.logical, prv_Swapchain.images, prv_Buffers.descriptor_pool, 
		prv_Buffers.uniform, prv_Texture.texture_image_view, prv_Texture.sampler, prv_Buffers.descriptor_set_layout,
		prv_Buffers.descriptor_sets), "Create Desriptor Sets Has Failed!");
	CHECK_RESULT(vk_create_command_buffer(prv_Devices.logical, prv_Command.command_pool, prv_RenderGraphicsPipeline.render_pass,
		prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline,
		prv_Swapchain.extent2D, prv_Swapchain.frame_buffers, prv_Buffers.descriptor_sets, prv_Buffers.vertex, 0, 
		prv_Buffers.index, prv_ObjectList, prv_Command.command_buffers), "Create Command Buffer Has Failed!");
	CHECK_RESULT(vk_sync_semaphore_and_fences(prv_Devices.logical, prv_SemaphoreAndFences.image_available_semaphores,
		prv_SemaphoreAndFences.render_finished_semaphores, prv_SemaphoreAndFences.fences), "Create Semaphore Has Failed!");

	return true;
}

void VulkanObj::idle_device()
{
	vkDeviceWaitIdle(prv_Devices.logical);
}

void VulkanObj::reset_swapchain(unsigned short win_width, unsigned short win_height)
{
	vkDeviceWaitIdle(prv_Devices.logical);

	CleanupSwapchain();

	CHECK_RESULT_NO_RETURN(vk_create_swapchain(prv_Devices.physical, prv_Devices.logical, prv_Window.surface, win_width, win_height,
		prv_Swapchain.swapchain, prv_Swapchain.images, prv_Swapchain.format, prv_Swapchain.extent2D), "Create Swap Chain Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_swapchain_image_view(prv_Devices.logical, prv_Swapchain.images, prv_Swapchain.format,
		prv_Swapchain.views), "Create Image View Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_render_pass(prv_Devices.physical, prv_Devices.logical, prv_Swapchain.format,
		prv_Texture.msaa_sample, prv_RenderGraphicsPipeline.render_pass), "Create Render Pass Has Failed!");

	CHECK_RESULT_NO_RETURN(vk_create_graphics_pipeline(prv_Devices.logical, prv_Swapchain.extent2D, prv_Buffers.descriptor_set_layout,
		prv_Texture.msaa_sample, prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
		prv_RenderGraphicsPipeline.graphics_pipeline), "Create Graphics Pipeline Has Failed!");

	CHECK_RESULT_NO_RETURN(vk_create_color_buffer(prv_Devices.physical, prv_Devices.logical, prv_Command.command_pool,
		prv_Devices.q_graphics, 1, prv_Texture.msaa_sample, prv_Swapchain.extent3D, prv_Swapchain.format,
		prv_Texture.color_image, prv_Texture.color_image_memory, prv_Texture.color_image_view), "Create Color Buffer Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_depth_buffer(prv_Devices.physical, prv_Devices.logical, prv_Command.command_pool,
		prv_Devices.q_graphics, prv_Swapchain.extent3D, prv_Texture.msaa_sample, prv_Texture.depth_buffer,
		prv_Texture.depth_buffer_memory, prv_Texture.depth_buffer_view), "Create Depth Resources Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_swapchain_frame_buffer(prv_Devices.logical, prv_RenderGraphicsPipeline.render_pass,
		prv_Swapchain.views, prv_Swapchain.extent2D, prv_Texture.color_image_view, prv_Texture.depth_buffer_view,
		prv_Swapchain.frame_buffers), "Create Frame Buffers Has Failed");

	CHECK_RESULT_NO_RETURN(vk_create_uniform_buffer(prv_Devices.physical, prv_Devices.logical, prv_Swapchain.images,
		prv_Buffers.uniform, prv_Buffers.uniform_memory), "Create Uniform Buffer Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_descriptor_pool(prv_Devices.logical, prv_Swapchain.images, prv_Buffers.descriptor_pool), "Create Descriptor Pool Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_descriptor_sets(prv_Devices.logical, prv_Swapchain.images, prv_Buffers.descriptor_pool,
		prv_Buffers.uniform, prv_Texture.texture_image_view, prv_Texture.sampler, prv_Buffers.descriptor_set_layout,
		prv_Buffers.descriptor_sets), "Create Desriptor Sets Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_command_buffer(prv_Devices.logical, prv_Command.command_pool, prv_RenderGraphicsPipeline.render_pass,
		prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline,
		prv_Swapchain.extent2D, prv_Swapchain.frame_buffers, prv_Buffers.descriptor_sets, prv_Buffers.vertex, 0,
		prv_Buffers.index, prv_ObjectList, prv_Command.command_buffers), "Create Command Buffer Has Failed!");

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
	if (prv_Texture.color_image_view)		vkDestroyImageView(prv_Devices.logical, prv_Texture.color_image_view, nullptr);
	if (prv_Texture.color_image)			vkDestroyImage(prv_Devices.logical, prv_Texture.color_image, nullptr);
	if (prv_Texture.color_image_memory)		vkFreeMemory(prv_Devices.logical, prv_Texture.color_image_memory, nullptr);
	if (prv_Texture.depth_buffer_view)		vkDestroyImageView(prv_Devices.logical, prv_Texture.depth_buffer_view, nullptr);
	if (prv_Texture.depth_buffer)			vkDestroyImage(prv_Devices.logical, prv_Texture.depth_buffer, nullptr);
	if (prv_Texture.depth_buffer_memory)	vkFreeMemory(prv_Devices.logical, prv_Texture.depth_buffer_memory, nullptr);

	for (unsigned int i = 0; i < prv_Swapchain.frame_buffers.size(); ++i)
		vkDestroyFramebuffer(prv_Devices.logical, prv_Swapchain.frame_buffers[i], nullptr);


	vkFreeCommandBuffers(prv_Devices.logical, prv_Command.command_pool, CAST(uint32_t, prv_Command.command_buffers.size()), prv_Command.command_buffers.data());

	if (prv_Buffers.descriptor_pool)							vkDestroyDescriptorPool(prv_Devices.logical, prv_Buffers.descriptor_pool, nullptr);
	if (prv_RenderGraphicsPipeline.graphics_pipeline)			vkDestroyPipeline(prv_Devices.logical, prv_RenderGraphicsPipeline.graphics_pipeline, nullptr);
	if (prv_RenderGraphicsPipeline.graphics_pipeline_layout)	vkDestroyPipelineLayout(prv_Devices.logical, prv_RenderGraphicsPipeline.graphics_pipeline_layout, nullptr);
	if (prv_RenderGraphicsPipeline.render_pass)					vkDestroyRenderPass(prv_Devices.logical, prv_RenderGraphicsPipeline.render_pass, nullptr);

	for (unsigned int i = 0; i < prv_Swapchain.images.size(); ++i)
		vkDestroyImageView(prv_Devices.logical, prv_Swapchain.views[i], nullptr);

	if (prv_Swapchain.swapchain)								vkDestroySwapchainKHR(prv_Devices.logical, prv_Swapchain.swapchain, nullptr);

	for (uint32_t i = 0; i < prv_Swapchain.images.size(); ++i)
	{
		vkDestroyBuffer(prv_Devices.logical, prv_Buffers.uniform[i], nullptr);
		vkFreeMemory(prv_Devices.logical, prv_Buffers.uniform_memory[i], nullptr);
	}
}

void VulkanObj::cleanup()
{
	CleanupSwapchain();

	for (unsigned int i = 0; i < MAX_FRAMES_FLIGHT; ++i)
	{
		if (prv_SemaphoreAndFences.render_finished_semaphores[i])	vkDestroySemaphore(prv_Devices.logical, prv_SemaphoreAndFences.render_finished_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.image_available_semaphores[i])	vkDestroySemaphore(prv_Devices.logical, prv_SemaphoreAndFences.image_available_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.fences[i])						vkDestroyFence(prv_Devices.logical, prv_SemaphoreAndFences.fences[i], nullptr);
	}

	if (prv_Buffers.descriptor_set_layout)	vkDestroyDescriptorSetLayout(prv_Devices.logical, prv_Buffers.descriptor_set_layout, nullptr);
	if (prv_Texture.sampler)				vkDestroySampler(prv_Devices.logical, prv_Texture.sampler, nullptr);
	if (prv_Texture.texture_image_view)		vkDestroyImageView(prv_Devices.logical, prv_Texture.texture_image_view, nullptr);

	if (prv_Texture.texture_image)			vkDestroyImage(prv_Devices.logical, prv_Texture.texture_image, nullptr);
	if (prv_Texture.texture_image_memory)	vkFreeMemory(prv_Devices.logical, prv_Texture.texture_image_memory, nullptr);

	if (prv_Buffers.index)					vkDestroyBuffer(prv_Devices.logical, prv_Buffers.index, nullptr);
	if (prv_Buffers.index_memory)			vkFreeMemory(prv_Devices.logical, prv_Buffers.index_memory, nullptr);

	if (prv_Buffers.vertex)					vkDestroyBuffer(prv_Devices.logical, prv_Buffers.vertex, nullptr);
	if (prv_Buffers.vertex_memory)			vkFreeMemory(prv_Devices.logical, prv_Buffers.vertex_memory, nullptr);

	if (prv_Command.command_pool)			vkDestroyCommandPool(prv_Devices.logical, prv_Command.command_pool, nullptr);
	if (prv_Devices.logical)				vkDestroyDevice(prv_Devices.logical, nullptr);
	if (prv_Window.surface)					vkDestroySurfaceKHR(prv_Window.instance, prv_Window.surface, nullptr);
	if (VkObj_ValidationLayer::validation_layers_enabled)	
											vk_destroy_debug_utils_messenger_ext(prv_Window.instance, prv_ValidationLayer.console, nullptr);
	if (prv_Window.instance)				vkDestroyInstance(prv_Window.instance, nullptr);
}

void VulkanObj::draw_frames()
{
	vkWaitForFences(prv_Devices.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame], VK_TRUE, MAX_UINT64);

	uint32_t image_index;
	VkResult frame_result = vkAcquireNextImageKHR(prv_Devices.logical, prv_Swapchain.swapchain, MAX_UINT64,
		prv_SemaphoreAndFences.image_available_semaphores[prv_Frame], VK_NULL_HANDLE, &image_index);
	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR)
		return;
	else if (frame_result != VK_SUCCESS && frame_result != VK_SUBOPTIMAL_KHR)
	{
		LOG("CANNOT DRAW FRAME!!!!!");
		return;
	}

	vk_update_uniform_buffer(prv_Devices.logical, prv_Swapchain.extent3D, image_index, prv_Buffers.uniform_memory);

	VkSemaphore wait_semaphores[] = { prv_SemaphoreAndFences.image_available_semaphores[prv_Frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphore[] = { prv_SemaphoreAndFences.render_finished_semaphores[prv_Frame] };


	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &prv_Command.command_buffers[image_index];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;

	vkResetFences(prv_Devices.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame]);

	if (vkQueueSubmit(prv_Devices.q_graphics, 1, &submit_info, prv_SemaphoreAndFences.fences[prv_Frame]))
	{
		LOG("Failed to Submit Queue");
	}

	VkSwapchainKHR swapchains[] = { prv_Swapchain.swapchain };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphore; //<--- I don't get it! why signal
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	frame_result = vkQueuePresentKHR(prv_Devices.q_present, &present_info);

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
