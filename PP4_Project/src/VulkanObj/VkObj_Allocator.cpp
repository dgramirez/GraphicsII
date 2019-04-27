#include "VkObj_Allocator.h"

VkObj_Allocator::VkObj_Allocator(VkPhysicalDevice &physical_device, VkDevice &device)
	: prv_GarbageIndex(0), prv_DeviceLocalMemory(0), prv_HostVisibleMemory(0), 
	prv_BufferImageGranularity(0), prv_PhysicalDevice(&physical_device), prv_Device(&device) { }

void VkObj_Allocator::init()
{
	prv_DeviceLocalMemory = MB_TO_BYTE(VKDEFINE_DEVICE_LOCAL_MEMORY);
	prv_HostVisibleMemory = MB_TO_BYTE(VKDEFINE_HOST_VISIBLE_MEMORY);

	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(*prv_PhysicalDevice, &properties);
	
	prv_BufferImageGranularity = properties.limits.bufferImageGranularity;
}

void VkObj_Allocator::shutdown()
{
	empty_garbage();
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i)
	{
		std::vector<VkObj_MemoryPool*> *block = &prv_MemoryPools[i];
		for (uint32_t j = 0; j < block->size(); ++j)
			delete block->operator[](j);
		block->clear();
	}
}

void VkObj_Allocator::empty_garbage()
{
	prv_GarbageIndex = (prv_GarbageIndex + 1) % MAX_FRAMES;
	std::vector<VkObj_MemoryBlock> garbage = prv_Garbage[prv_GarbageIndex];

	for (uint32_t i = 0; i < garbage.size(); ++i)
	{
		VkObj_MemoryBlock allocation = garbage[i];
		allocation.pool->free(allocation);

		if (!allocation.pool->prv_Allocated)
		{
			prv_MemoryPools[i].erase(prv_MemoryPools[i].begin() + i);
			delete allocation.pool;
			allocation.pool = nullptr;
		}

	}

	garbage.clear();
}

VkObj_MemoryBlock VkObj_Allocator::allocate(const uint32_t &size, const uint32_t &align, const uint32_t &memory_type_bits, const uint32_t &vkdefine_memory_usage,
	const uint32_t &vkdefine_allocation_type)
{
	//Setup Allocation
	VkObj_MemoryBlock allocation;
	uint32_t memory_type_index = vk_find_memory_type_index(*prv_PhysicalDevice, memory_type_bits, vkdefine_memory_usage);
	if (memory_type_index == MAX_UINT32)
	{
		LOG("Failed to find Memory Type Index, in Allocator.");
	}

	//Tries to allocate from an existing memory pool
	std::vector<VkObj_MemoryPool*> *memory_pool= &prv_MemoryPools[memory_type_index];
	for (uint32_t i = 0; i < memory_pool->size(); ++i)
	{
		VkObj_MemoryPool *block = memory_pool->operator[](i);
		if (block->prv_MemoryTypeIndex != memory_type_index)
			continue;
		if (block->allocate(size, align, prv_BufferImageGranularity, vkdefine_allocation_type, allocation))
			return allocation;
	}

	//Create a new memory pool
	VkDeviceSize new_size = (vkdefine_memory_usage == VKDEFINE_MEMORY_USAGE_GPU_ONLY) ? prv_HostVisibleMemory : prv_DeviceLocalMemory;
	VkObj_MemoryPool *new_pool = new VkObj_MemoryPool(*prv_PhysicalDevice, *prv_Device, memory_type_index, (uint64_t)size, vkdefine_allocation_type);

	if (new_pool->init())
		memory_pool->push_back(new_pool);
	else
	{
		LOG("Failed to push back new memory pool!");
	}

	return allocation;
}

void VkObj_Allocator::free(const VkObj_MemoryBlock & allocation)
{
	prv_Garbage[prv_GarbageIndex].push_back(allocation);
}

VkObj_MemoryPool::VkObj_MemoryPool(VkPhysicalDevice &physical_device, VkDevice &device, const uint32_t &memory_type_index, const VkDeviceSize &size, const uint32_t &vkdefine_allocation_type)
	: prv_PhysicalDevice(&physical_device), prv_Device(&device), prv_MemoryTypeIndex(memory_type_index), prv_Size(size), prv_Usage(vkdefine_allocation_type),
	prv_NextId(0), prv_Allocated(0), prv_DeviceMemory(VK_NULL_HANDLE) {}

VkObj_MemoryPool::~VkObj_MemoryPool() {
	shutdown();
}

