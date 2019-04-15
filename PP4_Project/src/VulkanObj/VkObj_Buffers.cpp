#include "VkObj_Buffers.h"

bool vk_create_vertex_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device,  const VkCommandPool &command_pool, const VkQueue &graphics_queue,
	std::vector<Object3D> &object_list, VkBuffer &vertex_buffer, VkDeviceMemory &vertex_buffer_memory)
{
	VkDeviceSize buffer_size = sizeof(object_list[0].vertices[0]) * object_list[0].vertices.size();
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	vk_create_buffer(physical_device, device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer, staging_buffer_memory);

	void* data;
	vkMapMemory(device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, object_list[0].vertices.data(), (uint32_t)buffer_size);
	vkUnmapMemory(device, staging_buffer_memory);

	vk_create_buffer(physical_device, device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertex_buffer, vertex_buffer_memory);

	vk_copy_buffer(device, command_pool, graphics_queue, staging_buffer, vertex_buffer, buffer_size);

	vkDestroyBuffer(device, staging_buffer, nullptr);
	vkFreeMemory(device, staging_buffer_memory, nullptr);

	return true;
}

bool vk_create_index_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkCommandPool &command_pool, const VkQueue &graphics_queue,
	std::vector<Object3D> &object_list, VkBuffer &index_buffer, VkDeviceMemory &index_buffer_memory)
{
	VkDeviceSize buffer_size = sizeof(object_list[0].indices[0]) * object_list[0].indices.size();
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	vk_create_buffer(physical_device, device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer, staging_buffer_memory);

	void* data;
	vkMapMemory(device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, object_list[0].indices.data(), (uint32_t)buffer_size);
	vkUnmapMemory(device, staging_buffer_memory);

	vk_create_buffer(physical_device, device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		index_buffer, index_buffer_memory);

	vk_copy_buffer(device, command_pool, graphics_queue, staging_buffer, index_buffer, buffer_size);

	vkDestroyBuffer(device, staging_buffer, nullptr);
	vkFreeMemory(device, staging_buffer_memory, nullptr);

	return true;
}

bool vk_create_uniform_buffer(const VkPhysicalDevice &physical_device, const VkDevice &device, const std::vector<VkImage> &swapchain_images, 
	std::vector<VkBuffer> &uniform_buffers, std::vector<VkDeviceMemory> &uniform_buffers_memory)
{
	VkDeviceSize buffer_size = sizeof(Mvp_object);

	uniform_buffers.resize(swapchain_images.size());
	uniform_buffers_memory.resize(swapchain_images.size());

	for (uint32_t i = 0; i < swapchain_images.size(); ++i)
	{
		vk_create_buffer(physical_device, device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniform_buffers[i], uniform_buffers_memory[i]);
	}

	return true;
}

bool vk_update_uniform_buffer(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, std::vector<VkDeviceMemory> &uniform_buffer_memory)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	Mvp_object mvp;

#if USE_PYRAMID
	mvp.model = glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
#else
	mvp.model = glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
#endif
	mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 10.0f);

	mvp.projection[1][1] = -mvp.projection[1][1];

	void* data;
	vkMapMemory(device, uniform_buffer_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_buffer_memory[current_image]);

	return true;
}

bool vk_create_descriptor_set_layout(const VkDevice &device, VkDescriptorSetLayout &descriptor_set_layout)
{
	VkDescriptorSetLayoutBinding mvp_layout_binding = {};
	mvp_layout_binding.binding = 0;
	mvp_layout_binding.descriptorCount = 1;
	mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvp_layout_binding.pImmutableSamplers = nullptr;
	mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding sampler_layout_binding = {};
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = 1;
	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.pImmutableSamplers = nullptr;
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { mvp_layout_binding, sampler_layout_binding };
	VkDescriptorSetLayoutCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	create_info.bindingCount = (uint32_t)bindings.size();
	create_info.pBindings = bindings.data();

	CHECK_VKRESULT(a, vkCreateDescriptorSetLayout(device, &create_info, nullptr, &descriptor_set_layout), "Failed to create Descriptor Set Layout!");

	return true;
}

bool vk_create_descriptor_pool(const VkDevice &device, const std::vector<VkImage> &swapchain_images, VkDescriptorPool &descriptor_pool)
{
	std::array<VkDescriptorPoolSize, 2> descriptor_pool_size = {};
	descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_pool_size[0].descriptorCount = (uint32_t)swapchain_images.size();
	descriptor_pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_pool_size[1].descriptorCount = (uint32_t)swapchain_images.size();

	VkDescriptorPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = (uint32_t)descriptor_pool_size.size();
	create_info.pPoolSizes = descriptor_pool_size.data();
	create_info.maxSets = (uint32_t)swapchain_images.size();

	CHECK_VKRESULT(r, vkCreateDescriptorPool(device, &create_info, nullptr, &descriptor_pool), "Failed to create a Desriptor Pool! Error Code: ");

	return true;
}

bool vk_create_descriptor_sets(const VkDevice &device, const std::vector<VkImage> &swapchain_images, VkDescriptorPool &descriptor_pool,
	const std::vector<VkBuffer> &uniform_buffers, const VkImageView &texture_image_view, const VkSampler &sampler,
	const VkDescriptorSetLayout &descriptor_set_layout, std::vector<VkDescriptorSet> &descriptor_sets)
{
	std::vector<VkDescriptorSetLayout> descriptor_set_layout_vector(swapchain_images.size(), descriptor_set_layout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate_info.descriptorSetCount = (uint32_t)swapchain_images.size();
	descriptor_set_allocate_info.descriptorPool = descriptor_pool;
	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout_vector.data();

	descriptor_sets.resize(swapchain_images.size());
	CHECK_VKRESULT(r, vkAllocateDescriptorSets(device, &descriptor_set_allocate_info, descriptor_sets.data()), "Failed to Allocate Descriptor Set!");

	for (uint32_t i = 0; i < swapchain_images.size(); ++i)
	{
		VkDescriptorBufferInfo descriptor_buffer_info = {};
		descriptor_buffer_info.buffer = uniform_buffers[i];
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = sizeof(Mvp_object);

		VkDescriptorImageInfo descriptor_image_info = {};
		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_image_info.imageView = texture_image_view;
		descriptor_image_info.sampler = sampler;


		std::array<VkWriteDescriptorSet, 2> write_descriptor_set = {};
		write_descriptor_set[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[0].dstSet = descriptor_sets[i];
		write_descriptor_set[0].dstBinding = 0;
		write_descriptor_set[0].dstArrayElement = 0;
		write_descriptor_set[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor_set[0].descriptorCount = 1;
		write_descriptor_set[0].pBufferInfo = &descriptor_buffer_info;
		write_descriptor_set[0].pImageInfo = nullptr;
		write_descriptor_set[0].pTexelBufferView = nullptr;

		write_descriptor_set[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[1].dstSet = descriptor_sets[i];
		write_descriptor_set[1].dstBinding = 1;
		write_descriptor_set[1].dstArrayElement = 0;
		write_descriptor_set[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptor_set[1].descriptorCount = 1;
		write_descriptor_set[1].pBufferInfo = nullptr;
		write_descriptor_set[1].pImageInfo = &descriptor_image_info;
		write_descriptor_set[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device, (uint32_t)write_descriptor_set.size(), write_descriptor_set.data(), 0, nullptr);
	}

	return true;
}
