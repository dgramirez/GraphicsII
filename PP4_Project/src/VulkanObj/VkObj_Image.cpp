#include "VkObj_Image.h"


VkDevice *VkObj_Image::prv_Device = nullptr;
VkFormat *VkObj_Image::prv_DepthBuffer = nullptr;
VkSampleCountFlagBits *VkObj_Image::prv_MSAABuffer = nullptr;
uint32_t VkObj_Image::prv_GarbageIndex = 0;
std::vector<VkObj_MemoryBlock> VkObj_Image::prv_MemoryGarbage[MAX_FRAMES];
std::vector<VkImage> VkObj_Image::prv_ImageGarbage[MAX_FRAMES];
std::vector<VkImageView> VkObj_Image::prv_ImageViewGarbage[MAX_FRAMES];
std::vector<VkSampler> VkObj_Image::prv_SamplerGarbage[MAX_FRAMES];

VkObj_Image::VkObj_Image(const char* _name, const Texture &_texture, VkDevice &device, VkFormat &depth_buffer, VkSampleCountFlagBits &msaa, bool purge)
	: name(_name), do_not_purge(purge), texture(_texture),
	image(nullptr), view(nullptr), sampler(nullptr), sampler_repeat(VK_SAMPLER_ADDRESS_MODE_REPEAT), 
	format(VK_FORMAT_UNDEFINED), filter(VK_FILTER_LINEAR), usage_flags(0)
{
	component_map.r = VK_COMPONENT_SWIZZLE_R;
	component_map.g = VK_COMPONENT_SWIZZLE_G;
	component_map.b = VK_COMPONENT_SWIZZLE_B;
	component_map.a = VK_COMPONENT_SWIZZLE_A;

	prv_Device = &device;
	prv_DepthBuffer = &depth_buffer;
	prv_MSAABuffer = &msaa;
}

void VkObj_Image::CreateSampler()
{
	VkSamplerCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	create_info.addressModeU = sampler_repeat;
	create_info.addressModeV = sampler_repeat;
	create_info.addressModeW = sampler_repeat;
	create_info.anisotropyEnable = VK_FALSE;
	create_info.maxAnisotropy = 1;
	create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	create_info.unnormalizedCoordinates = false;
	create_info.mipLodBias = 0.0f;
	create_info.minLod = 0.0f;
	create_info.maxLod = 0.0f;
	create_info.compareEnable = (format == *prv_DepthBuffer);
	create_info.compareOp = (format == *prv_DepthBuffer) ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_NEVER;
	switch (filter)
	{
	case VK_FILTER_LINEAR:
		create_info.magFilter = VK_FILTER_LINEAR;
		create_info.minFilter = VK_FILTER_LINEAR;
		create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case VK_FILTER_NEAREST:
		create_info.magFilter = VK_FILTER_NEAREST;
		create_info.minFilter = VK_FILTER_NEAREST;
		create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	}

	vkCreateSampler(*prv_Device, &create_info, nullptr, &sampler);
}

void VkObj_Image::EmptyGarbage()
{
	prv_GarbageIndex = (prv_GarbageIndex + 1) % MAX_FRAMES;

	for (uint32_t i = 0; i < prv_MemoryGarbage->size(); ++i)
		Allocator.free(prv_MemoryGarbage[prv_GarbageIndex][i]);
	prv_MemoryGarbage->clear();

	for (uint32_t i = 0; i < prv_ImageGarbage->size(); ++i)
		vkDestroyImage(*prv_Device, prv_ImageGarbage[prv_GarbageIndex][i], nullptr);
	prv_ImageGarbage->clear();

	for (uint32_t i = 0; i < prv_ImageViewGarbage->size(); ++i)
		vkDestroyImageView(*prv_Device, prv_ImageViewGarbage[prv_GarbageIndex][i], nullptr);
	prv_ImageViewGarbage->clear();

	for (uint32_t i = 0; i < prv_SamplerGarbage->size(); ++i)
		vkDestroySampler(*prv_Device, prv_SamplerGarbage[prv_GarbageIndex][i], nullptr);
	prv_SamplerGarbage->clear();
}

void VkObj_Image::AllocateImage()
{
	CreateSampler();

	if (format == *prv_DepthBuffer)
		usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	else
		usage_flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	VkImageCreateInfo image_create_info = {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.extent = { texture.width, texture.height, 1 };
	image_create_info.mipLevels = texture.mip_levels;
	image_create_info.arrayLayers = 1;
	image_create_info.format = format;
	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_create_info.usage = usage_flags;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_create_info.samples = *prv_MSAABuffer;
	image_create_info.flags = 0;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateImage(*prv_Device, &image_create_info, nullptr, &image);

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(*prv_Device, image, &memory_requirements);

	memory_block = Allocator.allocate(memory_requirements.size, memory_requirements.alignment, memory_requirements.memoryTypeBits, VKDEFINE_MEMORY_USAGE_GPU_ONLY, VKDEFINE_ALLOCATION_TYPE_IMAGE_OPTIMAL);
	vkBindImageMemory(*prv_Device, image, memory_block.device_memory, memory_block.offset);

	VkImageViewCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = format;
	create_info.subresourceRange.aspectMask = (format == *prv_DepthBuffer) ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	create_info.subresourceRange.baseMipLevel = 0;
	create_info.subresourceRange.levelCount = texture.mip_levels;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;

	vkCreateImageView(*prv_Device, &create_info, nullptr, &view);
}

void VkObj_Image::PurgeImage()
{
	if (sampler)
	{
		prv_SamplerGarbage->push_back(sampler);
		sampler = nullptr;
	}

	if (image)
	{
		prv_MemoryGarbage->push_back(memory_block);
		memory_block = VkObj_MemoryBlock();
		prv_ImageGarbage->push_back(image);
		image = nullptr;
		prv_ImageViewGarbage->push_back(view);
		view = nullptr;
	}
}

void VkObj_Image::SubImageUpload(const uint32_t &mip_level, const int32_t &offset_x, const int32_t &offset_y, const uint32_t &offset_z, const uint32_t &width, const uint32_t &height, const void* pixels, const uint32_t &pitch)
{
	uint32_t size = (width * height) * (format >> 3);

	VkBuffer buffer;
	VkCommandBuffer command_buffer;
	uint64_t offset;

	char* data = StageManager.stage(size, 16, command_buffer, buffer, offset);
	memcpy(data, pixels, size);

	VkBufferImageCopy buffer_image_copy = {};
	buffer_image_copy.bufferOffset = offset;
	buffer_image_copy.bufferRowLength = pitch;
	buffer_image_copy.bufferImageHeight = height;
	buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	buffer_image_copy.imageSubresource.layerCount = 1;
	buffer_image_copy.imageSubresource.mipLevel = mip_level;
	buffer_image_copy.imageSubresource.baseArrayLayer = offset_z;
	buffer_image_copy.imageOffset = { offset_x, offset_y, 0 };
	buffer_image_copy.imageExtent = { width, height, 1 };

	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.image = image;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = texture.mip_levels;
	image_memory_barrier.subresourceRange.baseArrayLayer = offset_z;
	image_memory_barrier.subresourceRange.layerCount = 1;
	
	image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	image_memory_barrier.srcAccessMask = 0;
	image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	vkCmdPipelineBarrier(command_buffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier);

	vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);

	image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(command_buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier);

	layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}
