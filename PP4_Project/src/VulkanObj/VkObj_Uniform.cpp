// #include "VkObj_Uniform.h"
// 
// VkObj_Uniform::VkObj_Uniform() { }
// 
// void VkObj_Uniform::init(VkObj_DeviceProperties &device, VkObj_Swapchain &swapchain, VkCommandPool &command_pool)
// {
// 	pDevice = &device;
// 	pSwapchain = &swapchain;
// 	pCommandPool = &command_pool;
// 
// 	CreateDescriptorSetLayout();
// }
// 
// bool VkObj_Uniform::CreateDescriptorSetLayout()
// {
// 	VkDescriptorSetLayoutBinding mvp_layout_binding = {};
// 	mvp_layout_binding.binding = 0;
// 	mvp_layout_binding.descriptorCount = 1;
// 	mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
// 	mvp_layout_binding.pImmutableSamplers = nullptr;
// 	mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
// 
// 	VkDescriptorSetLayoutBinding sampler_layout_binding = {};
// 	sampler_layout_binding.binding = 1;
// 	sampler_layout_binding.descriptorCount = 1;
// 	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
// 	sampler_layout_binding.pImmutableSamplers = nullptr;
// 	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
// 
// 	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { mvp_layout_binding, sampler_layout_binding };
// 	VkDescriptorSetLayoutCreateInfo create_info = {};
// 	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
// 	create_info.bindingCount = CAST(uint32_t, bindings.size());
// 	create_info.pBindings = bindings.data();
// 
// 	CHECK_VKRESULT(a, vkCreateDescriptorSetLayout(pDevice->logical, &create_info, nullptr, &prv_DescriptorSetLayout), "Failed to create Descriptor Set Layout!");
// 
// 	return true;
// }
// 
// bool VkObj_Uniform::CreateUniformBuffer(std::vector<VkBuffer> &buffer, std::vector<VkDeviceMemory> &memory)
// {
// 
// }
