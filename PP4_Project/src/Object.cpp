#include "Object.h"

Object::Object(const std::vector<Vertex>& vertex, const std::vector<uint32_t>& index_buffer, const Texture& texture)
{
	prv_Vertices = vertex;
	prv_Indices = index_buffer;
	prv_Texture = texture;
}


Object::~Object()
{

}