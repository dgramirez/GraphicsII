#include "VulkanObj.h"

VulkanObj::VulkanObj() {}
VulkanObj::~VulkanObj() { cleanup(); }

XTime myTime;

bool VulkanObj::init(const char* title, SDL_Window* window, unsigned short win_width, unsigned short win_height)
{
	context.init(window, prv_ObjectList);
	
	Object3D::set_static_contexts( context.device, context.command_pool);

	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
	{
		prv_ObjectList[i].init(sizeof(Mvp_object), context.pipeline_layout[0], context.pipelines[0], VkObj_Swapchain::swapchain_size);

		context.CreateCommandBuffer(prv_ObjectList[i], i);
		prv_ObjectList[i].CreateDescriptorSet(context.descriptor_pool, context.descriptor_set_layout);
	}

	//Create Semaphores
#if TRANSITION
	CHECK_RESULT(vk_create_semaphore(context.device.logical, prv_SemaphoreAndFences.image_available_semaphores,
		prv_SemaphoreAndFences.render_finished_semaphores), "Create Semaphore Has Failed!");
#else
	CHECK_RESULT(vk_sync_semaphore_and_fences(context.device.logical, prv_SemaphoreAndFences.image_available_semaphores,
		prv_SemaphoreAndFences.render_finished_semaphores, prv_SemaphoreAndFences.fences), "Create Semaphore Has Failed!");
#endif

	//	TODO: LOOK INTO QUERY POOL
	//	TODO: INVESTIGATE COMMAND BUFFERS MORE
#if TRANSITION
	prv_Buffers.CreateCommandBuffer();
#endif

//	TODO: START ALLOCATOR AND STAGING MANAGER
//	prv_Allocator.init(context.device.physical, context.device.logical);
//	prv_StagingManager.init(prv_Window.surface, context.device.physical, context.device.logical, context.device.q_graphics);

	//TODO: Continue Tutorial for RenderProgManager
	//TODO: Continue Tutorial for Vertex Cache
	
	//[These don't exist]
// 	CHECK_RESULT(vk_create_descriptor_set_layout(context.device.logical, prv_Buffers_old.descriptor_set_layout), "Create Descriptor Set Layout Failed!");
// 	CHECK_RESULT(vk_create_graphics_pipeline(context.device.logical, context.swapchain.extent2D, prv_Buffers_old.descriptor_set_layout,
// 		context.device.msaa_support, context.swapchain.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
// 		prv_RenderGraphicsPipeline.graphics_pipeline), "Create Graphics Pipeline Has Failed!");
// 	CHECK_RESULT(vk_create_texture_image(context.device.physical, context.device.logical, context.command_pool, 
// 		context.device.q_graphics, prv_ObjectList, { prv_ObjectList[0].texture->width,prv_ObjectList[0].texture->height, 1 }, 
// 		context.device.msaa_support, prv_Texture.texture_image, prv_Texture.texture_image_memory), "Create Texture Image Has Failed!");
// 	CHECK_RESULT(vk_create_texture_image_view(context.device.logical, prv_Texture.texture_image, VK_IMAGE_ASPECT_COLOR_BIT, 
// 		prv_ObjectList[0].texture->mip_levels, prv_Texture.texture_image_view), "Create Texture Image View Has Failed!");
// 	CHECK_RESULT(vk_create_texture_sampler(context.device.logical, prv_Texture.sampler), "Create Texture Image View Has Failed!");
// 	CHECK_RESULT(vk_create_vertex_buffer(context.device.physical, context.device.logical, context.command_pool,
// 		context.device.q_graphics, prv_ObjectList, prv_Buffers_old.vertex, prv_Buffers_old.vertex_memory), "Creating Vertex Buffer Has Failed!");
// 	CHECK_RESULT(vk_create_index_buffer(context.device.physical, context.device.logical, context.command_pool, 
// 		context.device.q_graphics, prv_ObjectList, prv_Buffers_old.index, prv_Buffers_old.index_memory), "Creating Index Buffer Has Failed!");
// 	CHECK_RESULT(vk_create_uniform_buffer(context.device.physical, context.device.logical, context.swapchain.images,
// 		prv_Buffers_old.uniform, prv_Buffers_old.uniform_memory), "Create Uniform Buffer Has Failed!");
// 	CHECK_RESULT(vk_create_descriptor_pool(context.device.logical, context.swapchain.images, prv_Buffers_old.descriptor_pool), "Create Descriptor Pool Has Failed!");
// 	CHECK_RESULT(vk_create_descriptor_sets(context.device.logical, context.swapchain.images, prv_Buffers_old.descriptor_pool, 
// 		prv_Buffers_old.uniform, prv_Texture.texture_image_view, prv_Texture.sampler, prv_Buffers_old.descriptor_set_layout,
// 		prv_Buffers_old.descriptor_sets), "Create Desriptor Sets Has Failed!");
// 	CHECK_RESULT(vk_create_command_buffer(context.device.logical, context.command_pool, context.swapchain.render_pass,
// 		prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline,
// 		context.swapchain.extent2D, context.swapchain.frame_buffers, prv_Buffers_old.descriptor_sets, prv_Buffers_old.vertex, 0, 
// 		prv_Buffers_old.index, prv_ObjectList, prv_Command.command_buffers), "Create Command Buffer Has Failed!");

	return true;
}

