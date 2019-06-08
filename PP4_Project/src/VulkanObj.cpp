#include "pch.h"
#include "VulkanObj.h"
#include "planets.h"

VulkanObj::VulkanObj() {}
VulkanObj::~VulkanObj() { cleanup(); }

bool VulkanObj::init(const char* title, SDL_Window* window, unsigned short win_width, unsigned short win_height)
{
	myContext.init(window);
	
	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
	{
		prv_ObjectList[i]->init(sizeof(Mvp_object), myContext.pipeline_layout[0], myContext.pipelines[0]);
		prv_ObjectList[i]->create_descriptor_set();
	}

	//Create Semaphores
#if TRANSITION
	CHECK_RESULT(vk_create_semaphore(myContext.device.logical, prv_SemaphoreAndFences.image_available_semaphores,
		prv_SemaphoreAndFences.render_finished_semaphores), "Create Semaphore Has Failed!");
#else
	CHECK_RESULT(vk_sync_semaphore_and_fences(myContext.device.logical, prv_SemaphoreAndFences.image_available_semaphores,
		prv_SemaphoreAndFences.render_finished_semaphores, prv_SemaphoreAndFences.fences), "Create Semaphore Has Failed!");
#endif

	//	TODO: LOOK INTO QUERY POOL
	//	TODO: INVESTIGATE COMMAND BUFFERS MORE
#if TRANSITION
	prv_Buffers.CreateCommandBuffer();
#endif

//	TODO: START ALLOCATOR AND STAGING MANAGER
//	prv_Allocator.init(myContext.device.physical, myContext.device.logical);
//	prv_StagingManager.init(prv_Window.surface, myContext.device.physical, myContext.device.logical, myContext.device.q_graphics);

	//TODO: Continue Tutorial for RenderProgManager
	//TODO: Continue Tutorial for Vertex Cache
	
	//[These don't exist]
// 	CHECK_RESULT(vk_create_descriptor_set_layout(myContext.device.logical, prv_Buffers_old.descriptor_set_layout), "Create Descriptor Set Layout Failed!");
// 	CHECK_RESULT(vk_create_graphics_pipeline(myContext.device.logical, myContext.swapchain.extent2D, prv_Buffers_old.descriptor_set_layout,
// 		myContext.device.msaa_support, myContext.swapchain.render_pass, prv_RenderGraphicsPipeline.graphics_pipeline_layout,
// 		prv_RenderGraphicsPipeline.graphics_pipeline), "Create Graphics Pipeline Has Failed!");
// 	CHECK_RESULT(vk_create_texture_image(myContext.device.physical, myContext.device.logical, myContext.command_pool, 
// 		myContext.device.q_graphics, prv_ObjectList, { prv_ObjectList[0].texture->width,prv_ObjectList[0].texture->height, 1 }, 
// 		myContext.device.msaa_support, prv_Texture.texture_image, prv_Texture.texture_image_memory), "Create Texture Image Has Failed!");
// 	CHECK_RESULT(vk_create_texture_image_view(myContext.device.logical, prv_Texture.texture_image, VK_IMAGE_ASPECT_COLOR_BIT, 
// 		prv_ObjectList[0].texture->mip_levels, prv_Texture.texture_image_view), "Create Texture Image View Has Failed!");
// 	CHECK_RESULT(vk_create_texture_sampler(myContext.device.logical, prv_Texture.sampler), "Create Texture Image View Has Failed!");
// 	CHECK_RESULT(vk_create_vertex_buffer(myContext.device.physical, myContext.device.logical, myContext.command_pool,
// 		myContext.device.q_graphics, prv_ObjectList, prv_Buffers_old.vertex, prv_Buffers_old.vertex_memory), "Creating Vertex Buffer Has Failed!");
// 	CHECK_RESULT(vk_create_index_buffer(myContext.device.physical, myContext.device.logical, myContext.command_pool, 
// 		myContext.device.q_graphics, prv_ObjectList, prv_Buffers_old.index, prv_Buffers_old.index_memory), "Creating Index Buffer Has Failed!");
// 	CHECK_RESULT(vk_create_uniform_buffer(myContext.device.physical, myContext.device.logical, myContext.swapchain.images,
// 		prv_Buffers_old.uniform, prv_Buffers_old.uniform_memory), "Create Uniform Buffer Has Failed!");
// 	CHECK_RESULT(vk_create_descriptor_pool(myContext.device.logical, myContext.swapchain.images, prv_Buffers_old.descriptor_pool), "Create Descriptor Pool Has Failed!");
// 	CHECK_RESULT(vk_create_descriptor_sets(myContext.device.logical, myContext.swapchain.images, prv_Buffers_old.descriptor_pool, 
// 		prv_Buffers_old.uniform, prv_Texture.texture_image_view, prv_Texture.sampler, prv_Buffers_old.descriptor_set_layout,
// 		prv_Buffers_old.descriptor_sets), "Create Desriptor Sets Has Failed!");
// 	CHECK_RESULT(vk_create_command_buffer(myContext.device.logical, myContext.command_pool, myContext.swapchain.render_pass,
// 		prv_RenderGraphicsPipeline.graphics_pipeline_layout, prv_RenderGraphicsPipeline.graphics_pipeline,
// 		myContext.swapchain.extent2D, myContext.swapchain.frame_buffers, prv_Buffers_old.descriptor_sets, prv_Buffers_old.vertex, 0, 
// 		prv_Buffers_old.index, prv_ObjectList, prv_Command.command_buffers), "Create Command Buffer Has Failed!");

	camera.init(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -5.0f, 5.0f)), glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	camera.set_object_list(prv_ObjectList);
	camera.set_planet_lookup(MERCURY);
	return true;
}

