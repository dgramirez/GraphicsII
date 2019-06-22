#include "pch.h"
#include "Object.h"

//   _____                         _                           _
//  / ____|                       | |                         | |
// | |        ___    _ __    ___  | |_   _ __   _   _    ___  | |_    ___    _ __
// | |       / _ \  | '_ \  / __| | __| | '__| | | | |  / __| | __|  / _ \  | '__|
// | |____  | (_) | | | | | \__ \ | |_  | |    | |_| | | (__  | |_  | (_) | | |
//  \_____|  \___/  |_| |_| |___/  \__| |_|     \__,_|  \___|  \__|  \___/  |_|
Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, const uint32_t &pipeline_index, const uint32_t &vs_ubuffer_size, Texture* _t)
{
	pMesh = new Mesh(vertices, indices);
	pColorMap = _t;
	prv_Scale = 1.0f;

	prv_PipelineIndex = pipeline_index;
	prv_VSUniformBufferSize = vs_ubuffer_size;
}

Object::Object(const OBJ_VERT* object_vertices, const unsigned int &vertices_size, const unsigned int* object_indices, const unsigned int & indices_size, const uint32_t &pipeline_index, const uint32_t &vs_ubuffer_size, Texture* _t)
{
	pMesh = new Mesh(object_vertices, vertices_size, object_indices, indices_size);
	pColorMap = _t;
	prv_Scale = 1.0f;

	prv_PipelineIndex = pipeline_index;
	prv_VSUniformBufferSize = vs_ubuffer_size;
}

Object::Object(
	const char *fmd_filename, const uint32_t &pipeline_index, const uint32_t &pipeline_mask, const uint32_t &vs_ubuffer_size, const uint32_t &gs_ubuffer_size,
	const char *color_map_filename, const char *normal_map_filename,
	const char *ambient_map_filename, const char *specular_map_filename
)
{
	//Basic Constructor
	pMesh = new Mesh(fmd_filename);
	prv_PipelineIndex = pipeline_index;
	prv_PipelineMask = pipeline_mask;
	prv_VSUniformBufferSize = vs_ubuffer_size;
	prv_GSUniformBufferSize = gs_ubuffer_size;

	//Object Properties for pipeline
	if (prv_PipelineMask & MASK_BIT_1)
		pColorMap = new Texture(color_map_filename);		else pColorMap = nullptr;
	if (prv_PipelineMask & MASK_BIT_2)
		pNormalMap = new Texture(normal_map_filename);		else pNormalMap = nullptr;
	if (prv_PipelineMask & MASK_BIT_3)
		pAmbientMap = new Texture(ambient_map_filename);	else pAmbientMap = nullptr;
	if (prv_PipelineMask & MASK_BIT_4)
		pSpecularMap = new Texture(specular_map_filename);	else pSpecularMap = nullptr;
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
void Object::init()
{
	//Basic Initialization
	prv_ModelMatrixPrevious = glm::mat4(1.0f);
	if (pMesh)			pMesh->init();
	if (pColorMap)		pColorMap->init();
	if (pNormalMap)		pNormalMap->init();
	if (pAmbientMap)	pAmbientMap->init();
	if (pSpecularMap)	pSpecularMap->init();
	
	//	prv_PipelineIndex = pipe_index;
	reset();
}

void Object::swap_color_format(const VkComponentSwizzle &r, const VkComponentSwizzle &g, const VkComponentSwizzle &b, const VkComponentSwizzle &a)
{
	pColorMap->swizzle(r, g, b, a);
}

void Object::clear()
{
	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();
	
	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		if (vs_uniform_buffer.empty())
			break;

		vkDestroyBuffer(myContext.device.logical, vs_uniform_buffer[i], nullptr);
		vkFreeMemory(myContext.device.logical, vs_uniform_memory[i], nullptr);
	}
	
	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		if (gs_uniform_buffer.empty())
			break;

		vkDestroyBuffer(myContext.device.logical, gs_uniform_buffer[i], nullptr);
		vkFreeMemory(myContext.device.logical, gs_uniform_memory[i], nullptr);
	}

	vs_uniform_buffer.clear();
	vs_uniform_memory.clear();
	gs_uniform_buffer.clear();
	gs_uniform_memory.clear();
}