void VulkanObj::idle_device()
{
	vkDeviceWaitIdle(context.device.logical);
}

void VulkanObj::reset_swapchain()
{
	vkDeviceWaitIdle(context.device.logical);

	CleanupSwapchain();

	context.swapchain.init(context.window, context.device, context.command_pool);
	context.CreatePipelines();
	context.CreateDescriptorPool();

	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
	{
		prv_ObjectList[i].CreateUniformBuffer();
		prv_ObjectList[i].CreateDescriptorSet(context.descriptor_pool, context.descriptor_set_layout);
	}
}

void VulkanObj::setup_object_list(uint32_t size)
{
	prv_ObjectList.resize(size);
}

void VulkanObj::add_to_object_list(const Object3D & object)
{
	prv_ObjectList.push_back(object);
}

void VulkanObj::update(const SDL_Event &e)
{
	float posrotspeed = float(glm::radians(60.0f) * myTime.SmoothDelta());
	float negrotspeed = float(glm::radians(-60.0f) * myTime.SmoothDelta());
	float posmovspeed = float(5.0f * myTime.SmoothDelta());
	float negmovspeed = float(-5.0f * myTime.SmoothDelta());

	if (InputController::r_negYaw)
		myview = glm::rotate(myview, posrotspeed, glm::vec3(0, 1, 0));
	if (InputController::r_posYaw)
		myview = glm::rotate(myview, negrotspeed, glm::vec3(0, 1, 0));
	if (InputController::r_posPitch)
		myview = glm::rotate(myview, posrotspeed, glm::vec3(1, 0, 0));
	if (InputController::r_negPitch)
		myview = glm::rotate(myview, negrotspeed, glm::vec3(1, 0, 0));
	if (InputController::r_posRoll)
		myview = glm::rotate(myview, posrotspeed, glm::vec3(0, 0, 1));
	if (InputController::r_negRoll)
		myview = glm::rotate(myview, negrotspeed, glm::vec3(0, 0, 1));


	if (InputController::m_forward)
		myview = glm::translate(myview, glm::vec3(0.0f, 0.0f, negmovspeed));
	if (InputController::m_back)
		myview = glm::translate(myview, glm::vec3(0.0f, 0.0f, posmovspeed));
	if (InputController::m_left)
		myview = glm::translate(myview, glm::vec3(negmovspeed, 0.0f, 0.0f));
	if (InputController::m_right)
		myview = glm::translate(myview, glm::vec3(posmovspeed, 0.0f, 0.0f));
	if (InputController::m_up)
		myview = glm::translate(myview, glm::vec3(0.0f, negmovspeed, 0.0f));
	if (InputController::m_down)
		myview = glm::translate(myview, glm::vec3(0.0f, posmovspeed, 0.0f));

}

void VulkanObj::start_frame()
{
	vkWaitForFences(context.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame], VK_TRUE, MAX_UINT64);

	VkResult frame_result = vkAcquireNextImageKHR(context.device.logical, context.swapchain.me, MAX_UINT64,
		prv_SemaphoreAndFences.image_available_semaphores[prv_Frame], VK_NULL_HANDLE, &context.swapchain.image_index);
	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		reset_swapchain();
		return;
	}
	else if (frame_result != VK_SUCCESS && frame_result != VK_SUBOPTIMAL_KHR)
	{
		LOG("CANNOT DRAW FRAME!!!!!");
		return;
	}

	//Create the Command Buffer's Begin Info
	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	command_buffer_begin_info.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(context.swapchain.command_buffer[context.swapchain.image_index], &command_buffer_begin_info);