void VulkanObj::idle_device()
{
	vkDeviceWaitIdle(myContext.device.logical);
}

void VulkanObj::reset_swapchain()
{
	vkDeviceWaitIdle(myContext.device.logical);

	CleanupSwapchain();

	myContext.swapchain.init(myContext.window, myContext.device, myContext.command_pool);
	myContext.CreatePipelines();
	myContext.CreateDescriptorPool();

	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
	{
		prv_ObjectList[i]->create_uniform_buffer();
		prv_ObjectList[i]->create_descriptor_set();
	}
}

void VulkanObj::setup_object_list(uint32_t size)
{
	prv_ObjectList.resize(size);
}

void VulkanObj::add_to_object_list(Object *object)
{
	prv_ObjectList.push_back(object);
}

void VulkanObj::update(const SDL_Event &e)
{
	update_window_title();
	camera.update(e);
}

void VulkanObj::update_window_title()
{
	char buffer[255];
	ZeroMemory(buffer, 255);
	sprintf_s(buffer, sizeof(buffer), "Fov: %f | Near: %f | Far: %f | MovSpd: %f", camera.fov, camera.nearplane, camera.farplane, camera.viewspd);

	uint32_t length = (uint32_t)strlen(buffer);
	char* title = new char[length + 1];
	ZeroMemory(title, length + 1);

	strcpy(title, buffer);

	SDL_SetWindowTitle(myContext.window.window, title);
}

void VulkanObj::start_frame()
{
	vkWaitForFences(myContext.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame], VK_TRUE, MAX_UINT64);

	VkResult frame_result = vkAcquireNextImageKHR(myContext.device.logical, myContext.swapchain.me, MAX_UINT64,
		prv_SemaphoreAndFences.image_available_semaphores[prv_Frame], VK_NULL_HANDLE, &myContext.swapchain.image_index);
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
	vkBeginCommandBuffer(myContext.swapchain.command_buffer[myContext.swapchain.image_index], &command_buffer_begin_info);

