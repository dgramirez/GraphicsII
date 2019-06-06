#ifndef OBJECT_MESH_H
#define OBJECT_MESH_H

#include "../Defines.h"
#include "../Maths.h"
#include "../VulkanObj/VkObj_Shared.h"
#include "../VulkanObj/VkObj_Context.h"
#include "Texture.h"

#ifndef __OBJ_VERT__
typedef struct _OBJ_VERT_
{
	float pos[3];
	float uvw[3];
	float nrm[3];
}OBJ_VERT;
#define __OBJ_VERT__
#endif

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);
	Mesh(const char* fbx_filename, float _scale = 1.0f);
	Mesh(const OBJ_VERT *object_vertices, const unsigned int &vertices_size, const unsigned int *object_indices, const unsigned int &indices_size);
	~Mesh();

	//Vertices
	std::vector<Vertex> prv_Vertices;
	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_memory;
	std::vector<Vertex> get_vertices() { return prv_Vertices; }
	__declspec(property(get = get_vertices)) std::vector<Vertex> vertices;

	//Indices
	std::vector<uint32_t> prv_Indices;
	VkBuffer index_buffer;
	VkDeviceMemory index_memory;
	std::vector<uint32_t> get_indices() { return prv_Indices; }
	__declspec(property(get = get_indices)) std::vector<uint32_t> indices;

	const char* get_FBXFilename() { return prv_FBXFilename; }
	__declspec(property(get = get_FBXFilename)) const char *fbx_filepath;
	const char* get_TextureFilename() { return prv_TextureFilename; }
	__declspec(property(get = get_TextureFilename)) const char *texture_filename;

	void init();
	
	float scale;
	glm::mat4 world_matrix;

	void cleanup();
private:

	const char *prv_FBXFilename;
	const char *prv_TextureFilename;

	void ProcessFbxMesh(FbxNode* node);
	void Compactify(const std::vector<Vertex>& vertex2);
	void SetUVs(FbxArray<FbxVector2>& uv, const FbxMesh* mesh);
	void ImportFbx(const char* fbx_filename);
	void GetTextureFilename(FbxNode* child_node, const char* return_value);
	void CreateVertexBuffer();
	void CreateIndexBuffer();
};

#endif