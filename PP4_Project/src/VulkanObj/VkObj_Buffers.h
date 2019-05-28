#ifndef VKOBJ_BUFFERS_H
#define VKOBJ_BUFFERS_H

#include "VkObj_Shared.h"
#include "VkObj_WindowSetup.h"
#include "VkObj_Devices.h"
#include "VkObj_Swapchain.h"
#include "VkObj_Allocator.h"
#include "VkObj_StagingBuffer.h"
#include "../Object.h"


struct VkObj_Buffer
{
	VkBuffer vertex;
	VkDeviceMemory vertex_memory;

	VkBuffer index;
	VkDeviceMemory index_memory;

	std::vector<VkBuffer> uniform;
	std::vector<VkDeviceMemory> uniform_memory;

	VkDescriptorSetLayout descriptor_set_layout;
	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;
};

struct VkObj_CommandBuffer
{
	VkCommandBuffer me;
	std::vector<VkFence> fence;
};

// class VkObj_Buffers
// {
// // public:
// // 	VkObj_Buffers();
// // 	bool init(VkObj_WindowProperties &window_properties, VkObj_DeviceProperties &device_properties, VkObj_Swapchain &swapchain, VkObj_Pools &pools);
// // 
// // 
// // 	VkObj_WindowProperties *pWindowProperties;
// // 	VkObj_DeviceProperties *pDeviceProperties;
// // 	VkObj_Swapchain *pSwapchain;
// // 	VkObj_Pools *pPools;
// // 	VkObj_CommandBuffer command
// // private:
// 
// public:
// 	VkObj_Buffers();
// 	bool init(VkObj_WindowProperties &window_properties, VkObj_DeviceProperties &device_properties, VkObj_Swapchain &swapchain, VkObj_Pools &pools);
// 
// 	int64_t get_size();
// 	int64_t get_allocated_size();
// 	uint64_t get_usage_type();
// 	VkBuffer get_buffer();
// 	int64_t get_offset();
// 	bool is_mapped();
// 
// 	bool CreateCommandBuffer();
// 	bool CreateDepthAndMSAA();
// 	bool CreateSwapchainFrameBuffers(const VkImageView &color_image_view, const VkImageView &depth_buffer_view, const VkRenderPass &render_pass);
// protected:
// 	VkObj_WindowProperties *pWindowProperties;
// 	VkObj_DeviceProperties *pDeviceProperties;
// 	VkObj_Swapchain *pSwapchain;
// 	VkObj_Pools *pPools;
// 	VkObj_CommandBuffer command;
// 
// 	int32_t prv_Size;
// 	int32_t prv_OffsetInOtherBuffer;
// 	uint32_t prv_Usage; //Usages are set as VKDEFINES
// 	VkBuffer prv_Buffer;
// 	VkObj_MemoryBlock prv_Allocation;
// 
// 	void set_map_flag(const bool &on);
// 	bool root_buffer();
// };
// 
// class VkObj_VertexBuffer : public VkObj_Buffers
// {
// public:
// 	VkObj_VertexBuffer();
// 	bool allocate_buffer(const void* data, const uint32_t &allocation_size, const uint32_t &usage);
// 	void free_buffer();
// 
// //	void reference(const VkObj_VertexBuffer &other);
// //	void reference(const VkObj_VertexBuffer &other, uint32_t offset, uint32_t size);
// 
// 	void update(const void* data, const uint32_t &size, const uint32_t &offset);
// 	void* map_buffer(); //Map Types are set as VKDEFINES
// //	Vertex* map_vertex_buffer(uint32_t map_type);
// 	void unmap_buffer();
// 
// private:
// 	void clear_no_free();
// 
// 	//Was Set as a macro, will explicitly define it here;
// 	VkObj_VertexBuffer(const VkObj_VertexBuffer &other);
// 	VkObj_VertexBuffer operator=(const VkObj_VertexBuffer &other);
// };
// 
// class VkObj_IndexBuffer : public VkObj_Buffers
// {
// 	VkObj_IndexBuffer();
// 	bool allocate_buffer(const void* data, const uint32_t &allocation_size, const uint32_t &usage);
// 	void free_buffer();
// 
// 	//	void reference(const VkObj_VertexBuffer &other);
// 	//	void reference(const VkObj_VertexBuffer &other, uint32_t offset, uint32_t size);
// 
// 	void update(const void* data, const uint32_t &size, const uint32_t &offset);
// 	void* map_buffer(); //Map Types are set as VKDEFINES
// //	Vertex* map_vertex_buffer(uint32_t map_type);
// 	void unmap_buffer();
// 
// private:
// 	void clear_no_free();
// 
// 	//Was Set as a macro, will explicitly define it here;
// 	VkObj_IndexBuffer(const VkObj_IndexBuffer &other);
// 	VkObj_IndexBuffer operator=(const VkObj_IndexBuffer &other);
// };
// 
// class VkObj_UniformBuffer : public VkObj_Buffers
// {
// 	VkObj_UniformBuffer();
// 	bool allocate_buffer(const void* data, const uint32_t &allocation_size, const uint32_t &usage);
// 	void free_buffer();
// 
// 	//	void reference(const VkObj_VertexBuffer &other);
// 	//	void reference(const VkObj_VertexBuffer &other, uint32_t offset, uint32_t size);
// 
// 	void update(const void* data, const uint32_t &size, const uint32_t &offset);
// 	void* map_buffer(uint32_t map_type); //Map Types are set as VKDEFINES
// //	Vertex* map_vertex_buffer(uint32_t map_type);
// 	void unmap_buffer();
// 
// private:
// 	void clear_no_free();
// 
// 	//Was Set as a macro, will explicitly define it here;
// 	VkObj_UniformBuffer(const VkObj_UniformBuffer &other);
// 	VkObj_UniformBuffer operator=(const VkObj_UniformBuffer &other);
// };


bool vk_create_vertex_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue,
	std::vector<Object3D> &object_list, VkBuffer &vertex_buffer, VkDeviceMemory &vertex_buffer_memor);
bool vk_create_index_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue,
	std::vector<Object3D> &object_list, VkBuffer &index_buffer, VkDeviceMemory &index_buffer_memory);
bool vk_create_uniform_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const std::vector<VkImage> &swapchain_images,
	std::vector<VkBuffer> &uniform_buffers, std::vector<VkDeviceMemory> &uniform_buffers_memory);
bool vk_update_uniform_buffer(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory);

bool vk_create_descriptor_set_layout(const VkDevice &device, VkDescriptorSetLayout &descriptor_set_layout);
bool vk_create_descriptor_pool(const VkDevice &device, const std::vector<VkImage> &swapchain_images, VkDescriptorPool &descriptor_pool);
bool vk_create_descriptor_sets(const VkDevice &device, const std::vector<VkImage> &swapchain_images, VkDescriptorPool &descriptor_pool,
	const std::vector<VkBuffer> &uniform_buffers, const VkImageView &texture_image_view, const VkSampler &sampler,
	const VkDescriptorSetLayout &descriptor_set_layout, std::vector<VkDescriptorSet> &descriptor_sets);

#endif