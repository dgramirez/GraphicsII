#ifndef OBJECT_H
#define OBJECT_H

#include "../../Maths.h"
#include "Texture.h"
#include "Mesh.h"

class VkObj_Context;
class Object;
class Camera;

typedef void(*UniformFctn)(const VkObj_Context &context, Object &obj, Camera &camera);

class Camera;

class Object
{
public:
	Object() = default;
	Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, const uint32_t &pipeline_index, const uint32_t &vs_ubuffer_size, Texture* _t = nullptr);
	Object(const OBJ_VERT* object_vertices, const unsigned int &vertices_size, const unsigned int* object_indices, const unsigned int & indices_size, const uint32_t &pipeline_index, const uint32_t &vs_ubuffer_size, Texture* _t = nullptr);
	Object(
		const char *fmd_filename, const uint32_t &pipeline_index, const uint32_t &pipeline_mask, const uint32_t &vs_ubuffer_size, const uint32_t &gs_ubuffer_size = 0,
		const char *color_map_filename = nullptr, const char *normal_map_filename = nullptr,
		const char *ambient_map_filename = nullptr, const char *specular_map_filename = nullptr
	);
	~Object();

	void init();
	void clear();
	void reset();
	void cleanup();
	
	void create_uniform_buffer(std::vector<VkBuffer> &uniform_buffer, std::vector<VkDeviceMemory> &uniform_memory, const uint32_t & uniform_size);
	void create_descriptor_set();
	void create_descriptor_set2();

	void create_descriptor_set_nm();

	void swap_color_format(const VkComponentSwizzle &r, const VkComponentSwizzle &g, const VkComponentSwizzle &b, const VkComponentSwizzle &a);
	void update_uniform_buffer(Camera &camera);
	UniformFctn uniform_function;

	std::vector<VkBuffer> vs_uniform_buffer;	   
	std::vector<VkDeviceMemory> vs_uniform_memory;
	std::vector<VkBuffer> gs_uniform_buffer;
	std::vector<VkDeviceMemory> gs_uniform_memory;
	std::vector<VkDescriptorSet> descriptor_set;

	uint32_t get_pipeline_index() { return prv_PipelineIndex; }
	__declspec(property(get = get_pipeline_index)) uint32_t pipeline_index;

	Texture* get_texture() { return pColorMap; }
	__declspec(property(get = get_texture)) Texture *color_map;

	Texture* get_normalmap() { return pNormalMap; }
	__declspec(property(get = get_normalmap)) Texture *normal_map;

	Mesh* get_mesh() { return pMesh; }
	__declspec(property(get = get_mesh)) Mesh *mesh;

	float get_scale() { return prv_Scale; }
	__declspec(property(get = get_scale)) float scale;

	glm::mat4 get_model_matrix() { return prv_ModelMatrix; }
	glm::mat4 get_model_matrix_previous() { return prv_ModelMatrixPrevious; }
	void set_model_matrix(const glm::mat4 &matrix) { prv_ModelMatrixPrevious = prv_ModelMatrix; prv_ModelMatrix = matrix; }
	__declspec(property(get = get_model_matrix, put = set_model_matrix)) glm::mat4 model_matrix;
	__declspec(property(get = get_model_matrix_previous)) glm::mat4 model_matrix_previous;

//	glm::mat4 model_matrix;
private:
	Mesh *pMesh;
	Texture *pColorMap;
	Texture *pNormalMap;
	Texture *pAmbientMap;
	Texture *pSpecularMap;
	glm::mat4 prv_ModelMatrixPrevious;
	glm::mat4 prv_ModelMatrix;

	uint32_t prv_PipelineIndex;
	uint32_t prv_PipelineMask;
	uint32_t prv_VSUniformBufferSize;
	uint32_t prv_GSUniformBufferSize;
	float prv_Scale;

	void PushBackImageSampler(std::vector<VkWriteDescriptorSet> &write_descriptor_sets, const VkDescriptorSet &descriptor_set, const VkImageView &image_view, const VkSampler &sampler, const VkDescriptorType &descriptor_type, const uint32_t &binding_index, VkDescriptorImageInfo &image_info, VkWriteDescriptorSet &set);
	void PushBackUniformBuffer(std::vector<VkWriteDescriptorSet> &write_descriptor_sets, const VkDescriptorSet &descriptor_set, const VkBuffer &uniform_buffer, const uint32_t &uniform_structure_size, const VkDescriptorType &descriptor_type, const uint32_t &binding_index, VkDescriptorBufferInfo &buffer_info, VkWriteDescriptorSet &set);
};

#endif