#include "VulkanObj.h"

VulkanObj::VulkanObj() {}
VulkanObj::~VulkanObj() { cleanup(); }

bool VulkanObj::init(const char* title, GLFWwindow* window, unsigned short win_width, unsigned short win_height)
{
	prv_Window.window = window;
	prv_Swapchain_old.depth = 1;

	//Create Instance, Validation Layer, and Surface
	prv_Window.init(title);

	//Create Physical and Logical Device
	prv_Devices.init();

	//Create Semaphores
#if TRANSITION
	CHECK_RESULT(vk_create_semaphore(prv_Devices.logical, prv_SemaphoreAndFences.image_available_semaphores,
		prv_SemaphoreAndFences.render_finished_semaphores), "Create Semaphore Has Failed!");
#else
	CHECK_RESULT(vk_sync_semaphore_and_fences(prv_Devices.logical, prv_SemaphoreAndFences.image_available_semaphores,
		prv_SemaphoreAndFences.render_finished_semaphores, prv_SemaphoreAndFences.fences), "Create Semaphore Has Failed!");
#endif

	//	TODO: LOOK INTO QUERY POOL
	prv_Pools.CreateQueryPool();
	prv_Pools.CreateCommandPool();

	//	TODO: INVESTIGATE COMMAND BUFFERS MORE
#if TRANSITION
	prv_Buffers.CreateCommandBuffer();
#endif

//	TODO: START ALLOCATOR AND STAGING MANAGER
	prv_Allocator.init(prv_Devices.physical, prv_Devices.logical);
	prv_StagingManager.init(prv_Window.surface, prv_Devices.physical, prv_Devices.logical, prv_Devices.q_graphics);

	CHECK_RESULT(vk_create_swapchain(prv_Devices.physical, prv_Devices.logical, prv_Window.surface, win_width, win_height,
		prv_Swapchain_old.swapchain, prv_Swapchain_old.images, prv_Swapchain_old.format, prv_Swapchain_old.extent2D), "Create Swap Chain Has Failed!");
	CHECK_RESULT(vk_create_swapchain_image_view(prv_Devices.logical, prv_Swapchain_old.images, prv_Swapchain_old.format,
		prv_Swapchain_old.views), "Create Image View Has Failed!");


	CHECK_RESULT(vk_create_render_pass(prv_Devices.physical, prv_Devices.logical, prv_Swapchain_old.format,
		prv_Devices.msaa_support, prv_RenderGraphicsPipeline.render_pass), "Create Render Pass Has Failed!");
	CHECK_RESULT(vk_create_color_buffer(prv_Devices.physical, prv_Devices.logical, prv_Pools.command,
		prv_Devices.q_graphics, 1, prv_Devices.msaa_support, prv_Swapchain_old.extent3D, prv_Swapchain_old.format,
		prv_Texture.color_image, prv_Texture.color_image_memory, prv_Texture.color_image_view), "Create Color Buffer Has Failed!");
	CHECK_RESULT(vk_create_depth_buffer(prv_Devices.physical, prv_Devices.logical, prv_Pools.command,
		prv_Devices.q_graphics, prv_Swapchain_old.extent3D, prv_Devices.msaa_support, prv_Texture.depth_buffer,
		prv_Texture.depth_buffer_memory, prv_Texture.depth_buffer_view), "Create Depth Resources Has Failed!");


	CHECK_RESULT(vk_create_swapchain_frame_buffer(prv_Devices.logical, prv_RenderGraphicsPipeline.render_pass,
		prv_Swapchain_old.views, prv_Swapchain_old.extent2D, prv_Texture.color_image_view, prv_Texture.depth_buffer_view,
		prv_Swapchain_old.frame_buffers), "Create Frame Buffers Has Failed");


	//TODO: Continue Tutorial for RenderProgManager
	//TODO: Continue Tutorial for Vertex Cache

	//[These don't exist]
	CHECK_RESULT(vk_create_descriptor_set_layout(prv_Devices.logical, prv_Buffers_old.descriptor_set_layout), "Create Descriptor Set Layout Failed!");
	CHECK_RESULT(vk_create_graphics_pipeline(prv_Devices.logical, prv_Swapchain_old.extent2D, prv_Buffers_old.descriptor_set_layout,
		prv_Devices.msaa_support, prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
		prv_RenderGraphicsPipeline.graphics_pipeline), "Create Graphics Pipeline Has Failed!");
	CHECK_RESULT(vk_create_texture_image(prv_Devices.physical, prv_Devices.logical, prv_Pools.command, 
		prv_Devices.q_graphics, prv_ObjectList, { prv_ObjectList[0].texture->width,prv_ObjectList[0].texture->height, 1 }, 
		prv_Devices.msaa_support, prv_Texture.texture_image, prv_Texture.texture_image_memory), "Create Texture Image Has Failed!");
	CHECK_RESULT(vk_create_texture_image_view(prv_Devices.logical, prv_Texture.texture_image, VK_IMAGE_ASPECT_COLOR_BIT, 
		prv_ObjectList[0].texture->mip_levels, prv_Texture.texture_image_view), "Create Texture Image View Has Failed!");
	CHECK_RESULT(vk_create_texture_sampler(prv_Devices.logical, prv_Texture.sampler), "Create Texture Image View Has Failed!");
	CHECK_RESULT(vk_create_vertex_buffer(prv_Devices.physical, prv_Devices.logical, prv_Pools.command,
		prv_Devices.q_graphics, prv_ObjectList, prv_Buffers_old.vertex, prv_Buffers_old.vertex_memory), "Creating Vertex Buffer Has Failed!");
	CHECK_RESULT(vk_create_index_buffer(prv_Devices.physical, prv_Devices.logical, prv_Pools.command, 
		prv_Devices.q_graphics, prv_ObjectList, prv_Buffers_old.index, prv_Buffers_old.index_memory), "Creating Index Buffer Has Failed!");
	CHECK_RESULT(vk_create_uniform_buffer(prv_Devices.physical, prv_Devices.logical, prv_Swapchain_old.images,
		prv_Buffers_old.uniform, prv_Buffers_old.uniform_memory), "Create Uniform Buffer Has Failed!");
	CHECK_RESULT(vk_create_descriptor_pool(prv_Devices.logical, prv_Swapchain_old.images, prv_Buffers_old.descriptor_pool), "Create Descriptor Pool Has Failed!");
	CHECK_RESULT(vk_create_descriptor_sets(prv_Devices.logical, prv_Swapchain_old.images, prv_Buffers_old.descriptor_pool, 
		prv_Buffers_old.uniform, prv_Texture.texture_image_view, prv_Texture.sampler, prv_Buffers_old.descriptor_set_layout,
		prv_Buffers_old.descriptor_sets), "Create Desriptor Sets Has Failed!");
	CHECK_RESULT(vk_create_command_buffer(prv_Devices.logical, prv_Pools.command, prv_RenderGraphicsPipeline.render_pass,
		prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline,
		prv_Swapchain_old.extent2D, prv_Swapchain_old.frame_buffers, prv_Buffers_old.descriptor_sets, prv_Buffers_old.vertex, 0, 
		prv_Buffers_old.index, prv_ObjectList, prv_Command.command_buffers), "Create Command Buffer Has Failed!");

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
		prv_Swapchain_old.swapchain, prv_Swapchain_old.images, prv_Swapchain_old.format, prv_Swapchain_old.extent2D), "Create Swap Chain Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_swapchain_image_view(prv_Devices.logical, prv_Swapchain_old.images, prv_Swapchain_old.format,
		prv_Swapchain_old.views), "Create Image View Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_render_pass(prv_Devices.physical, prv_Devices.logical, prv_Swapchain_old.format,
		prv_Devices.msaa_support, prv_RenderGraphicsPipeline.render_pass), "Create Render Pass Has Failed!");

	CHECK_RESULT_NO_RETURN(vk_create_graphics_pipeline(prv_Devices.logical, prv_Swapchain_old.extent2D, prv_Buffers_old.descriptor_set_layout,
		prv_Devices.msaa_support, prv_RenderGraphicsPipeline.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
		prv_RenderGraphicsPipeline.graphics_pipeline), "Create Graphics Pipeline Has Failed!");

	CHECK_RESULT_NO_RETURN(vk_create_color_buffer(prv_Devices.physical, prv_Devices.logical, prv_Pools.command,
		prv_Devices.q_graphics, 1, prv_Devices.msaa_support, prv_Swapchain_old.extent3D, prv_Swapchain_old.format,
		prv_Texture.color_image, prv_Texture.color_image_memory, prv_Texture.color_image_view), "Create Color Buffer Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_depth_buffer(prv_Devices.physical, prv_Devices.logical, prv_Pools.command,
		prv_Devices.q_graphics, prv_Swapchain_old.extent3D, prv_Devices.msaa_support, prv_Texture.depth_buffer,
		prv_Texture.depth_buffer_memory, prv_Texture.depth_buffer_view), "Create Depth Resources Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_swapchain_frame_buffer(prv_Devices.logical, prv_RenderGraphicsPipeline.render_pass,
		prv_Swapchain_old.views, prv_Swapchain_old.extent2D, prv_Texture.color_image_view, prv_Texture.depth_buffer_view,
		prv_Swapchain_old.frame_buffers), "Create Frame Buffers Has Failed");

	CHECK_RESULT_NO_RETURN(vk_create_uniform_buffer(prv_Devices.physical, prv_Devices.logical, prv_Swapchain_old.images,
		prv_Buffers_old.uniform, prv_Buffers_old.uniform_memory), "Create Uniform Buffer Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_descriptor_pool(prv_Devices.logical, prv_Swapchain_old.images, prv_Buffers_old.descriptor_pool), "Create Descriptor Pool Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_descriptor_sets(prv_Devices.logical, prv_Swapchain_old.images, prv_Buffers_old.descriptor_pool,
		prv_Buffers_old.uniform, prv_Texture.texture_image_view, prv_Texture.sampler, prv_Buffers_old.descriptor_set_layout,
		prv_Buffers_old.descriptor_sets), "Create Desriptor Sets Has Failed!");
	CHECK_RESULT_NO_RETURN(vk_create_command_buffer(prv_Devices.logical, prv_Pools.command, prv_RenderGraphicsPipeline.render_pass,
		prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline,
		prv_Swapchain_old.extent2D, prv_Swapchain_old.frame_buffers, prv_Buffers_old.descriptor_sets, prv_Buffers_old.vertex, 0,
		prv_Buffers_old.index, prv_ObjectList, prv_Command.command_buffers), "Create Command Buffer Has Failed!");
}

