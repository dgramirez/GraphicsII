#ifndef OBJECT_H
#define OBJECT_H

#include "../Defines.h"
#include "Texture.h"
#include "Mesh.h"
#include "../Camera.h"

class Camera;

class Object
{
public:
	Object() = default;
	Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, Texture* _t = nullptr);
	Object(const char* fbx_filename, const char *texture_folderlocation, float _scale = 1.0f);
	Object(const OBJ_VERT* object_vertices, const unsigned int &vertices_size, const unsigned int* object_indices, const unsigned int & indices_size, Texture* _t = nullptr);
	~Object();

	void init(const uint32_t &sizeof_ubuffer, VkPipelineLayout &graphics_pipeline_layout, VkPipeline &graphic_pipeline);
	void swap_color_format(const VkComponentSwizzle &r, const VkComponentSwizzle &g, const VkComponentSwizzle &b, const VkComponentSwizzle &a);
	void reset();
	void cleanup();
	void create_uniform_buffer();
	void create_descriptor_set();
	void update_uniform_buffer(Camera &camera);
	UniformFctn uniform_function;

	std::vector<VkBuffer> uniform_buffer;	   
	std::vector<VkDeviceMemory> uniform_memory;

	VkPipelineLayout *pipeline_layout;
	VkPipeline *pipeline;
	std::vector<VkDescriptorSet> descriptor_set; 

	Texture* get_texture() { return pTexture; }
	__declspec(property(get = get_texture)) Texture *texture;

	Mesh* get_mesh() { return pMesh; }
	__declspec(property(get = get_mesh)) Mesh *mesh;

	glm::mat4 get_model_matrix() { return prv_ModelMatrix; }
	glm::mat4 get_model_matrix_previous() { return prv_ModelMatrixPrevious; }
	void set_model_matrix(const glm::mat4 &matrix) { prv_ModelMatrixPrevious = prv_ModelMatrix; prv_ModelMatrix = matrix; }
	__declspec(property(get = get_model_matrix, put = set_model_matrix)) glm::mat4 model_matrix;
	__declspec(property(get = get_model_matrix_previous)) glm::mat4 model_matrix_previous;
//	glm::mat4 model_matrix;
private:
	Mesh *pMesh;
	Texture *pTexture;
	glm::mat4 prv_ModelMatrixPrevious;
	glm::mat4 prv_ModelMatrix;

	uint32_t uniform_size_bytes;

//	std::vector<VkCommandBuffer> command_buffer;


};

#endif