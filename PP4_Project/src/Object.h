#ifndef OBJECT_H
#define OBJECT_H

#include "Defines.h"
#include "Maths.h"
#include "Assets.h"

class Object
{
public:
	Object() = default;
	Object(const std::vector<Vertex>& vertex, const std::vector<uint32_t>& index_buffer, const Texture& texture);
	~Object();

	std::vector<Vertex> get_vertices() { return prv_Vertices; }
	std::vector<uint32_t> get_indices() { return prv_Indices; }
	const Texture get_texture2d() const { return prv_Texture; }
	void set_vertex(const std::vector<Vertex>& vertex) { prv_Vertices = vertex; }
	void set_index(const std::vector<uint32_t>& index) { prv_Indices = index; }
	void set_texture2d(const Texture& texture2d) { prv_Texture = texture2d; }

	__declspec(property(get = get_vertices, put = set_vertex)) std::vector<Vertex> vertices;
	__declspec(property(get = get_texture2d, put = set_texture2d)) Texture texture;
	__declspec(property(get = get_indices, put = set_index)) std::vector<uint32_t> indices;

	float scale = 1.0f;
protected:
	std::vector<uint32_t> prv_Indices;
	std::vector<Vertex> prv_Vertices;
	Texture prv_Texture;
};

class Object3D : protected Object
{
public:
	Object3D() = default;
	Object3D(const char* fbx_filename);
private:

	const char* prv_TextureFilename;
	void ProcessFbxMesh(FbxNode* node);
	void Compactify(const std::vector<Vertex>& vertex2);
	void SetUVs(FbxArray<FbxVector2>& uv, const FbxMesh* mesh);
	void GetTextureFilename(FbxNode* child_node, const char* return_value);
};

#endif // ifndef OBJECT_H

