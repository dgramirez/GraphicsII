#ifndef VKOBJ_ALLOCATOR_H
#define VKOBJ_ALLOCATOR_H

#include "VkObj_Shared.h"

struct VkObj_MemoryBlock;

class VkObj_MemoryPool {
	friend class VkObj_Allocator;
public:
	VkObj_MemoryPool(VkPhysicalDevice &physical_device, VkDevice &device, const uint32_t &memory_type_index, const VkDeviceSize &size, const uint32_t &vkdefine_allocation_type);
	~VkObj_MemoryPool();

	bool init();
	void shutdown();

	bool allocate(const uint32_t &size, const uint32_t &align, const VkDeviceSize &granularity, const uint32_t &allocation_type, VkObj_MemoryBlock &allocation);
	void free(const VkObj_MemoryBlock &allocation);

private:
	struct mem_block {
		uint32_t		 id;
		VkDeviceSize	 size,  offset;
		mem_block		*next, *prev;
		uint32_t		 type;

		mem_block(const uint32_t &_id, const VkDeviceSize &_size)
			: id(_id), size(_size), offset(0), prev(nullptr), next(nullptr), type(VKDEFINE_ALLOCATION_TYPE_FREE) {}
	};

	VkPhysicalDevice *prv_PhysicalDevice;
	VkDevice		*prv_Device;
	mem_block		*prv_Head;
	uint32_t		 prv_NextId, prv_MemoryTypeIndex;
	uint32_t		 prv_Usage;
	VkDeviceMemory	 prv_DeviceMemory;
	VkDeviceSize	 prv_Size, prv_Allocated;
	unsigned char*	 prv_Data;
};

class VkObj_Allocator {
public:
	VkObj_Allocator(VkPhysicalDevice &physical_device, VkDevice &device);

	void init();
	void shutdown();
	void empty_garbage();

	VkObj_MemoryBlock allocate(const uint32_t &size, const uint32_t &align, const uint32_t &memory_type_bits, const uint32_t &vkdefine_memory_usage,
		const uint32_t &vkdefine_allocation_type);
	void free(const VkObj_MemoryBlock &allocation);

private:
	VkPhysicalDevice *prv_PhysicalDevice;
	VkDevice *prv_Device;
	VkDeviceSize prv_BufferImageGranularity;
	int32_t prv_GarbageIndex;
	int32_t prv_DeviceLocalMemory; //In MB
	int32_t prv_HostVisibleMemory; //In MB
	std::array<std::vector<VkObj_MemoryPool*>, VK_MAX_MEMORY_TYPES> prv_MemoryPools;
	std::vector<VkObj_MemoryBlock> prv_Garbage[MAX_FRAMES];
};

struct VkObj_MemoryBlock {
	VkObj_MemoryPool   *pool;
	uint32_t			id;
	VkDeviceMemory		device_memory;
	VkDeviceSize		offset;
	VkDeviceSize		size;
	unsigned char*		data;
};

#endif