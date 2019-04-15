#include "VkObj_Allocator.h"

VkObj_Allocation VkObj_Allocator::allocate(const uint32_t & size, const uint32_t & align, const uint32_t & memory_type_bits, const bool & host_visible)
{
	//Tries to allocate from an existing memory pool
	VkObj_Allocation allocation;
	if (Allocate(size, align, memory_type_bits, host_visible, allocation)) {
		return allocation;
	}

	//If not, create a new memory pool
	VkDeviceSize pool_size = host_visible ? prv_HostVisibleMemory : prv_DeviceLocalMemory;
	VkObj_MemoryPool *memory_pool = new VkObj_MemoryPool(prv_NextId++, memory_type_bits, pool_size, host_visible);
	if (memory_pool->init())
		prv_MemoryPools.push_back(memory_pool);
	else {
		LOG("FAILED TO ALLOCATE MEMORY POOL!");
	}

	memory_pool->allocate(size, align, allocation);
	return allocation;
}

bool VkObj_Allocator::Allocate(const uint32_t &size, const uint32_t &align, const uint32_t &memory_type_bits, const bool &need_host_visible, VkObj_Allocation &allocation)
{
	VkPhysicalDeviceMemoryProperties pdevice_memory_properties;
	vkGetPhysicalDeviceMemoryProperties(*physical_device, &pdevice_memory_properties);

	VkMemoryPropertyFlags required = need_host_visible ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 0;
	VkMemoryPropertyFlags preferred = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	//Search for both required and preferred. If nothing, then look for required only.
	for (uint32_t i = 0; i < prv_MemoryPools.size(); ++i)
	{
		if (need_host_visible && !prv_MemoryPools[i]->prv_HostVisible ||
			(!(memory_type_bits >> prv_MemoryPools[i]->prv_MemoryTypeIndex) & 1))
			continue;
		
		VkMemoryPropertyFlags memory_property_flags = pdevice_memory_properties.memoryTypes[prv_MemoryPools[i]->prv_MemoryTypeIndex].propertyFlags;
		if ((memory_property_flags & required) != required ||
			(memory_property_flags & preferred) != preferred)
			continue;

		if (prv_MemoryPools[i]->allocate(size, align, allocation))
			return true;
	}
	for (uint32_t i = 0; i < prv_MemoryPools.size(); ++i)
	{
		if (need_host_visible && !prv_MemoryPools[i]->prv_HostVisible ||
			(!(memory_type_bits >> prv_MemoryPools[i]->prv_MemoryTypeIndex) & 1))
			continue;

		VkMemoryPropertyFlags memory_property_flags = pdevice_memory_properties.memoryTypes[prv_MemoryPools[i]->prv_MemoryTypeIndex].propertyFlags;
		if ((memory_property_flags & required) != required)
			continue;
		if (prv_MemoryPools[i]->allocate(size, align, allocation))
			return true;
	}

	return false;
}


VkObj_MemoryPool::VkObj_MemoryPool(const uint32_t &id, const uint32_t &memory_type_bits, const VkDeviceSize &size, const bool &host_visible)
	: prv_Id(id), prv_NextId(0), prv_Size(size), prv_Allocated(0), prv_HostVisible(host_visible) {
	
	//Needs to be looked at, will fail! Needs to "Find Suitable Memory Type Index" for the pool
	//Note: VkObj_Buffers is a good reference point.
	prv_MemoryTypeIndex = 0;
}

bool VkObj_MemoryPool::init()
{
	if (prv_MemoryTypeIndex == MAX_UINT64)
		return false;

	VkMemoryAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.allocationSize = prv_Size;
	allocate_info.memoryTypeIndex = prv_MemoryTypeIndex;


}