bool VkObj_MemoryPool::init()
{
	if (prv_MemoryTypeIndex == MAX_UINT32)
		return false;

	VkMemoryAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.allocationSize = prv_Size;
	allocate_info.memoryTypeIndex = prv_MemoryTypeIndex;

	CHECK_VKRESULT(r, vkAllocateMemory(*prv_Device, &allocate_info, nullptr, &prv_DeviceMemory), "Failed to Allocate Memory in Memory Pool");

	if (!prv_DeviceMemory)
		return false;

	if (prv_Usage != VKDEFINE_MEMORY_USAGE_GPU_ONLY)
	{
		//Cant use CAST(x,y) macro because static_cast fails (void**) conversion from unsigned char pointer address
		CHECK_VKRESULT(hv, vkMapMemory(*prv_Device, prv_DeviceMemory, 0, prv_Size, 0, (void**)&prv_Data), "Failed to Map Data to Memory");
	}

	prv_Head = new mem_block(prv_NextId++, prv_Size);

	return true;
}

void VkObj_MemoryPool::shutdown()
{
	if (prv_Usage != VKDEFINE_MEMORY_USAGE_GPU_ONLY)
		vkUnmapMemory(*prv_Device, prv_DeviceMemory);

	vkFreeMemory(*prv_Device, prv_DeviceMemory, nullptr);
	prv_DeviceMemory = nullptr;

	mem_block *prev = nullptr;
	mem_block *current = prv_Head;

	while (current->next)
	{
		prev = current;
		current = current->next;
		delete prev;
	}

	delete current;

	prv_Head = nullptr;
}

bool VkObj_MemoryPool::allocate(const uint32_t & size, const uint32_t & align, const VkDeviceSize & granularity, const uint32_t & allocation_type, VkObj_MemoryBlock & allocation)
{
	if ((prv_Size - prv_Allocated) < size)
		return false;

	mem_block *current = prv_Head;
	mem_block *previous = nullptr;
	mem_block *best = nullptr;
	
	VkDeviceSize padding = 0;
	VkDeviceSize offset = 0;
	VkDeviceSize aligned_size = 0;

	do {
		if (current->type != VKDEFINE_ALLOCATION_TYPE_FREE)
			continue;
		if (size > current->size)
			continue;

		offset = ALIGN_UINT64(current->offset, align); //ALIGN THE CURRENT OFFSET WITH THE ALIGN PARAMETER!

		if (previous && granularity > 1)
			if (vk_check_page(previous->offset, previous->size, offset, granularity, VKDEFINE_OPERATION_TYPE_EQUAL) && vk_granularity_conflict(previous->type, allocation_type))
				offset = ALIGN_UINT64(current->offset, granularity); //ALIGN THE CURRENT OFFSET WITH THE GRANULARITY VALUE!

		padding = offset - current->offset;
		aligned_size = padding + size;

		if (aligned_size > current->size)
			continue;
		if ((aligned_size + prv_Allocated) >= size)
			return false;
		
		if (current->next && granularity > 1)
			if (vk_check_page(offset, size, current->next->offset, granularity, VKDEFINE_OPERATION_TYPE_EQUAL) && vk_granularity_conflict(allocation_type, current->next->type))
				continue;

		best = current;
		break;

	} while (previous = current, current = current->next);

	if (!best)
		return false;

	if (best->size > size)
	{
		mem_block *new_block = new mem_block(prv_NextId++, 0);
		mem_block *next = best->next;

		new_block->prev = best;
		best->next = new_block;

		new_block->next = next;
		if (next)
			next->prev = new_block;

		new_block->size = best->size - aligned_size;
		new_block->offset = offset + size;
		new_block->type = VKDEFINE_ALLOCATION_TYPE_FREE;
	}

	best->type = allocation_type;
	best->size = size;
	
	prv_Allocated += aligned_size;
	
	allocation.size = best->size;
	allocation.id = best->id;
	allocation.device_memory = prv_DeviceMemory;

	if (prv_Usage != VKDEFINE_MEMORY_USAGE_GPU_ONLY)
		allocation.data = prv_Data + offset;
	
	allocation.offset = offset;
	allocation.pool = this;

	return true;
}

void VkObj_MemoryPool::free(const VkObj_MemoryBlock &allocation)
{
	mem_block *current = prv_Head;
	do {
		if (current->id == allocation.id)
			break;
	} while (current = current->next);

	if (!current) {
		LOG("Failed to find Memory Block!");
		return;
	}

	current->type = VKDEFINE_ALLOCATION_TYPE_FREE;

	if (current->prev && current->prev->type == VKDEFINE_ALLOCATION_TYPE_FREE)
	{
		mem_block *prev = current->prev;

		if (current->next)
			current->next->prev = prev;
		prev->next = current->next;

		prev->size += current->size;

		delete current;
		current = prev;
	}

	if (current->next && current->next->type == VKDEFINE_ALLOCATION_TYPE_FREE)
	{
		mem_block *next = current->next;

		if (next->next)
			next->next->prev = current;
		current->next = next->next;

		current->size += next->size;

		delete next;
	}

	prv_Allocated -= allocation.size;
}
