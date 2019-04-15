#ifndef VKOBJ_ALLOCATOR_H
#define VKOBJ_ALLOCATOR_H

#include "VkObj_Shared.h"

struct VkObj_Allocation {
	uint32_t		pool_id;
	uint32_t		block_id;
	VkDeviceMemory	device_memory;
	VkDeviceSize	offset;
	VkDeviceSize	size;
	unsigned char*	data;
};

class VkObj_MemoryPool {
	friend class VkObj_Allocator;
public:
	VkObj_MemoryPool(const uint32_t &id, const uint32_t &memory_type_bits, const VkDeviceSize &size, 
		const bool &host_visible);
	~VkObj_MemoryPool();

	bool init();
	void shutdown();

	bool allocate(const uint32_t &size, const uint32_t &align, VkObj_Allocation &allocation);
	void free(const VkObj_Allocation &allocation);

private:
	struct mem_block {
		uint32_t		 id;
		VkDeviceSize	 size,  offset;
		mem_block		*next, *prev;
		bool			 is_free;
	};

	mem_block		*prv_Head;
	uint32_t		 prv_Id, prv_NextId, prv_MemoryTypeIndex;
	bool			 prv_HostVisible;
	VkDeviceMemory	 prv_DeviceMemory;
	VkDeviceSize	 prv_Size, prv_Allocated;
	unsigned char*	 prv_Data;
};

class VkObj_Allocator {
public:
	VkObj_Allocator(const VkPhysicalDevice &physical_device);

	void init();
	void empty_garbage();

	VkObj_Allocation allocate(const uint32_t &size, const uint32_t &align, const uint32_t &memory_type_bits,
		const bool &host_visible);
	void free(const VkObj_Allocation &allocation);

private:
	VkPhysicalDevice *physical_device;
	int32_t prv_NextId;
	int32_t prv_GarbageIndex;
	int32_t prv_DeviceLocalMemory; //In MB
	int32_t prv_HostVisibleMemory; //In MB
	std::vector<VkObj_MemoryPool*> prv_MemoryPools;
	std::vector<VkObj_Allocation> prv_Garbage[MAX_FRAMES_FLIGHT];

	bool Allocate(const uint32_t &size, const uint32_t &align, const uint32_t &memory_type_bits,
		const bool &need_host_visible, VkObj_Allocation &allocation);
};

#endif