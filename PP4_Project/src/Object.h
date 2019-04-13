#ifndef OBJECT_H
#define OBJECT_H

#include "Defines.h"
#include "Maths.h"
#include "Assets.h"

class Object3D
{
public:
	Object3D() = default;
	Object3D(const char* fbx_filename);
	Object3D(const char* fbx_filename, Texture* texture_dot_h);
	Object3D(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, Texture* texture_dot_h);

	~Object3D();

	std::vector<Vertex> get_vertices() { return prv_Vertices; }
	std::vector<uint32_t> get_indices() { return prv_Indices; }
	Texture* get_texture() const { return prv_Texture; }
	void set_vertex(const std::vector<Vertex>& vertex) { prv_Vertices = vertex; }
	void set_index(const std::vector<uint32_t>& index) { prv_Indices = index; }
	void set_texture(Texture* texture2d) { if (prv_Texture->on_heap) delete prv_Texture; prv_Texture = texture2d; }

	__declspec(property(get = get_vertices, put = set_vertex)) std::vector<Vertex> vertices;
	__declspec(property(get = get_texture, put = set_texture)) Texture* texture;
	__declspec(property(get = get_indices, put = set_index)) std::vector<uint32_t> indices;

	float scale = 10.0f;
private:
	std::vector<uint32_t> prv_Indices;
	std::vector<Vertex> prv_Vertices;
	Texture *prv_Texture;
	uint32_t tex2d;
	const char *prv_TextureFilename;
	void ProcessFbxMesh(FbxNode* node);
	void Compactify(const std::vector<Vertex>& vertex2);
	void SetUVs(FbxArray<FbxVector2>& uv, const FbxMesh* mesh);
	void GetTextureFilename(FbxNode* child_node, const char* return_value);
	void ImportFbx(const char* fbx_filename);
};

#endif // ifndef OBJECT_H