// 	VkMemoryBarrier memory_barrier = {};
// 	memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
// 	memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
// 	memory_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT | 
// 		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | 
// 		VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
// 
// 	vkCmdPipelineBarrier(context.swapchain.command_buffer[context.swapchain.image_index], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
// 		1, &memory_barrier,
// 		0, nullptr,
// 		0, nullptr);

	VkClearValue clear_value[2];
	clear_value[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clear_value[1].depthStencil = { 1.0f, 128 };

	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = context.swapchain.render_pass;
	render_pass_begin_info.framebuffer = context.swapchain.frame_buffers[context.swapchain.image_index];
	render_pass_begin_info.renderArea.extent = context.swapchain.extent2D;
	render_pass_begin_info.clearValueCount = 2;
	render_pass_begin_info.pClearValues = clear_value;

	vkCmdBeginRenderPass(context.swapchain.command_buffer[context.swapchain.image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanObj::end_frame()
{
	vkCmdEndRenderPass(context.swapchain.command_buffer[context.swapchain.image_index]);
	vkEndCommandBuffer(context.swapchain.command_buffer[context.swapchain.image_index]);
	//TODO: Record that the frame buffer has ended

	VkSemaphore wait_semaphores[] = { prv_SemaphoreAndFences.image_available_semaphores[prv_Frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphore[] = { prv_SemaphoreAndFences.render_finished_semaphores[prv_Frame] };

	std::array<VkCommandBuffer, 1 > pCommandBuffer = { context.swapchain.command_buffer[context.swapchain.image_index] };
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = CAST(uint32_t,pCommandBuffer.size());
	submit_info.pCommandBuffers = pCommandBuffer.data();
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;

	vkResetFences(context.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame]);

	if (vkQueueSubmit(context.device.q_graphics, 1, &submit_info, prv_SemaphoreAndFences.fences[prv_Frame]))
	{
		LOG("Failed to Submit Queue");
	}

	VkSwapchainKHR swapchains[] = { context.swapchain.me };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphore; //<--- I don't get it! why signal
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &context.swapchain.image_index;
	present_info.pResults = nullptr;

	VkResult frame_result = vkQueuePresentKHR(context.device.q_present, &present_info);

	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR || frame_result == VK_SUBOPTIMAL_KHR)
	{
		reset_swapchain();
	}
	else if (frame_result != VK_SUCCESS)
	{
		LOG("QUEUE PRESENT IS UNSUCCESSFUL!	")
	}

	prv_Frame = (prv_Frame + 1) % MAX_FRAMES;
}

void VulkanObj::CleanupSwapchain()
{
	if (context.swapchain.msaa.image)
	{
		vkDestroyImageView(	context.device.logical, context.swapchain.msaa.view,	nullptr);
		vkDestroyImage(		context.device.logical, context.swapchain.msaa.image,	nullptr);
		vkFreeMemory(		context.device.logical, context.swapchain.msaa.memory,	nullptr);
	}

	if (context.swapchain.zbuffer.image)
	{
		vkDestroyImageView(	context.device.logical,	context.swapchain.zbuffer.view,		nullptr);
		vkDestroyImage(		context.device.logical,	context.swapchain.zbuffer.image,	nullptr);
		vkFreeMemory(		context.device.logical,	context.swapchain.zbuffer.memory,	nullptr);
	}

	if (!context.swapchain.frame_buffers.empty())
	{
		for (uint32_t i = 0; i < context.swapchain.frame_buffers.size(); ++i)
			vkDestroyFramebuffer(context.device.logical, context.swapchain.frame_buffers[i], nullptr);
	}

	vkFreeCommandBuffers(context.device.logical, context.command_pool, CAST(uint32_t, context.swapchain.command_buffer.size()), context.swapchain.command_buffer.data());
	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
		prv_ObjectList[i].reset();
	
	while (!context.pipelines.empty())
	{
		uint32_t last = (uint32_t)context.pipelines.size() - 1;
		vkDestroyPipelineLayout(context.device.logical, context.pipeline_layout[last], nullptr);
		vkDestroyPipeline(context.device.logical, context.pipelines[last], nullptr);

		context.pipeline_layout.pop_back();
		context.pipelines.pop_back();
	}
	if (context.swapchain.render_pass)	vkDestroyRenderPass(context.device.logical, context.swapchain.render_pass, nullptr);

 	for (unsigned int i = 0; i < context.swapchain.images.size(); ++i)
 		vkDestroyImageView(context.device.logical, context.swapchain.image_views[i], nullptr);
 
 	if (context.swapchain.me)
		vkDestroySwapchainKHR(context.device.logical, context.swapchain.me, nullptr);

	if (context.descriptor_pool)
		vkDestroyDescriptorPool(context.device.logical, context.descriptor_pool, nullptr);
}

void VulkanObj::cleanup()
{
 	CleanupSwapchain();

	for (unsigned int i = 0; i < MAX_FRAMES; ++i)
	{
		if (prv_SemaphoreAndFences.render_finished_semaphores[i])	vkDestroySemaphore(context.device.logical, prv_SemaphoreAndFences.render_finished_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.image_available_semaphores[i])	vkDestroySemaphore(context.device.logical, prv_SemaphoreAndFences.image_available_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.fences[i])						vkDestroyFence(context.device.logical, prv_SemaphoreAndFences.fences[i], nullptr);
	}

	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
		prv_ObjectList[i].cleanup();

	context.shutdown();

}

void VulkanObj::draw_frames()
{
	vkWaitForFences(context.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame], VK_TRUE, MAX_UINT64);

	uint32_t image_index;
	VkResult frame_result = vkAcquireNextImageKHR(context.device.logical, context.swapchain.me, MAX_UINT64,
		prv_SemaphoreAndFences.image_available_semaphores[prv_Frame], VK_NULL_HANDLE, &image_index);
	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR)
		return;
	else if (frame_result != VK_SUCCESS && frame_result != VK_SUBOPTIMAL_KHR)
	{
		LOG("CANNOT DRAW FRAME!!!!!");
		return;
	}

	VkSemaphore wait_semaphores[] = { prv_SemaphoreAndFences.image_available_semaphores[prv_Frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphore[] = { prv_SemaphoreAndFences.render_finished_semaphores[prv_Frame] };

	std::array<VkCommandBuffer, 1 > pCommandBuffer = { context.swapchain.command_buffer[image_index] };

// 	for (uint32_t i = 0; i < buffersize; ++i)
// 		pCommandBuffer.push_back(prv_ObjectList[i].command_buffer[image_index]);

//	vk_update_uniform_buffer(context.device.logical, context.swapchain.extent3D, image_index, context.uniform.memory);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = (uint32_t)pCommandBuffer.size();
	submit_info.pCommandBuffers = pCommandBuffer.data();
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;

	vkResetFences(context.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame]);

	if (vkQueueSubmit(context.device.q_graphics, 1, &submit_info, prv_SemaphoreAndFences.fences[prv_Frame]))
	{
		LOG("Failed to Submit Queue");
	}

	VkSwapchainKHR swapchains[] = { context.swapchain.me };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphore; //<--- I don't get it! why signal
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	frame_result = vkQueuePresentKHR(context.device.q_present, &present_info);

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

void VulkanObj::draw()
{
	start_frame();

	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
	{
		std::array<VkBuffer, 1> vertex_buffer = { prv_ObjectList[i].vertex_buffer };
		VkDeviceSize offset[] = { 0 };

		prv_ObjectList[i].UpdateUniformBuffer(context.device.logical, context.swapchain.extent3D, context.swapchain.image_index, prv_ObjectList[i].world_matrix, prv_ObjectList[i].uniform_memory, glm::inverse(myview));
		vkCmdBindPipeline(context.swapchain.command_buffer[context.swapchain.image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, *prv_ObjectList[i].pipeline);
		vkCmdBindDescriptorSets(context.swapchain.command_buffer[context.swapchain.image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, *prv_ObjectList[i].pipeline_layout, 0, 1, &prv_ObjectList[i].descriptor_set[context.swapchain.image_index], 0, nullptr);
		vkCmdBindVertexBuffers(context.swapchain.command_buffer[context.swapchain.image_index], 0, 1, vertex_buffer.data(), offset);
		vkCmdBindIndexBuffer(context.swapchain.command_buffer[context.swapchain.image_index], prv_ObjectList[i].index_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(context.swapchain.command_buffer[context.swapchain.image_index], CAST(uint32_t, prv_ObjectList[i].indices.size()), 1, 0, 0, 0);
	}

	end_frame();
}