// 	VkMemoryBarrier memory_barrier = {};
// 	memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
// 	memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
// 	memory_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT | 
// 		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | 
// 		VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
// 
// 	vkCmdPipelineBarrier(myContext.swapchain.command_buffer[myContext.swapchain.image_index], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
// 		1, &memory_barrier,
// 		0, nullptr,
// 		0, nullptr);

	VkClearValue clear_value[2];
	clear_value[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clear_value[1].depthStencil = { 1.0f, 128 };

	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = myContext.swapchain.render_pass;
	render_pass_begin_info.framebuffer = myContext.swapchain.frame_buffers[myContext.swapchain.image_index];
	render_pass_begin_info.renderArea.extent = myContext.swapchain.extent2D;
	render_pass_begin_info.clearValueCount = 2;
	render_pass_begin_info.pClearValues = clear_value;

	vkCmdBeginRenderPass(myContext.swapchain.command_buffer[myContext.swapchain.image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanObj::end_frame()
{
	vkCmdEndRenderPass(myContext.swapchain.command_buffer[myContext.swapchain.image_index]);
	vkEndCommandBuffer(myContext.swapchain.command_buffer[myContext.swapchain.image_index]);
	//TODO: Record that the frame buffer has ended

	VkSemaphore wait_semaphores[] = { prv_SemaphoreAndFences.image_available_semaphores[prv_Frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphore[] = { prv_SemaphoreAndFences.render_finished_semaphores[prv_Frame] };

	std::array<VkCommandBuffer, 1 > pCommandBuffer = { myContext.swapchain.command_buffer[myContext.swapchain.image_index] };
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = CAST(uint32_t,pCommandBuffer.size());
	submit_info.pCommandBuffers = pCommandBuffer.data();
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;

	vkResetFences(myContext.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame]);

	if (vkQueueSubmit(myContext.device.q_graphics, 1, &submit_info, prv_SemaphoreAndFences.fences[prv_Frame]))
	{
		LOG("Failed to Submit Queue");
	}

	VkSwapchainKHR swapchains[] = { myContext.swapchain.me };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphore; //<--- I don't get it! why signal
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &myContext.swapchain.image_index;
	present_info.pResults = nullptr;

	VkResult frame_result = vkQueuePresentKHR(myContext.device.q_present, &present_info);

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
	if (myContext.swapchain.msaa.image)
	{
		vkDestroyImageView(	myContext.device.logical, myContext.swapchain.msaa.view,	nullptr);
		vkDestroyImage(		myContext.device.logical, myContext.swapchain.msaa.image,	nullptr);
		vkFreeMemory(		myContext.device.logical, myContext.swapchain.msaa.memory,	nullptr);
	}

	if (myContext.swapchain.zbuffer.image)
	{
		vkDestroyImageView(	myContext.device.logical,	myContext.swapchain.zbuffer.view,		nullptr);
		vkDestroyImage(		myContext.device.logical,	myContext.swapchain.zbuffer.image,	nullptr);
		vkFreeMemory(		myContext.device.logical,	myContext.swapchain.zbuffer.memory,	nullptr);
	}

	if (!myContext.swapchain.frame_buffers.empty())
	{
		for (uint32_t i = 0; i < myContext.swapchain.frame_buffers.size(); ++i)
			vkDestroyFramebuffer(myContext.device.logical, myContext.swapchain.frame_buffers[i], nullptr);
	}

	vkFreeCommandBuffers(myContext.device.logical, myContext.command_pool, CAST(uint32_t, myContext.swapchain.command_buffer.size()), myContext.swapchain.command_buffer.data());
	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
		prv_ObjectList[i]->reset();
	
	while (!myContext.pipelines.empty())
	{
		uint32_t last = (uint32_t)myContext.pipelines.size() - 1;
		vkDestroyPipelineLayout(myContext.device.logical, myContext.pipeline_layout[last], nullptr);
		vkDestroyPipeline(myContext.device.logical, myContext.pipelines[last], nullptr);

		myContext.pipeline_layout.pop_back();
		myContext.pipelines.pop_back();
	}
	if (myContext.swapchain.render_pass)	vkDestroyRenderPass(myContext.device.logical, myContext.swapchain.render_pass, nullptr);

 	for (unsigned int i = 0; i < myContext.swapchain.images.size(); ++i)
 		vkDestroyImageView(myContext.device.logical, myContext.swapchain.image_views[i], nullptr);
 
 	if (myContext.swapchain.me)
		vkDestroySwapchainKHR(myContext.device.logical, myContext.swapchain.me, nullptr);

	if (myContext.descriptor_pool)
		vkDestroyDescriptorPool(myContext.device.logical, myContext.descriptor_pool, nullptr);
}

void VulkanObj::cleanup()
{
 	CleanupSwapchain();

	for (unsigned int i = 0; i < MAX_FRAMES; ++i)
	{
		if (prv_SemaphoreAndFences.render_finished_semaphores[i])	vkDestroySemaphore(myContext.device.logical, prv_SemaphoreAndFences.render_finished_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.image_available_semaphores[i])	vkDestroySemaphore(myContext.device.logical, prv_SemaphoreAndFences.image_available_semaphores[i], nullptr);
		if (prv_SemaphoreAndFences.fences[i])						vkDestroyFence(myContext.device.logical, prv_SemaphoreAndFences.fences[i], nullptr);
	}

	for (uint32_t i = 0; i < prv_ObjectList.size(); ++i)
		prv_ObjectList[i]->cleanup();

	myContext.shutdown();

}

void VulkanObj::draw_frames()
{
	vkWaitForFences(myContext.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame], VK_TRUE, MAX_UINT64);

	uint32_t image_index;
	VkResult frame_result = vkAcquireNextImageKHR(myContext.device.logical, myContext.swapchain.me, MAX_UINT64,
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

	std::array<VkCommandBuffer, 1 > pCommandBuffer = { myContext.swapchain.command_buffer[image_index] };

// 	for (uint32_t i = 0; i < buffersize; ++i)
// 		pCommandBuffer.push_back(prv_ObjectList[i]->command_buffer[image_index]);

//	vk_update_uniform_buffer(myContext.device.logical, myContext.swapchain.extent3D, image_index, myContext.uniform.memory);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = (uint32_t)pCommandBuffer.size();
	submit_info.pCommandBuffers = pCommandBuffer.data();
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphore;

	vkResetFences(myContext.device.logical, 1, &prv_SemaphoreAndFences.fences[prv_Frame]);

	if (vkQueueSubmit(myContext.device.q_graphics, 1, &submit_info, prv_SemaphoreAndFences.fences[prv_Frame]))
	{
		LOG("Failed to Submit Queue");
	}

	VkSwapchainKHR swapchains[] = { myContext.swapchain.me };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphore; //<--- I don't get it! why signal
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	frame_result = vkQueuePresentKHR(myContext.device.q_present, &present_info);

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

	for (uint32_t i = 0; i < 1; ++i)
	{
		std::array<VkBuffer, 1> vertex_buffer = { prv_ObjectList[i]->mesh->vertex_buffer };
		VkDeviceSize offset[] = { 0 };

		prv_ObjectList[i]->update_uniform_buffer(camera);
		vkCmdBindPipeline(myContext.swapchain.command_buffer[myContext.swapchain.image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, *prv_ObjectList[i]->pipeline);
		vkCmdBindDescriptorSets(myContext.swapchain.command_buffer[myContext.swapchain.image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, *prv_ObjectList[i]->pipeline_layout, 0, 1, &prv_ObjectList[i]->descriptor_set[myContext.swapchain.image_index], 0, nullptr);
		vkCmdBindVertexBuffers(myContext.swapchain.command_buffer[myContext.swapchain.image_index], 0, 1, vertex_buffer.data(), offset);
		vkCmdBindIndexBuffer(myContext.swapchain.command_buffer[myContext.swapchain.image_index], prv_ObjectList[i]->mesh->index_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(myContext.swapchain.command_buffer[myContext.swapchain.image_index], CAST(uint32_t, prv_ObjectList[i]->mesh->indices.size()), 1, 0, 0, 0);
	}

	std::array<VkClearDepthStencilValue, 1> clear_value = { { 1.0f, 128 } };
	VkImageSubresourceRange range = {};
	range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	range.baseMipLevel = 0;
	range.levelCount = 1;
	range.baseArrayLayer = 0;
	range.layerCount = 1;
	vkCmdClearDepthStencilImage(myContext.swapchain.command_buffer[myContext.swapchain.image_index], myContext.swapchain.zbuffer.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clear_value.data(), 1, &range);

	for (uint32_t i = 1; i < prv_ObjectList.size(); ++i)
	{
		std::array<VkBuffer, 1> vertex_buffer = { prv_ObjectList[i]->mesh->vertex_buffer };
		VkDeviceSize offset[] = { 0 };

		prv_ObjectList[i]->update_uniform_buffer(camera);
		vkCmdBindPipeline(myContext.swapchain.command_buffer[myContext.swapchain.image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, *prv_ObjectList[i]->pipeline);
		vkCmdBindDescriptorSets(myContext.swapchain.command_buffer[myContext.swapchain.image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, *prv_ObjectList[i]->pipeline_layout, 0, 1, &prv_ObjectList[i]->descriptor_set[myContext.swapchain.image_index], 0, nullptr);
		vkCmdBindVertexBuffers(myContext.swapchain.command_buffer[myContext.swapchain.image_index], 0, 1, vertex_buffer.data(), offset);
		vkCmdBindIndexBuffer(myContext.swapchain.command_buffer[myContext.swapchain.image_index], prv_ObjectList[i]->mesh->index_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(myContext.swapchain.command_buffer[myContext.swapchain.image_index], CAST(uint32_t, prv_ObjectList[i]->mesh->indices.size()), 1, 0, 0, 0);
	}

	end_frame();
}