void Object::reset()
{
	//Vertex Shader UBO [MVP at minimum]
	create_uniform_buffer(vs_uniform_buffer, vs_uniform_memory, prv_VSUniformBufferSize);

	if (prv_PipelineMask & PMASK_GEOSHADER)
		create_uniform_buffer(gs_uniform_buffer, gs_uniform_memory, prv_GSUniformBufferSize);

	create_descriptor_set2();
}

void Object::cleanup()
{
	if (pSpecularMap)
	{
		delete pSpecularMap;
		pSpecularMap = nullptr;
	}
	
	if (pAmbientMap)
	{
		delete pAmbientMap;
		pAmbientMap = nullptr;
	}

	if (pColorMap)
	{
		delete pColorMap;
		pColorMap = nullptr;
	}

	if (pNormalMap)
	{
		delete pNormalMap;
		pNormalMap = nullptr;
	}

	if (pMesh)
	{
		delete pMesh;
		pMesh = nullptr;
	}

	clear();
}

//  _____           _                   _
// |  __ \         (_)                 | |
// | |__) |  _ __   _  __   __   __ _  | |_    ___
// |  ___/  | '__| | | \ \ / /  / _` | | __|  / _ \
// | |      | |    | |  \ V /  | (_| | | |_  |  __/
// |_|      |_|    |_|   \_/    \__,_|  \__|  \___|
void Object::create_uniform_buffer(std::vector<VkBuffer> &uniform_buffer, std::vector<VkDeviceMemory> &uniform_memory, const uint32_t & uniform_size)
{
	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();

	uniform_buffer.resize(swapchain_size);
	uniform_memory.resize(swapchain_size);

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		vk_create_buffer(myContext.device.physical, myContext.device.logical, uniform_size, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniform_buffer[i], uniform_memory[i]);
	}
}

void Object::create_descriptor_set()
{
// 	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();
// 	std::vector<VkDescriptorSetLayout> descriptor_set_layout_vector(swapchain_size, myContext.pipelines.pipelines[prv_PipelineIndex].descriptor_set_layout);
// 
// 	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
// 	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
// 	descriptor_set_allocate_info.descriptorSetCount = swapchain_size;
// 	descriptor_set_allocate_info.descriptorPool = myContext.pipelines.pipelines[prv_PipelineIndex].descriptor_pool;
// 	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout_vector.data();
// 
// 	descriptor_set.resize(swapchain_size);
// 	vkAllocateDescriptorSets(myContext.device.logical, &descriptor_set_allocate_info, descriptor_set.data());
// 
// 	for (uint32_t i = 0; i < swapchain_size; ++i)
// 	{
// 		VkDescriptorBufferInfo descriptor_buffer_info = {};
// 		descriptor_buffer_info.buffer = vs_uniform_buffer[i];
// 		descriptor_buffer_info.offset = 0;
// 		descriptor_buffer_info.range = prv_VSUniformBufferSize;
// 
// 		VkDescriptorImageInfo descriptor_image_info = {};
// 		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
// 		descriptor_image_info.imageView = pColorMap->image_view;
// 		descriptor_image_info.sampler = pColorMap->sampler;
// 
// 		std::array<VkWriteDescriptorSet, 2> write_descriptor_set;
// 		write_descriptor_set[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
// 		write_descriptor_set[0].dstSet = descriptor_set[i];
// 		write_descriptor_set[0].dstBinding = 0;
// 		write_descriptor_set[0].dstArrayElement = 0;
// 		write_descriptor_set[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
// 		write_descriptor_set[0].descriptorCount = 1;
// 		write_descriptor_set[0].pBufferInfo = &descriptor_buffer_info;
// 		write_descriptor_set[0].pImageInfo = nullptr;
// 		write_descriptor_set[0].pTexelBufferView = nullptr;
// 		write_descriptor_set[0].pNext = nullptr;
// 
// 		write_descriptor_set[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
// 		write_descriptor_set[1].dstSet = descriptor_set[i];
// 		write_descriptor_set[1].dstBinding = 1;
// 		write_descriptor_set[1].dstArrayElement = 0;
// 		write_descriptor_set[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
// 		write_descriptor_set[1].descriptorCount = 1;
// 		write_descriptor_set[1].pBufferInfo = nullptr;
// 		write_descriptor_set[1].pImageInfo = &descriptor_image_info;
// 		write_descriptor_set[1].pTexelBufferView = nullptr;
// 		write_descriptor_set[1].pNext = nullptr;
// 
// 		vkUpdateDescriptorSets(myContext.device.logical, CAST(uint32_t, write_descriptor_set.size()), write_descriptor_set.data(), 0, nullptr);
// 	}

}

