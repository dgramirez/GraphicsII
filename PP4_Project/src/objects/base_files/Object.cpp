#include "pch.h"
#include "Object.h"

//   _____                         _                           _
//  / ____|                       | |                         | |
// | |        ___    _ __    ___  | |_   _ __   _   _    ___  | |_    ___    _ __
// | |       / _ \  | '_ \  / __| | __| | '__| | | | |  / __| | __|  / _ \  | '__|
// | |____  | (_) | | | | | \__ \ | |_  | |    | |_| | | (__  | |_  | (_) | | |
//  \_____|  \___/  |_| |_| |___/  \__| |_|     \__,_|  \___|  \__|  \___/  |_|
Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, Texture* _t)
{
	pMesh = new Mesh(vertices, indices);
	pTexture = _t;
	prv_Scale = 1.0f;
}

Object::Object(const char* fbx_filename, const char *texture_folderlocation, float _scale, Texture* _t)
{
	prv_Scale = _scale;
	pMesh = new Mesh(fbx_filename, _scale);

	if (!_t)
	{
		char texture_filepath[255];
		strcpy(texture_filepath, texture_folderlocation);
		strcat(texture_filepath, pMesh->texture_filename);

		uint32_t len = (uint32_t)strlen(texture_filepath);
		char *fp = new char[len + 1];
		strcpy_s(fp, len + 1, texture_filepath);
		pTexture = new Texture(fp);
		delete[] fp;
	}
	else
		pTexture = _t;
}

Object::Object(const OBJ_VERT* object_vertices, const unsigned int &vertices_size, const unsigned int* object_indices, const unsigned int & indices_size, Texture* _t)
{
	pMesh = new Mesh(object_vertices, vertices_size, object_indices, indices_size);
	pTexture = _t;
	prv_Scale = 1.0f;
}

Object::~Object()
{
	cleanup();
}

//  _____            _       _   _
// |  __ \          | |     | | (_)
// | |__) |  _   _  | |__   | |  _    ___
// |  ___/  | | | | | '_ \  | | | |  / __|
// | |      | |_| | | |_) | | | | | | (__
// |_|       \__,_| |_.__/  |_| |_|  \___|
void Object::init(const uint32_t &sizeof_ubuffer, const uint32_t &pipe_index)
{
	prv_ModelMatrixPrevious = glm::mat4(1.0f);
	pMesh->init();
	pTexture->init();

	pipeline_index = pipe_index;
	uniform_size_bytes = sizeof_ubuffer;
	create_uniform_buffer();
	create_descriptor_set();
}

void Object::swap_color_format(const VkComponentSwizzle &r, const VkComponentSwizzle &g, const VkComponentSwizzle &b, const VkComponentSwizzle &a)
{
	pTexture->swizzle(r, g, b, a);
}

void Object::reset()
{
	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();
	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		if (uniform_buffer.empty())
			break;

		vkDestroyBuffer(myContext.device.logical, uniform_buffer[i], nullptr);
		vkFreeMemory(myContext.device.logical, uniform_memory[i], nullptr);
	}
	uniform_buffer.clear();
	uniform_memory.clear();
}

void Object::cleanup()
{
	if (pTexture)
	{
		delete pTexture;
		pTexture = nullptr;
	}

	if (pMesh)
	{
		delete pMesh;
		pMesh = nullptr;
	}

	reset();
}

//  _____           _                   _
// |  __ \         (_)                 | |
// | |__) |  _ __   _  __   __   __ _  | |_    ___
// |  ___/  | '__| | | \ \ / /  / _` | | __|  / _ \
// | |      | |    | |  \ V /  | (_| | | |_  |  __/
// |_|      |_|    |_|   \_/    \__,_|  \__|  \___|
void Object::create_uniform_buffer()
{
	VkDeviceSize buffer_size = uniform_size_bytes;
	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();

	uniform_buffer.resize(swapchain_size);
	uniform_memory.resize(swapchain_size);

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		vk_create_buffer(myContext.device.physical, myContext.device.logical, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniform_buffer[i], uniform_memory[i]);
	}
}

void Object::create_descriptor_set()
{
	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();
	std::vector<VkDescriptorSetLayout> descriptor_set_layout_vector(swapchain_size, myContext.pipelines.pipelines[pipeline_index].descriptor_set_layout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate_info.descriptorSetCount = swapchain_size;
	descriptor_set_allocate_info.descriptorPool = myContext.pipelines.pipelines[pipeline_index].descriptor_pool;
	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout_vector.data();

	descriptor_set.resize(swapchain_size);
	vkAllocateDescriptorSets(myContext.device.logical, &descriptor_set_allocate_info, descriptor_set.data());

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		VkDescriptorBufferInfo descriptor_buffer_info = {};
		descriptor_buffer_info.buffer = uniform_buffer[i];
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = uniform_size_bytes;

		VkDescriptorImageInfo descriptor_image_info = {};
		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_image_info.imageView = pTexture->image_view;
		descriptor_image_info.sampler = pTexture->sampler;

		std::array<VkWriteDescriptorSet, 2> write_descriptor_set;
		write_descriptor_set[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[0].dstSet = descriptor_set[i];
		write_descriptor_set[0].dstBinding = 0;
		write_descriptor_set[0].dstArrayElement = 0;
		write_descriptor_set[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor_set[0].descriptorCount = 1;
		write_descriptor_set[0].pBufferInfo = &descriptor_buffer_info;
		write_descriptor_set[0].pImageInfo = nullptr;
		write_descriptor_set[0].pTexelBufferView = nullptr;
		write_descriptor_set[0].pNext = nullptr;

		write_descriptor_set[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[1].dstSet = descriptor_set[i];
		write_descriptor_set[1].dstBinding = 1;
		write_descriptor_set[1].dstArrayElement = 0;
		write_descriptor_set[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptor_set[1].descriptorCount = 1;
		write_descriptor_set[1].pBufferInfo = nullptr;
		write_descriptor_set[1].pImageInfo = &descriptor_image_info;
		write_descriptor_set[1].pTexelBufferView = nullptr;
		write_descriptor_set[1].pNext = nullptr;

		vkUpdateDescriptorSets(myContext.device.logical, CAST(uint32_t, write_descriptor_set.size()), write_descriptor_set.data(), 0, nullptr);
	}

}

void Object::update_uniform_buffer(Camera &camera)
{
	if (uniform_function)
		uniform_function(myContext, *this, camera);
}
