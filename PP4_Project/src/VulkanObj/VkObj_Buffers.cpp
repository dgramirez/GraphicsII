#include "VkObj_Buffers.h"

bool vk_create_descriptor_set_layout(const VkDevice &device, VkDescriptorSetLayout &descriptor_set_layout)
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
	create_info.bindingCount = CAST(uint32_t, bindings.size());
	create_info.pBindings = bindings.data();

	CHECK_VKRESULT(a, vkCreateDescriptorSetLayout(device, &create_info, nullptr, &descriptor_set_layout), "Failed to create Descriptor Set Layout!");

	return true;
}

bool vk_create_descriptor_pool(const VkDevice &device, const std::vector<VkImage> &swapchain_images, VkDescriptorPool &descriptor_pool)
{
	std::array<VkDescriptorPoolSize, 2> descriptor_pool_size = {};
	descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_pool_size[0].descriptorCount = CAST(uint32_t, swapchain_images.size());
	descriptor_pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_pool_size[1].descriptorCount = CAST(uint32_t, swapchain_images.size());

	VkDescriptorPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = CAST(uint32_t, descriptor_pool_size.size());
	create_info.pPoolSizes = descriptor_pool_size.data();
	create_info.maxSets = CAST(uint32_t, swapchain_images.size());

	CHECK_VKRESULT(r, vkCreateDescriptorPool(device, &create_info, nullptr, &descriptor_pool), "Failed to create a Desriptor Pool! Error Code: ");

	return true;
}