void Object::create_descriptor_set2()
{
	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();
	std::vector<VkDescriptorSetLayout> descriptor_set_layout_vector(swapchain_size, myContext.pipelines.pipelines[prv_PipelineIndex].descriptor_set_layout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate_info.descriptorSetCount = swapchain_size;
	descriptor_set_allocate_info.descriptorPool = myContext.pipelines.pipelines[prv_PipelineIndex].descriptor_pool;
	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout_vector.data();

	descriptor_set.resize(swapchain_size);
	vkAllocateDescriptorSets(myContext.device.logical, &descriptor_set_allocate_info, descriptor_set.data());

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		int binding_index = -1;


		std::vector<VkWriteDescriptorSet> write_descriptor_sets;

		//Note:
		//Try using pointers for the VkDescriptorBufferInfo and VkWriteDescriptorSet
		//More than likely i'll need to setup a struct and return those pointer values
		//Remember to delete them right after everything is set up

		VkDescriptorBufferInfo vs_descriptor_buffer_info = {};
		VkWriteDescriptorSet vs_set = {};
		VkDescriptorImageInfo cm_descriptor_image_info = {};
		VkWriteDescriptorSet cm_set = {};
		VkDescriptorImageInfo nm_descriptor_image_info = {};
		VkWriteDescriptorSet nm_set = {};
		VkDescriptorImageInfo am_descriptor_image_info = {};
		VkWriteDescriptorSet am_set = {};
		VkDescriptorImageInfo sm_descriptor_image_info = {};
		VkWriteDescriptorSet sm_set = {};
		VkDescriptorBufferInfo gs_descriptor_buffer_info = {};
		VkWriteDescriptorSet gs_set = {};


		//Get Total Bindings
		if (!vs_uniform_buffer.empty())
		{
			++binding_index;
			PushBackUniformBuffer(write_descriptor_sets, descriptor_set[i], vs_uniform_buffer[i], prv_VSUniformBufferSize, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, binding_index
				,vs_descriptor_buffer_info, vs_set);
		}
		if (pColorMap)
		{
			++binding_index;
			PushBackImageSampler(write_descriptor_sets, descriptor_set[i], pColorMap->image_view, pColorMap->sampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, binding_index,
				cm_descriptor_image_info, cm_set);
		}
		if (pNormalMap)
		{
			++binding_index;
			PushBackImageSampler(write_descriptor_sets, descriptor_set[i], pNormalMap->image_view, pNormalMap->sampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, binding_index,
				nm_descriptor_image_info, nm_set);
		}
		if (pAmbientMap)
		{
			++binding_index;
			PushBackImageSampler(write_descriptor_sets, descriptor_set[i], pAmbientMap->image_view, pAmbientMap->sampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, binding_index,
				am_descriptor_image_info, am_set);
		}
		if (pSpecularMap)
		{
			++binding_index;
			PushBackImageSampler(write_descriptor_sets, descriptor_set[i], pSpecularMap->image_view, pSpecularMap->sampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, binding_index,
				sm_descriptor_image_info, sm_set);
		}
		if (!gs_uniform_buffer.empty())
		{
			++binding_index;
			PushBackUniformBuffer(write_descriptor_sets, descriptor_set[i], gs_uniform_buffer[i], prv_GSUniformBufferSize, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, binding_index
			, gs_descriptor_buffer_info, gs_set);
		}

		vkUpdateDescriptorSets(myContext.device.logical, CAST(uint32_t, write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
	}

}

void Object::create_descriptor_set_nm()
{
	uint32_t swapchain_size = (uint32_t)myContext.swapchain.frame_buffers.size();
	std::vector<VkDescriptorSetLayout> descriptor_set_layout_vector(swapchain_size, myContext.pipelines.pipelines[prv_PipelineIndex].descriptor_set_layout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate_info.descriptorSetCount = swapchain_size;
	descriptor_set_allocate_info.descriptorPool = myContext.pipelines.pipelines[prv_PipelineIndex].descriptor_pool;
	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout_vector.data();

	descriptor_set.resize(swapchain_size);
	vkAllocateDescriptorSets(myContext.device.logical, &descriptor_set_allocate_info, descriptor_set.data());

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		VkDescriptorBufferInfo descriptor_buffer_info = {};
		descriptor_buffer_info.buffer = vs_uniform_buffer[i];
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = prv_VSUniformBufferSize;

		VkDescriptorImageInfo descriptor_image_info = {};
		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_image_info.imageView = pColorMap->image_view;
		descriptor_image_info.sampler = pColorMap->sampler;

		VkDescriptorImageInfo descriptor_normal_image_info = {};
		descriptor_normal_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_normal_image_info.imageView = pNormalMap->image_view;
		descriptor_normal_image_info.sampler = pNormalMap->sampler;

		std::array<VkWriteDescriptorSet, 3> write_descriptor_set;
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

		write_descriptor_set[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[2].dstSet = descriptor_set[i];
		write_descriptor_set[2].dstBinding = 2;
		write_descriptor_set[2].dstArrayElement = 0;
		write_descriptor_set[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptor_set[2].descriptorCount = 1;
		write_descriptor_set[2].pBufferInfo = nullptr;
		write_descriptor_set[2].pImageInfo = &descriptor_normal_image_info;
		write_descriptor_set[2].pTexelBufferView = nullptr;
		write_descriptor_set[2].pNext = nullptr;

		vkUpdateDescriptorSets(myContext.device.logical, CAST(uint32_t, write_descriptor_set.size()), write_descriptor_set.data(), 0, nullptr);
	}

}

void Object::update_uniform_buffer(Camera &camera)
{
	if (uniform_function)
		uniform_function(myContext, *this, camera);
}

void Object::PushBackImageSampler(std::vector<VkWriteDescriptorSet> &write_descriptor_sets, const VkDescriptorSet &descriptor_set, const VkImageView &image_view, const VkSampler &sampler, const VkDescriptorType &descriptor_type, const uint32_t &binding_index, VkDescriptorImageInfo &image_info, VkWriteDescriptorSet &set)
{
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.imageView = image_view;
	image_info.sampler = sampler;

	set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	set.dstSet = descriptor_set;
	set.dstBinding = binding_index;
	set.dstArrayElement = 0;
	set.descriptorType = descriptor_type;
	set.descriptorCount = 1;
	set.pBufferInfo = nullptr;
	set.pImageInfo = &image_info;
	set.pTexelBufferView = nullptr;
	set.pNext = nullptr;

	write_descriptor_sets.push_back(set);
}

void Object::PushBackUniformBuffer(std::vector<VkWriteDescriptorSet> &write_descriptor_sets, const VkDescriptorSet &descriptor_set, const VkBuffer &uniform_buffer, const uint32_t &uniform_structure_size, const VkDescriptorType &descriptor_type, const uint32_t &binding_index, VkDescriptorBufferInfo &buffer_info, VkWriteDescriptorSet &set)
{
	buffer_info.buffer = uniform_buffer;
	buffer_info.offset = 0;
	buffer_info.range = uniform_structure_size;

	set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	set.dstSet = descriptor_set;
	set.dstBinding = binding_index;
	set.dstArrayElement = 0;
	set.descriptorType = descriptor_type;
	set.descriptorCount = 1;
	set.pBufferInfo = &buffer_info;
	set.pImageInfo = nullptr;
	set.pTexelBufferView = nullptr;
	set.pNext = nullptr;

	write_descriptor_sets.push_back(set);
}
