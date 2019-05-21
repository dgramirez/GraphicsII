#include "VkObj_Uniform.h"

VkObj_Uniform::VkObj_Uniform() { }

void VkObj_Uniform::init(VkObj_DeviceProperties &device, VkObj_Swapchain &swapchain, VkCommandPool &command_pool)
{
	pDevice = &device;
	pSwapchain = &swapchain;
	pCommandPool = &command_pool;

	CreateDescriptorSetLayout();
	CreateUniformBuffer();
}

bool VkObj_Uniform::CreateDescriptorSetLayout()
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
	create_info.bindingCount = CAST(uint32_t, bindings.size());
	create_info.pBindings = bindings.data();

	CHECK_VKRESULT(a, vkCreateDescriptorSetLayout(pDevice->logical, &create_info, nullptr, &prv_DescriptorSetLayout), "Failed to create Descriptor Set Layout!");

	return true;
}

bool VkObj_Uniform::CreateUniformBuffer()
{
	VkDeviceSize buffer_size = sizeof(Mvp_object);

	buffer.resize(VkObj_Swapchain::swapchain_size);
	memory.resize(VkObj_Swapchain::swapchain_size);

	for (uint32_t i = 0; i < VkObj_Swapchain::swapchain_size; ++i)
	{
		vk_create_buffer(pDevice->physical, pDevice->logical, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			buffer[i], memory[i]);
	}

	return true;
}
