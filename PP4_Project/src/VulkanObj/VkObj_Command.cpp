#include "VkObj_Command.h"

bool vk_create_command_pool(const VkPhysicalDevice &physical_device, const VkSurfaceKHR& surface, const VkDevice &device, VkCommandPool &command_pool)
{
	//Gather the queue family's graphic value
	QueueFamilyIndices queue_family_indices = vk_find_queue_family(physical_device, surface);

	//command pool's create info
	VkCommandPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_create_info.queueFamilyIndex = queue_family_indices.graphics.value();


	//Create the Command Pool (With Results) [VK_SUCCESS = 0]
	CHECK_VKRESULT(r, vkCreateCommandPool(device, &pool_create_info, nullptr, &command_pool), "Failed to create Command Pool!");

	//Command Pool has been created successfully
	return true;
}

bool vk_create_command_buffer(const VkDevice &device, const VkCommandPool &command_pool, 
	const VkRenderPass &render_pass, const VkPipelineLayout &graphics_pipeline_layout, const VkPipeline &graphics_pipeline,
	const VkExtent2D &swapchain_extent, const std::vector<VkFramebuffer> &swapchain_frame_buffers, const std::vector<VkDescriptorSet>& descriptor_sets,
	const VkBuffer &vertex_buffers, const VkDeviceSize &vertex_buffer_offset, const VkBuffer &index_buffers, std::vector<Object3D> &object_list,
	std::vector<VkCommandBuffer> &command_buffers)
{
	//Resize the Command Buffer to the Swapchain's buffer size
	command_buffers.resize(swapchain_frame_buffers.size());

	//Allocate Command buffer Information
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = command_pool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = CAST(uint32_t, command_buffers.size());

	//Allocate Command Buffer
	CHECK_VKRESULT(r, vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers.data()), "Failed to Allocate Command Buffer!");

	//Loop through all the allocated command buffer and set up Begin Info's (Command Buffer, Render Pass)
	for (unsigned int i = 0; i < command_buffers.size(); ++i)
	{
		//Create the Command Buffer's Begin Info
		VkCommandBufferBeginInfo command_buffer_begin_info = {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		command_buffer_begin_info.pInheritanceInfo = nullptr;

		//Begin the command buffer's begin process
		CHECK_VKRESULT(b, vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info), "Failed to Begin Command Buffer's Begin Process at index " << i << "!");

		//Create the Render Pass Begin Info
		VkRenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = render_pass;
		render_pass_begin_info.framebuffer = swapchain_frame_buffers[i];
		render_pass_begin_info.renderArea.offset = { 0,0 };
		render_pass_begin_info.renderArea.extent = swapchain_extent;

		//Clear Value for both Screen and Depth Buffer
		std::array<VkClearValue, 2> clear_color = {};
		clear_color[0] = { 0.0f,0.0f,0.0f,1.0f };
		clear_color[1] = { 1.0f, 0.0f };

		render_pass_begin_info.clearValueCount = CAST(uint32_t, clear_color.size());
		render_pass_begin_info.pClearValues = clear_color.data();

		/*
		 * Starting of the Drawing Stuff Entering Command Buffer {NOT THE ACTUAL DRAW!}
		 */
		vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

		std::array<VkBuffer, 1> vertex_buffer = { vertex_buffers };
		VkDeviceSize offsets[] = { vertex_buffer_offset };
		vkCmdBindVertexBuffers(command_buffers[i], 0, 1, vertex_buffer.data(), offsets);
		vkCmdBindIndexBuffer(command_buffers[i], index_buffers, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_layout, 0, 1, &descriptor_sets[i], 0, nullptr);

		vkCmdDrawIndexed(command_buffers[i], CAST(uint32_t, object_list[0].indices.size()), 1, 0, 0, 0);
		vkCmdEndRenderPass(command_buffers[i]);
		/*
		 * Ending of the Drawing Stuff Entering Command Buffer {NOT THE ACTUAL DRAW!}
		 */

		//End the Command Buffer
		CHECK_VKRESULT(e, vkEndCommandBuffer(command_buffers[i]), "Failed to End Command Buffer at index " << i << ",");
	}

	//Command Buffer has been created successfully
	return true;
}