void VulkanObj::setup_object_list(uint32_t size)
{
	prv_ObjectList.resize(size);
}

void VulkanObj::add_to_object_list(const Object3D & object)
{
	prv_ObjectList.push_back(object);
}

void VulkanObj::start_frame()
{
	vkAcquireNextImageKHR(prv_Devices.logical, prv_Swapchain_old.swapchain, MAX_UINT64,
		prv_SemaphoreAndFences.image_available_semaphores[prv_Frame], VK_NULL_HANDLE, &prv_Devices.current_frame);

//	VkObj_Image::EmptyGarbage(); TODO: MAKE THIS STATIC!!!
	Allocator.empty_garbage();
	StageManager.flush();

	//rendermanager.start_frame();

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(prv_Command.command_buffers[prv_Devices.current_frame], &command_buffer_begin_info);

	VkMemoryBarrier memory_barrier = {};
	memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	memory_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT | 
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | 
		VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(prv_Command.command_buffers[prv_Devices.current_frame], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
		1, &memory_barrier,
		0, nullptr,
		0, nullptr);

	VkClearValue clear_value[2];
	clear_value[0].color = { { 0.0f, 0.721f, 1.0f, 1.0f } };
	clear_value[1].depthStencil = { 1.0f, 128 };

	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = prv_RenderGraphicsPipeline.render_pass;
	render_pass_begin_info.framebuffer = prv_Swapchain.frame_buffers[prv_Devices.current_frame];
	render_pass_begin_info.renderArea.extent = prv_Swapchain.extent2D;
	render_pass_begin_info.clearValueCount = 2;
	render_pass_begin_info.pClearValues = clear_value;

	vkCmdBeginRenderPass(prv_Command.command_buffers[prv_Devices.current_frame], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanObj::end_frame()
{
	vkCmdEndRenderPass(prv_Command.command_buffers[prv_Devices.current_frame]);
	vkEndCommandBuffer(prv_Command.command_buffers[prv_Devices.current_frame]);
	//TODO: Record that the frame buffer has ended

	VkSemaphore wait_semaphores[] = { prv_SemaphoreAndFences.image_available_semaphores[prv_Devices.current_frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
	VkSemaphore signal_semaphore[] = { prv_SemaphoreAndFences.render_finished_semaphores[prv_Devices.current_frame] };


	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &prv_Command.command_buffers[prv_Devices.current_frame];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;
}

void VulkanObj::CleanupSwapchain()
{
	if (prv_Texture.color_image_view)		vkDestroyImageView(prv_Devices.logical, prv_Texture.color_image_view, nullptr);
	if (prv_Texture.color_image)			vkDestroyImage(prv_Devices.logical, prv_Texture.color_image, nullptr);
	if (prv_Texture.color_image_memory)		vkFreeMemory(prv_Devices.logical, prv_Texture.color_image_memory, nullptr);
	if (prv_Texture.depth_buffer_view)		vkDestroyImageView(prv_Devices.logical, prv_Texture.depth_buffer_view, nullptr);
	if (prv_Texture.depth_buffer)			vkDestroyImage(prv_Devices.logical, prv_Texture.depth_buffer, nullptr);
	if (prv_Texture.depth_buffer_memory)	vkFreeMemory(prv_Devices.logical, prv_Texture.depth_buffer_memory, nullptr);

	for (unsigned int i = 0; i < prv_Swapchain_old.frame_buffers.size(); ++i)
		vkDestroyFramebuffer(prv_Devices.logical, prv_Swapchain_old.frame_buffers[i], nullptr);


	vkFreeCommandBuffers(prv_Devices.logical, prv_Pools.command, CAST(uint32_t, prv_Command.command_buffers.size()), prv_Command.command_buffers.data());

	if (prv_Buffers_old.descriptor_pool)						vkDestroyDescriptorPool(prv_Devices.logical, prv_Buffers_old.descriptor_pool, nullptr);
	if (prv_RenderGraphicsPipeline.graphics_pipeline)			vkDestroyPipeline(prv_Devices.logical, prv_RenderGraphicsPipeline.graphics_pipeline, nullptr);
	if (prv_RenderGraphicsPipeline.graphics_pipeline_layout)	vkDestroyPipelineLayout(prv_Devices.logical, prv_RenderGraphicsPipeline.graphics_pipeline_layout, nullptr);
	if (prv_RenderGraphicsPipeline.render_pass)					vkDestroyRenderPass(prv_Devices.logical, prv_RenderGraphicsPipeline.render_pass, nullptr);

	for (unsigned int i = 0; i < prv_Swapchain_old.images.size(); ++i)
		vkDestroyImageView(prv_Devices.logical, prv_Swapchain_old.views[i], nullptr);

	if (prv_Swapchain_old.swapchain)								vkDestroySwapchainKHR(prv_Devices.logical, prv_Swapchain_old.swapchain, nullptr);

	for (uint32_t i = 0; i < prv_Swapchain_old.images.size(); ++i)
	{
		vkDestroyBuffer(prv_Devices.logical, prv_Buffers_old.uniform[i], nullptr);
		vkFreeMemory(prv_Devices.logical, prv_Buffers_old.uniform_memory[i], nullptr);
	}
}

void VulkanObj::cleanup()
{
	CleanupSwapchain();

	prv_Allocator.shutdown();
	prv_StagingManager.shutdown();

	for (unsigned int i = 0; i < MAX_FRAMES; ++i)
	{
		if (prv_SemaphoreAndFences.render_finished_semaphores[i])	vkDestroySemaphore(prv_Devices.logical, prv_SemaphoreAndFences.render_finished_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.image_available_semaphores[i])	vkDestroySemaphore(prv_Devices.logical, prv_SemaphoreAndFences.image_available_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.fences[i])						vkDestroyFence(prv_Devices.logical, prv_SemaphoreAndFences.fences[i], nullptr);
	}

	if (prv_Buffers_old.descriptor_set_layout)	vkDestroyDescriptorSetLayout(prv_Devices.logical, prv_Buffers_old.descriptor_set_layout, nullptr);
	if (prv_Texture.sampler)				vkDestroySampler(prv_Devices.logical, prv_Texture.sampler, nullptr);
	if (prv_Texture.texture_image_view)		vkDestroyImageView(prv_Devices.logical, prv_Texture.texture_image_view, nullptr);

	if (prv_Texture.texture_image)			vkDestroyImage(prv_Devices.logical, prv_Texture.texture_image, nullptr);
	if (prv_Texture.texture_image_memory)	vkFreeMemory(prv_Devices.logical, prv_Texture.texture_image_memory, nullptr);

	if (prv_Buffers_old.index)					vkDestroyBuffer(prv_Devices.logical, prv_Buffers_old.index, nullptr);
	if (prv_Buffers_old.index_memory)			vkFreeMemory(prv_Devices.logical, prv_Buffers_old.index_memory, nullptr);

	if (prv_Buffers_old.vertex)					vkDestroyBuffer(prv_Devices.logical, prv_Buffers_old.vertex, nullptr);
	if (prv_Buffers_old.vertex_memory)			vkFreeMemory(prv_Devices.logical, prv_Buffers_old.vertex_memory, nullptr);

	prv_Pools.shutdown();
	prv_Devices.shutdown();
	prv_Window.shutdown();
}

void VulkanObj::draw_frames()
{
	vkWaitForFences(prv_Devices.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame], VK_TRUE, MAX_UINT64);

	uint32_t image_index;
	VkResult frame_result = vkAcquireNextImageKHR(prv_Devices.logical, prv_Swapchain_old.swapchain, MAX_UINT64,
		prv_SemaphoreAndFences.image_available_semaphores[prv_Frame], VK_NULL_HANDLE, &image_index);
	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR)
		return;
	else if (frame_result != VK_SUCCESS && frame_result != VK_SUBOPTIMAL_KHR)
	{
		LOG("CANNOT DRAW FRAME!!!!!");
		return;
	}

	vk_update_uniform_buffer(prv_Devices.logical, prv_Swapchain_old.extent3D, image_index, prv_Buffers_old.uniform_memory);

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

	VkSwapchainKHR swapchains[] = { prv_Swapchain_old.swapchain };

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

	prv_Frame = (prv_Frame + 1) % MAX_FRAMES;
}
