#ifndef STAGING_BUFFER_H
#define STAGING_BUFFER_H

#include "VkObj_Shared.h"

struct VkStruct_StagingBuffer 
{
	bool submitted;
	VkCommandBuffer command_buffer;
	VkBuffer buffer;
	VkFence fence;
	VkDeviceSize offset;
	char* data;

	VkStruct_StagingBuffer()
		: submitted(false), command_buffer(0), buffer(0), fence(0), offset(0), data(nullptr) {}
};

class VkObj_StagingManager
{
public:
	VkObj_StagingManager();
	~VkObj_StagingManager();

	bool init(VkSurfaceKHR &surface, VkPhysicalDevice &physical_device, VkDevice &device, VkQueue &graphics_queue);
	void shutdown();

	char* stage(const uint32_t &size, const uint32_t &alignment, VkCommandBuffer &command_buffer, VkBuffer &buffer, uint64_t &buffer_offset);
	void flush();

private:
	bool init();
	VkSurfaceKHR *prv_Surface;
	VkPhysicalDevice *prv_PhysicalDevice;
	VkDevice *prv_Device;
	VkQueue *prv_GraphicsQueue;
	uint32_t prv_MaxBufferSize;
	uint32_t prv_CurrentBuffer;
	char* prv_MappedData;
	VkDeviceMemory prv_Memory;
	VkCommandPool prv_CommandPool;
	VkStruct_StagingBuffer prv_Buffers[MAX_FRAMES];

	void wait(VkStruct_StagingBuffer &staging_buffer);
	void align(const uint64_t &value, const uint64_t & alignment, uint64_t &changeme);
};

extern VkObj_StagingManager StageManager;

#endif