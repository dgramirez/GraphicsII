#ifndef VKOBJ_IMAGES_H
#define VKOBJ_IMAGES_H

#include "VkObj_Shared.h"
#include "VkObj_Allocator.h"
#include "VkObj_StagingBuffer.h"
#include "../Assets.h"

class VkObj_Image
{
public:
	VkObj_Image(const char* _name, const Texture &_texture, VkDevice &device, VkFormat &depth_buffer, VkSampleCountFlagBits &msaa, bool purge = true);

	void CreateSampler();
	static void EmptyGarbage();
	void AllocateImage();
	void PurgeImage();
	void SubImageUpload(const uint32_t &mip_level, const int32_t &offset_x, const int32_t &offset_y, const uint32_t &offset_z, const uint32_t &width, const uint32_t &height, const void* pixels, const uint32_t &pitch);

	const char* name;
	VkImage image;
	VkImageView view;
	VkFormat format;
	VkComponentMapping  component_map;
	VkSampler sampler;
	VkSamplerAddressMode sampler_repeat;
	VkFilter filter;
	VkImageTiling tiling;
	VkImageUsageFlags usage_flags;
	bool do_not_purge;
	Texture texture;
	VkObj_MemoryBlock memory_block;
	VkImageLayout layout;
private:
	static VkDevice *prv_Device;
	static VkFormat *prv_DepthBuffer;
	static VkSampleCountFlagBits *prv_MSAABuffer;
	static uint32_t prv_GarbageIndex;
	static std::vector<VkObj_MemoryBlock> prv_MemoryGarbage[MAX_FRAMES];
	static std::vector<VkImage> prv_ImageGarbage[MAX_FRAMES];
	static std::vector<VkImageView> prv_ImageViewGarbage[MAX_FRAMES];
	static std::vector<VkSampler> prv_SamplerGarbage[MAX_FRAMES];

};

#endif