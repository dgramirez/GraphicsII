#ifndef OBJECT_H
#define OBJECT_H

#include "Defines.h"
#include "Maths.h"
#include "Assets.h"
#include "VulkanObj/VkObj_Shared.h"
#include "VulkanObj/VkObj_Devices.h"

class Object3D
{
public:
	Object3D() = default;
	Object3D(const char* fbx_filename, Texture* texture_dot_h = nullptr, float _scale = 10);
	Object3D(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, Texture* texture_dot_h);
	~Object3D();

	std::vector<Vertex> get_vertices() { return prv_Vertices; }
	__declspec(property(get = get_vertices)) std::vector<Vertex> vertices;
	
	std::vector<uint32_t> get_indices() { return prv_Indices; }
	__declspec(property(get = get_indices)) std::vector<uint32_t> indices;
	
	Texture* get_texture() const { return prv_Texture; }
	__declspec(property(get = get_texture)) Texture* texture;
	
	VkImage image;
	VkImageView image_view = nullptr;
	VkDeviceMemory image_memory;

	VkComponentSwizzle component_r = VK_COMPONENT_SWIZZLE_IDENTITY;
	VkComponentSwizzle component_g = VK_COMPONENT_SWIZZLE_IDENTITY;
	VkComponentSwizzle component_b = VK_COMPONENT_SWIZZLE_IDENTITY;
	VkComponentSwizzle component_a = VK_COMPONENT_SWIZZLE_IDENTITY;

	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_memory;

	VkBuffer index_buffer;
	VkDeviceMemory index_memory;

	std::vector<VkBuffer> uniform_buffer;
	std::vector<VkDeviceMemory> uniform_memory;

	uint32_t ubuffer_range;
	VkSampler sampler;
	std::vector<VkDescriptorSet> descriptor_set;
	VkPipelineLayout *pipeline_layout;
	VkPipeline *pipeline;
	uint32_t swapchain_size;
	VkDescriptorPool descriptor_pool;

	std::vector<VkCommandBuffer> command_buffer;

	static void set_static_contexts(VkObj_DeviceProperties &device, VkCommandPool &command_pool);
	void init(const uint32_t &sizeof_ubuffer,
		VkPipelineLayout &graphics_pipeline_layout, VkPipeline &graphic_pipeline, const uint32_t &swapchain_vec_size);
	void set_image_view(const VkComponentSwizzle &red = VK_COMPONENT_SWIZZLE_IDENTITY, const VkComponentSwizzle &green = VK_COMPONENT_SWIZZLE_IDENTITY, const VkComponentSwizzle &blue = VK_COMPONENT_SWIZZLE_IDENTITY, const VkComponentSwizzle &alpha = VK_COMPONENT_SWIZZLE_IDENTITY);

	bool CreateDescriptorSet();
	bool CreateDescriptorSet(const VkDescriptorPool &descriptor_pool, const VkDescriptorSetLayout &descriptor_set_layout);
	void CreateUniformBuffer();

	void(*uniformFctn)(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view);
	void UpdateUniformBuffer(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view);
	float scale;
	glm::mat4 world_matrix;

	void reset();
	void cleanup();
private:
	std::vector<uint32_t> prv_Indices;
	std::vector<Vertex> prv_Vertices;
	Texture *prv_Texture;
	uint32_t tex2d;
	static VkObj_DeviceProperties *pDevice;
	static VkCommandPool *pCommandPool;
	static VkDescriptorSetLayout *pDescriptorSetLayout;
	const char *prv_TextureFilename;
	void ProcessFbxMesh(FbxNode* node);
	void Compactify(const std::vector<Vertex>& vertex2);
	void SetUVs(FbxArray<FbxVector2>& uv, const FbxMesh* mesh);
	void GetTextureFilename(FbxNode* child_node, const char* return_value);
	void ImportFbx(const char* fbx_filename);
	void CreateImage();
	void CreateSampler();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	bool CreateDescriptorPool();

};

#endif // ifndef OBJECT_H

