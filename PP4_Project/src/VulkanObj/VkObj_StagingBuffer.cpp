#include "VkObj_StagingBuffer.h"

VkObj_StagingManager::VkObj_StagingManager(VkSurfaceKHR &surface, VkPhysicalDevice &physical_device, VkDevice &device, VkQueue &graphics_queue)
	: prv_Surface(&surface), prv_PhysicalDevice(&physical_device), prv_Device(&device), prv_GraphicsQueue(&graphics_queue),
	prv_MaxBufferSize(0), prv_CurrentBuffer(0), prv_MappedData(nullptr), prv_Memory(0), prv_CommandPool(0) { }

VkObj_StagingManager::~VkObj_StagingManager() = default;

void VkObj_StagingManager::align(const uint64_t &value, const uint32_t & alignment, uint64_t &changeme)
{
	uint32_t align_mod = value % alignment;
	changeme = !align_mod ? value : (value + alignment - align_mod);
}

bool VkObj_StagingManager::init()
{
	prv_MaxBufferSize = MB_TO_BYTE(VKDEFINE_GPU_UPLOAD_BUFFER_SIZE);

	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = prv_MaxBufferSize;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	for (uint32_t i = 0; i < MAX_FRAMES; ++i) { 
		prv_Buffers[i].offset = 0;
		CHECK_VKRESULT(a, vkCreateBuffer(*prv_Device, &buffer_create_info, nullptr, &prv_Buffers[i].buffer), "");
	}

	VkMemoryRequirements memory_requirement;
	vkGetBufferMemoryRequirements(*prv_Device, prv_Buffers[0].buffer, &memory_requirement);

	uint64_t alignment_size;
	align(memory_requirement.size, memory_requirement.alignment, alignment_size);

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = alignment_size * MAX_FRAMES;
	memory_allocate_info.memoryTypeIndex = vk_find_memory_type_index(*prv_PhysicalDevice, memory_requirement.memoryTypeBits, VKDEFINE_MEMORY_USAGE_CPU_TO_GPU);

	CHECK_VKRESULT(b, vkAllocateMemory(*prv_Device, &memory_allocate_info, nullptr, &prv_Memory), "");

	for (uint32_t i = 0; i < MAX_FRAMES; ++i) {
		CHECK_VKRESULT(c, vkBindBufferMemory(*prv_Device, prv_Buffers[i].buffer, prv_Memory, i * alignment_size), "");
	}

	CHECK_VKRESULT(d, vkMapMemory(*prv_Device, prv_Memory, 0, alignment_size * MAX_FRAMES, 0, (void**)&prv_MappedData), "");

	QueueFamilyIndices queue_family_indices = vk_find_queue_family(*prv_PhysicalDevice, *prv_Surface);

	VkCommandPoolCreateInfo command_pool_create_info = {};
	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.queueFamilyIndex = queue_family_indices.graphics.value();
	vkCreateCommandPool(*prv_Device, &command_pool_create_info, nullptr, &prv_CommandPool);

	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = prv_CommandPool;
	command_buffer_allocate_info.commandBufferCount = 1;

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
	{
		vkAllocateCommandBuffers(*prv_Device, &command_buffer_allocate_info, &prv_Buffers[i].command_buffer);
		vkCreateFence(*prv_Device, &fence_create_info, nullptr, &prv_Buffers[i].fence);
		vkBeginCommandBuffer(prv_Buffers[i].command_buffer, &command_buffer_begin_info);

		prv_Buffers[i].data = (char*)prv_MappedData + (i + alignment_size);
	}
}

void VkObj_StagingManager::shutdown()
{
	vkUnmapMemory(*prv_Device, prv_Memory);
	prv_MappedData = nullptr;

	vkFreeMemory(*prv_Device, prv_Memory, nullptr);
	prv_Memory = nullptr;

	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
	{
		vkDestroyFence(*prv_Device, prv_Buffers[i].fence, nullptr);
		vkDestroyBuffer(*prv_Device, prv_Buffers[i].buffer, nullptr);
		vkFreeCommandBuffers(*prv_Device, prv_CommandPool, 1, &prv_Buffers[i].command_buffer);
	}
	memset(prv_Buffers, 0, sizeof(prv_Buffers));

	vkDestroyCommandPool(*prv_Device, prv_CommandPool, nullptr);
	prv_CommandPool = nullptr;

	prv_MaxBufferSize = 0;
	prv_CurrentBuffer = 0;
}

char* VkObj_StagingManager::stage(const uint32_t &size, const uint32_t &alignment, VkCommandBuffer &command_buffer, VkBuffer &buffer, uint32_t &buffer_offset)
{
	CHECK_RESULT_NO_RETURN((size > prv_MaxBufferSize), "ERROR: VkObj_StagingManager stage function has a size that is greater than Max Buffer Size");

	VkStruct_StagingBuffer *stage_buffer = &prv_Buffers[prv_CurrentBuffer];
	align(stage_buffer->offset, alignment, stage_buffer->offset);

	if ( ( (stage_buffer->offset + size) >= prv_MaxBufferSize ) && !stage_buffer->submitted)
		flush();

	stage_buffer = &prv_Buffers[prv_CurrentBuffer];
	if (stage_buffer->submitted)
		wait(*stage_buffer);

	command_buffer = stage_buffer->command_buffer;
	buffer = stage_buffer->buffer;
	buffer_offset = stage_buffer->offset;

	char* data = stage_buffer->data + stage_buffer->offset;
	stage_buffer->offset += size;

	return data;
}

void VkObj_StagingManager::flush()
{
	VkStruct_StagingBuffer *staging_buffer = &prv_Buffers[prv_CurrentBuffer];
	if (staging_buffer->submitted || !staging_buffer->offset)
		return;

	VkMemoryBarrier memory_barrier = {};
	memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	memory_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
	vkCmdPipelineBarrier(
		staging_buffer->command_buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		0,
		1, &memory_barrier,
		0, nullptr,
		0, nullptr
	);

	vkEndCommandBuffer(staging_buffer->command_buffer);

	VkMappedMemoryRange memory_range = {};
	memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memory_range.memory = prv_Memory;
	memory_range.size = MAX_UINT64;
	vkFlushMappedMemoryRanges(*prv_Device, 1, &memory_range);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &staging_buffer->command_buffer;

	vkQueueSubmit(*prv_GraphicsQueue, 1, &submit_info, staging_buffer->fence);

	staging_buffer->submitted = true;

	prv_CurrentBuffer = (prv_CurrentBuffer + 1) % MAX_FRAMES;
}

void VkObj_StagingManager::wait(VkStruct_StagingBuffer &staging_buffer)
{
	if (!staging_buffer.submitted)
		return;

	vkWaitForFences(*prv_Device, 1, &staging_buffer.fence, true, MAX_UINT64);
	vkResetFences(*prv_Device, 1, &staging_buffer.fence);
	staging_buffer.offset = 0;
	staging_buffer.submitted = 0;

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(staging_buffer.command_buffer, &command_buffer_begin_info);
}