// VkObj_Buffers::VkObj_Buffers()
// 	: prv_Size(0), prv_OffsetInOtherBuffer(0), prv_Usage(0), prv_Buffer(nullptr) {}
// 
// bool VkObj_Buffers::init(VkObj_WindowProperties &window_properties, VkObj_DeviceProperties &device_properties, VkObj_Swapchain &swapchain, VkObj_Pools &pools)
// {
// 	pWindowProperties = &window_properties;
// 	pDeviceProperties = &device_properties;
// 	pSwapchain = &swapchain;
// 	pPools = &pools;
// 
// 	return true;
// }
// 
// int64_t VkObj_Buffers::get_size()
// {
// 	return (prv_Size & ~MASK_BIT_32);
// }
// 
// 
// int64_t VkObj_Buffers::get_allocated_size()
// {
// 	return ((get_size() + 15) & ~15); //15 = 1111
// }
// 
// uint64_t VkObj_Buffers::get_usage_type()
// {
// 	return prv_Usage;
// }
// 
// VkBuffer VkObj_Buffers::get_buffer()
// {
// 	return prv_Buffer;
// }
// 
// int64_t VkObj_Buffers::get_offset()
// {
// 	return (prv_OffsetInOtherBuffer & ~MASK_BIT_32);
// }
// 
// bool VkObj_Buffers::is_mapped()
// {
// 	return (prv_Size & MASK_BIT_32);
// }
// 
// void VkObj_Buffers::set_map_flag(const bool &on)
// {
// 	if (on)
// 		prv_Size |= MASK_BIT_32;
// 	else
// 		prv_Size &= ~MASK_BIT_32;
// }
// 
// bool VkObj_Buffers::root_buffer()
// {
// 	return (prv_OffsetInOtherBuffer & MASK_BIT_32);
// }
// 
// bool VkObj_Buffers::CreateCommandBuffer()
// {
// 	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
// 	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
// 	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
// 	command_buffer_allocate_info.commandPool = pPools->command;
// 	command_buffer_allocate_info.commandBufferCount = MAX_FRAMES;
// 
// 	vkAllocateCommandBuffers(pDeviceProperties->logical, &command_buffer_allocate_info, &command.me);
// 
// 	VkFenceCreateInfo fence_create_info = {};
// 	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
// 
// 	command.fence.resize(MAX_FRAMES);
// 	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
// 		vkCreateFence(pDeviceProperties->logical, &fence_create_info, nullptr, &command.fence[i]);
// 
// 	return true;
// }
// 
// bool VkObj_Buffers::CreateDepthAndMSAA()
// {
// 	return true;
// }
// 
// bool VkObj_Buffers::CreateSwapchainFrameBuffers(const VkImageView &color_image_view, const VkImageView &depth_buffer_view, const VkRenderPass &render_pass)
// {
// 	//Resize the Swapchain Frame Buffer Vector
// 	pSwapchain->frame_buffers.resize(pSwapchain->image_views.size());
// 
// 	//Loop through the Swapchain Frame Buffers and set their create info
// 	for (unsigned int i = 0; i < pSwapchain->frame_buffers.size(); ++i) {
// 		// Create an array of image attachments for create info (NOTE: There is only 1 Color Image View and Depth Buffer!)
// 		std::array<VkImageView, 3> image_attachments = {
// 			color_image_view,
// 			depth_buffer_view,
// 			pSwapchain->image_views[i]
// 		};
// 
// 		//Frame Buffer's Create Info
// 		VkFramebufferCreateInfo frame_buffer_create_info = {};
// 		frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
// 		frame_buffer_create_info.renderPass = render_pass;
// 		frame_buffer_create_info.attachmentCount = CAST(uint32_t, image_attachments.size());
// 		frame_buffer_create_info.pAttachments = image_attachments.data();
// 		frame_buffer_create_info.width = pSwapchain->extent2D.width;
// 		frame_buffer_create_info.height = pSwapchain->extent2D.height;
// 		frame_buffer_create_info.layers = 1;
// 
// 
// 		//Create the Surface (With Results) [VK_SUCCESS = 0]
// 		CHECK_VKRESULT(r, vkCreateFramebuffer(pDeviceProperties->logical, &frame_buffer_create_info, nullptr, &pSwapchain->frame_buffers[i]), "Failed to create Frame buffer at index" << i);
// 	}
// 
// 	//Swapchain's Frame Buffers has been created successfully
// 	return true;
// }
// 
// 
// VkObj_VertexBuffer::VkObj_VertexBuffer()
// {
// 	set_map_flag(false);
// }
// 
// void VkObj_VertexBuffer::clear_no_free()
// {
// 	prv_Size = 0;
// 	prv_OffsetInOtherBuffer = OWNS_BUFFER_FLAG;
// 	prv_Buffer = nullptr;
// 	prv_Allocation.device_memory = nullptr;
// }
// 
// bool VkObj_VertexBuffer::allocate_buffer(const void* data, const uint32_t &allocation_size, const uint32_t &usage)
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// // 	//Assert 16 bit aligned (data)
// 	if ((uint32_t)(data) & 15) {
// 		LOG("Data Buffer is Not Aligned by 16 bits")
// 	}
// 
// 	if (allocation_size <= 0) {
// 		LOG("Allocation Size is below 1!")
// 	}
// 
// 	prv_Size = allocation_size;
// 	prv_Usage = usage;
// 
// 	int64_t bytes = get_allocated_size();
// 
// 	VkBufferCreateInfo buffer_create_info = {};
// 	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
// 	buffer_create_info.size = bytes;
// 	buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
// 	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC)
// 		buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
// 
// 	CHECK_VKRESULT(r, vkCreateBuffer(pDeviceProperties->logical, &buffer_create_info, nullptr, &prv_Buffer), "Failed to create buffer in VkObj_VertexBuffer::allocate_buffer");
// 
// 	VkMemoryRequirements memory_requirement;
// 	vkGetBufferMemoryRequirements(pDeviceProperties->logical, prv_Buffer, &memory_requirement);
// 	uint32_t memory_usage = (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) ? VKDEFINE_MEMORY_USAGE_GPU_ONLY : VKDEFINE_MEMORY_USAGE_CPU_TO_GPU;
// 
// 	prv_Allocation = Allocator.allocate(memory_requirement.size, memory_requirement.alignment, memory_requirement.memoryTypeBits, memory_usage, VKDEFINE_ALLOCATION_TYPE_BUFFER);
// 
// 	vkBindBufferMemory(pDeviceProperties->logical, prv_Buffer, prv_Allocation.device_memory, prv_Allocation.offset);
// 
// 	if (data)
// 		update(data, allocation_size, 0);
// 
// 	return true;
// }
// 
// void VkObj_VertexBuffer::free_buffer()
// {
// 	if (is_mapped())
// 		unmap_buffer();
// 
// 	if (!root_buffer()) {
// 		clear_no_free();
// 		return;
// 	}
// 
// 	if (!prv_Buffer)
// 		return;
// 
// 	Allocator.free(prv_Allocation);
// 	vkDestroyBuffer(pDeviceProperties->logical, prv_Buffer, nullptr);
// 	prv_Buffer = nullptr;
// 	prv_Allocation = VkObj_MemoryBlock();
// 
// 	clear_no_free();
// }
// 
// void VkObj_VertexBuffer::update(const void* data, const uint32_t &size, const uint32_t &offset)
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	//Assert 16 bit aligned (data)
// 	if ((uint32_t)(data) & 15) {
// 		LOG("Data Buffer is Not Aligned by 16 bits")
// 	}
// 
// 	if ( ((uint32_t)(offset)) & 15) {
// 		LOG("Offset is Not Aligned by 16 bits")
// 	}
// 
// 	if (size > get_size()) {
// 		LOG("Size is greater than the allocated size!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_DYNAMIC)
// 		memcpy(prv_Allocation.data + get_offset() + offset, data, size);
// 	else
// 	{
// 		VkBuffer stage_buffer;
// 		uint64_t stage_offset = 0;
// 		VkCommandBuffer command_buffer;
// 		char* stage_data = StageManager.stage(size, 1, command_buffer, stage_buffer, stage_offset);
// 
// 		memcpy(stage_data, data, size);
// 
// 		VkBufferCopy buffer_copy = {};
// 		buffer_copy.srcOffset = stage_offset;
// 		buffer_copy.dstOffset = get_offset() + offset;
// 		buffer_copy.size = size;
// 
// 		vkCmdCopyBuffer(command_buffer, stage_buffer, prv_Buffer, 1, &buffer_copy);
// 	}
// 
// }
// 
// void* VkObj_VertexBuffer::map_buffer()
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) {
// 		LOG("WARNING: TRYING TO MAP BUFFER, BUT VERTEX BUFFER'S USAGE IS STATIC")
// 	}
// 
// 	void *buffer = prv_Allocation.data + get_offset();
// 
// 	set_map_flag(true);
// 
// 	if (!buffer) {
// 		LOG("WARNING: BUFFER FAILED TO MAP!")
// 	}
// 
// 	return buffer;
// }
// 
// void VkObj_VertexBuffer::unmap_buffer()
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) {
// 		LOG("WARNING: TRYING TO MAP BUFFER, BUT VERTEX BUFFER'S USAGE IS STATIC")
// 	}
// 
// 	set_map_flag(false);
// }
// 
// 
// VkObj_IndexBuffer::VkObj_IndexBuffer()
// {
// 	set_map_flag(false);
// }
// 
// void VkObj_IndexBuffer::clear_no_free()
// {
// 	prv_Size = 0;
// 	prv_OffsetInOtherBuffer = OWNS_BUFFER_FLAG;
// 	prv_Buffer = nullptr;
// 	prv_Allocation.device_memory = nullptr;
// }
// 
// bool VkObj_IndexBuffer::allocate_buffer(const void* data, const uint32_t &allocation_size, const uint32_t &usage)
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	// 	//Assert 16 bit aligned (data)
// 	if ((uint32_t)(data) & 15) {
// 		LOG("Data Buffer is Not Aligned by 16 bits")
// 	}
// 
// 	if (allocation_size <= 0) {
// 		LOG("Allocation Size is below 1!")
// 	}
// 
// 	prv_Size = allocation_size;
// 	prv_Usage = usage;
// 
// 	int64_t bytes = get_allocated_size();
// 
// 	VkBufferCreateInfo buffer_create_info = {};
// 	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
// 	buffer_create_info.size = bytes;
// 	buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC)
// 		buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
// 
// 	CHECK_VKRESULT(r, vkCreateBuffer(pDeviceProperties->logical, &buffer_create_info, nullptr, &prv_Buffer), "Failed to create buffer in VkObj_VertexBuffer::allocate_buffer");
// 
// 	VkMemoryRequirements memory_requirement;
// 	vkGetBufferMemoryRequirements(pDeviceProperties->logical, prv_Buffer, &memory_requirement);
// 	uint32_t memory_usage = (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) ? VKDEFINE_MEMORY_USAGE_GPU_ONLY : VKDEFINE_MEMORY_USAGE_CPU_TO_GPU;
// 
// 	prv_Allocation = Allocator.allocate(memory_requirement.size, memory_requirement.alignment, memory_requirement.memoryTypeBits, memory_usage, VKDEFINE_ALLOCATION_TYPE_BUFFER);
// 
// 	vkBindBufferMemory(pDeviceProperties->logical, prv_Buffer, prv_Allocation.device_memory, prv_Allocation.offset);
// 
// 	if (data)
// 		update(data, allocation_size, 0);
// 
// 	return true;
// }
// 
// void VkObj_IndexBuffer::free_buffer()
// {
// 	if (is_mapped())
// 		unmap_buffer();
// 
// 	if (!root_buffer()) {
// 		clear_no_free();
// 		return;
// 	}
// 
// 	if (!prv_Buffer)
// 		return;
// 
// 	Allocator.free(prv_Allocation);
// 	vkDestroyBuffer(pDeviceProperties->logical, prv_Buffer, nullptr);
// 	prv_Buffer = nullptr;
// 	prv_Allocation = VkObj_MemoryBlock();
// 
// 	clear_no_free();
// }
// 
// void VkObj_IndexBuffer::update(const void* data, const uint32_t &size, const uint32_t &offset)
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	//Assert 16 bit aligned (data)
// 	if ((uint32_t)(data) & 15) {
// 		LOG("Data Buffer is Not Aligned by 16 bits")
// 	}
// 
// 	if (((uint32_t)(offset)) & 15) {
// 		LOG("Offset is Not Aligned by 16 bits")
// 	}
// 
// 	if (size > get_size()) {
// 		LOG("Size is greater than the allocated size!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_DYNAMIC)
// 		memcpy(prv_Allocation.data + get_offset() + offset, data, size);
// 	else
// 	{
// 		VkBuffer stage_buffer;
// 		uint64_t stage_offset = 0;
// 		VkCommandBuffer command_buffer;
// 		char* stage_data = StageManager.stage(size, 1, command_buffer, stage_buffer, stage_offset);
// 
// 		memcpy(stage_data, data, size);
// 
// 		VkBufferCopy buffer_copy = {};
// 		buffer_copy.srcOffset = stage_offset;
// 		buffer_copy.dstOffset = get_offset() + offset;
// 		buffer_copy.size = size;
// 
// 		vkCmdCopyBuffer(command_buffer, stage_buffer, prv_Buffer, 1, &buffer_copy);
// 	}
// }
// 
// void* VkObj_IndexBuffer::map_buffer()
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) {
// 		LOG("WARNING: TRYING TO MAP BUFFER, BUT VERTEX BUFFER'S USAGE IS STATIC")
// 	}
// 
// 	void *buffer = prv_Allocation.data + get_offset();
// 
// 	set_map_flag(true);
// 
// 	if (!buffer) {
// 		LOG("WARNING: BUFFER FAILED TO MAP!")
// 	}
// 
// 	return buffer;
// }
// 
// void VkObj_IndexBuffer::unmap_buffer()
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) {
// 		LOG("WARNING: TRYING TO MAP BUFFER, BUT VERTEX BUFFER'S USAGE IS STATIC")
// 	}
// 
// 	set_map_flag(false);
// }
// 
// 
// VkObj_UniformBuffer::VkObj_UniformBuffer()
// {
// 	prv_Usage = VKDEFINE_BUFFER_USAGE_DYNAMIC;
// 	set_map_flag(false);
// }
// 
// void VkObj_UniformBuffer::clear_no_free()
// {
// 	prv_Size = 0;
// 	prv_OffsetInOtherBuffer = OWNS_BUFFER_FLAG;
// 	prv_Buffer = nullptr;
// 	prv_Allocation.device_memory = nullptr;
// }
// 
// bool VkObj_UniformBuffer::allocate_buffer(const void* data, const uint32_t &allocation_size, const uint32_t &usage)
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	// 	//Assert 16 bit aligned (data)
// 	if ((uint32_t)(data) & 15) {
// 		LOG("Data Buffer is Not Aligned by 16 bits")
// 	}
// 
// 	if (allocation_size <= 0) {
// 		LOG("Allocation Size is below 1!")
// 	}
// 
// 	prv_Size = allocation_size;
// 	prv_Usage = usage;
// 
// 	int64_t bytes = get_allocated_size();
// 
// 	VkBufferCreateInfo buffer_create_info = {};
// 	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
// 	buffer_create_info.size = bytes;
// 	buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC)
// 		buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
// 
// 	CHECK_VKRESULT(r, vkCreateBuffer(pDeviceProperties->logical, &buffer_create_info, nullptr, &prv_Buffer), "Failed to create buffer in VkObj_VertexBuffer::allocate_buffer");
// 
// 	VkMemoryRequirements memory_requirement;
// 	vkGetBufferMemoryRequirements(pDeviceProperties->logical, prv_Buffer, &memory_requirement);
// 	uint32_t memory_usage = (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) ? VKDEFINE_MEMORY_USAGE_GPU_ONLY : VKDEFINE_MEMORY_USAGE_CPU_TO_GPU;
// 
// 	prv_Allocation = Allocator.allocate(memory_requirement.size, memory_requirement.alignment, memory_requirement.memoryTypeBits, memory_usage, VKDEFINE_ALLOCATION_TYPE_BUFFER);
// 
// 	vkBindBufferMemory(pDeviceProperties->logical, prv_Buffer, prv_Allocation.device_memory, prv_Allocation.offset);
// 
// 	if (data)
// 		update(data, allocation_size, 0);
// 
// 	return true;
// }
// 
// void VkObj_UniformBuffer::free_buffer()
// {
// 	if (is_mapped())
// 		unmap_buffer();
// 
// 	if (!root_buffer()) {
// 		clear_no_free();
// 		return;
// 	}
// 
// 	if (!prv_Buffer)
// 		return;
// 
// 	Allocator.free(prv_Allocation);
// 	vkDestroyBuffer(pDeviceProperties->logical, prv_Buffer, nullptr);
// 	prv_Buffer = nullptr;
// 	prv_Allocation = VkObj_MemoryBlock();
// 
// 	clear_no_free();
// }
// 
// void VkObj_UniformBuffer::update(const void* data, const uint32_t &size, const uint32_t &offset)
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	//Assert 16 bit aligned (data)
// 	if ((uint32_t)(data) & 15) {
// 		LOG("Data Buffer is Not Aligned by 16 bits")
// 	}
// 
// 	if (((uint32_t)(offset)) & 15) {
// 		LOG("Offset is Not Aligned by 16 bits")
// 	}
// 
// 	if (size > get_size()) {
// 		LOG("Size is greater than the allocated size!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_DYNAMIC)
// 		memcpy(prv_Allocation.data + get_offset() + offset, data, size);
// 	else
// 	{
// 		VkBuffer stage_buffer;
// 		uint64_t stage_offset = 0;
// 		VkCommandBuffer command_buffer;
// 		char* stage_data = StageManager.stage(size, 1, command_buffer, stage_buffer, stage_offset);
// 
// 		memcpy(stage_data, data, size);
// 
// 		VkBufferCopy buffer_copy = {};
// 		buffer_copy.srcOffset = stage_offset;
// 		buffer_copy.dstOffset = get_offset() + offset;
// 		buffer_copy.size = size;
// 
// 		vkCmdCopyBuffer(command_buffer, stage_buffer, prv_Buffer, 1, &buffer_copy);
// 	}
// }
// 
// void* VkObj_UniformBuffer::map_buffer(uint32_t map_type)
// {
// 	if (map_type == VKDEFINE_BUFFER_MAP_WRITE) {
// 		LOG("WARNING: UNIFORM BUFFER HAS WRITE MAP TYPE!")
// 	}
// 
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) {
// 		LOG("WARNING: TRYING TO MAP BUFFER, BUT VERTEX BUFFER'S USAGE IS STATIC")
// 	}
// 
// 	void *buffer = prv_Allocation.data + get_offset();
// 
// 	set_map_flag(true);
// 
// 	if (!buffer) {
// 		LOG("WARNING: BUFFER FAILED TO MAP!")
// 	}
// 
// 	return buffer;
// }
// 
// void VkObj_UniformBuffer::unmap_buffer()
// {
// 	if (!prv_Buffer) {
// 		LOG("Buffer is nullptr in Vertex Buffer!")
// 	}
// 
// 	if (prv_Usage == VKDEFINE_BUFFER_USAGE_STATIC) {
// 		LOG("WARNING: TRYING TO MAP BUFFER, BUT VERTEX BUFFER'S USAGE IS STATIC")
// 	}
// 
// 	set_map_flag(false);
// }
