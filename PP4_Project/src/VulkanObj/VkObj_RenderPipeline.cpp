#include "VkObj_RenderPipeline.h"
#include "../Maths.h"

void VkObj_RenderPipeline::init(VkObj_DeviceProperties &device, VkObj_Swapchain &swapchain)
{
	pDevice = &device;
	pSwapchain = &swapchain;
}

void VkObj_RenderPipeline::create_pipeline(const char *vertex_shader, const char *fragment_shader, bool enable_depth, bool enable_culling)
{
	VkStruct_Pipeline new_pipeline;
	new_pipeline.vertex_shader_name = vertex_shader;
	new_pipeline.fragment_shader_name = fragment_shader;
	new_pipeline.depth_enabled = enable_depth;
	new_pipeline.culling_enabled = enable_culling;
	CreateDescriptorPool(new_pipeline.descriptor_pool);
	CreateDescriptorSetLayout(new_pipeline.descriptor_set_layout);
	CreateGraphicsPipeline(vertex_shader, fragment_shader, new_pipeline.descriptor_set_layout, new_pipeline.pipeline_layout, new_pipeline.pipeline, enable_depth, enable_culling);
	pipelines.push_back(new_pipeline);
}

void VkObj_RenderPipeline::shutdown()
{
	for (uint32_t i = 0; i < pipelines.size(); ++i)
	{
		vkDestroyDescriptorPool(pDevice->logical, pipelines[i].descriptor_pool, nullptr);
		vkDestroyDescriptorSetLayout(pDevice->logical, pipelines[i].descriptor_set_layout, nullptr);
		vkDestroyPipelineLayout(pDevice->logical, pipelines[i].pipeline_layout, nullptr);
		vkDestroyPipeline(pDevice->logical, pipelines[i].pipeline, nullptr);
	}
}

void VkObj_RenderPipeline::clean_pipeline()
{
	for (uint32_t i = 0; i < pipelines.size(); ++i)
	{
		vkDestroyPipelineLayout(pDevice->logical, pipelines[i].pipeline_layout, nullptr);
		vkDestroyPipeline(pDevice->logical, pipelines[i].pipeline, nullptr);
	}
}

void VkObj_RenderPipeline::reset_pipeline()
{
	for (uint32_t i = 0; i < pipelines.size(); ++i)
	{
		CreateDescriptorPool(pipelines[i].descriptor_pool);
		CreateDescriptorSetLayout(pipelines[i].descriptor_set_layout);
		CreateGraphicsPipeline(
			pipelines[i].vertex_shader_name, pipelines[i].fragment_shader_name, 
			pipelines[i].descriptor_set_layout, pipelines[i].pipeline_layout, pipelines[i].pipeline, 
			pipelines[i].depth_enabled, pipelines[i].culling_enabled
		);
	}

}

void VkObj_RenderPipeline::CreateDescriptorPool(VkDescriptorPool &descriptor_pool)
{
	std::array<VkDescriptorPoolSize, 2> descriptor_pool_size = {};
	descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_pool_size[0].descriptorCount = 0xFF;
	descriptor_pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_pool_size[1].descriptorCount = 0xFF;

	VkDescriptorPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = CAST(uint32_t, descriptor_pool_size.size());
	create_info.pPoolSizes = descriptor_pool_size.data();
	create_info.maxSets = 0xFF * 3;

	vkCreateDescriptorPool(pDevice->logical, &create_info, nullptr, &descriptor_pool);
}

void VkObj_RenderPipeline::CreateDescriptorSetLayout(VkDescriptorSetLayout &descriptor_set_layout)
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

	vkCreateDescriptorSetLayout(pDevice->logical, &create_info, nullptr, &descriptor_set_layout);
}

void VkObj_RenderPipeline::CreateGraphicsPipeline(const char *vertex_shader, const char *fragment_shader, VkDescriptorSetLayout &descriptor_set_layout, VkPipelineLayout &pipeline_layout, VkPipeline &pipeline, bool enable_depth, bool enable_culling)
{
#pragma region Check and copy shader files

	std::vector<char> shader_vertex_file = vk_read_shader_file(vertex_shader);
	std::vector<char> shader_fragment_file = vk_read_shader_file(fragment_shader);

	if (shader_vertex_file.empty() || shader_fragment_file.empty())
	{
#ifdef _DEBUG
		while(true)
			LOG("Failed to Load Shaders.");
#endif
	}

	VkShaderModule shader_vertex_module = vk_create_shader_module(pDevice->logical, shader_vertex_file);
	VkShaderModule shader_fragment_module = vk_create_shader_module(pDevice->logical, shader_fragment_file);

#pragma endregion In case I forget, glslangValidator.exe is NOT in the project. its with the actual SDK

#pragma region Shaders Create Info

	VkPipelineShaderStageCreateInfo vertex_shader_create_info = {};
	vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_shader_create_info.module = shader_vertex_module;
	vertex_shader_create_info.pName = "main";

	VkPipelineShaderStageCreateInfo fragment_shader_create_info = {};
	fragment_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_shader_create_info.module = shader_fragment_module;
	fragment_shader_create_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_create_info, fragment_shader_create_info };

#pragma endregion

#pragma region Assembly and Vertex Input

	VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
	assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assembly_create_info.primitiveRestartEnable = false;

	auto vertex_binding_description = Vertex::get_binding_description();
	auto vertex_attribute_description = Vertex::get_attribute_description();

	VkPipelineVertexInputStateCreateInfo input_vertex_info = {};
	input_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_vertex_info.vertexBindingDescriptionCount = 1;
	input_vertex_info.pVertexBindingDescriptions = &vertex_binding_description;
	input_vertex_info.vertexAttributeDescriptionCount = CAST(uint32_t, vertex_attribute_description.size());
	input_vertex_info.pVertexAttributeDescriptions = vertex_attribute_description.data();

#pragma endregion

#pragma region Viewport and Scissor Setup and Create Info

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)pSwapchain->extent2D.width;
	viewport.height = (float)pSwapchain->extent2D.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = pSwapchain->extent2D;

	VkPipelineViewportStateCreateInfo viewport_create_info = {};
	viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_create_info.viewportCount = 1;
	viewport_create_info.pViewports = &viewport;
	viewport_create_info.scissorCount = 1;
	viewport_create_info.pScissors = &scissor;

#pragma endregion

#pragma region Rasterizer

	VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
	rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_create_info.rasterizerDiscardEnable = false;
	rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_create_info.lineWidth = 1.0f;
	rasterization_create_info.cullMode = enable_culling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
	rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_create_info.depthClampEnable = false;
	rasterization_create_info.depthBiasEnable = false;
	rasterization_create_info.depthBiasClamp = 0.0f;
	rasterization_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_create_info.depthBiasSlopeFactor = 0.0f;

#pragma endregion

#pragma region Multisampling

	VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
	multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_create_info.sampleShadingEnable = VK_TRUE;
	multisample_create_info.rasterizationSamples = pDevice->msaa_support;
	multisample_create_info.minSampleShading = 1.0f;
	multisample_create_info.pSampleMask = nullptr;
	multisample_create_info.alphaToCoverageEnable = false;
	multisample_create_info.alphaToOneEnable = false;

#pragma endregion

#pragma region Depth and Stencil

	VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
	depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	if (enable_depth)
	{
		depth_stencil_create_info.depthTestEnable = true;
		depth_stencil_create_info.depthWriteEnable = true;
		depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil_create_info.depthBoundsTestEnable = false;
		depth_stencil_create_info.minDepthBounds = 0.0f;
		depth_stencil_create_info.maxDepthBounds = 1.0f;
		depth_stencil_create_info.stencilTestEnable = false;
	}

#pragma endregion 

#pragma region Color Blending

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
	color_blend_attachment_state.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment_state.blendEnable = VK_TRUE;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
	color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
	color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_create_info.logicOpEnable = VK_FALSE;
	color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
	color_blend_create_info.attachmentCount = 1;
	color_blend_create_info.pAttachments = &color_blend_attachment_state;
	color_blend_create_info.blendConstants[0] = 0.0f;
	color_blend_create_info.blendConstants[1] = 0.0f;
	color_blend_create_info.blendConstants[2] = 0.0f;
	color_blend_create_info.blendConstants[3] = 0.0f;

#pragma endregion

#pragma region Dynamic State

	// 	VkDynamicState dynamicStates[] = {
	// 		VK_DYNAMIC_STATE_VIEWPORT,
	// 		VK_DYNAMIC_STATE_LINE_WIDTH
	// 	};
	// 
	// 	VkPipelineDynamicStateCreateInfo dynamic_create_info;
	//	dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//	dynamic_create_info.dynamicStateCount = 2;
	//	dynamic_create_info.pDynamicStates = dynamicStates;

#pragma endregion (Note: Nothing going on here. using nullptr. Used for Reference)

#pragma region Pipeline Layout

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;

	vkCreatePipelineLayout(pDevice->logical, &pipeline_layout_create_info, nullptr, &pipeline_layout);

#pragma endregion

#pragma region Pipeline Creation

	VkGraphicsPipelineCreateInfo pipeline_create_info = {};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.stageCount = 2;
	pipeline_create_info.pStages = shader_stages;
	pipeline_create_info.pInputAssemblyState = &assembly_create_info;
	pipeline_create_info.pVertexInputState = &input_vertex_info;
	pipeline_create_info.pViewportState = &viewport_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_create_info;
	pipeline_create_info.pMultisampleState = &multisample_create_info;
	pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
	pipeline_create_info.pColorBlendState = &color_blend_create_info;
	pipeline_create_info.pDynamicState = nullptr;

	pipeline_create_info.layout = pipeline_layout;
	pipeline_create_info.renderPass = pSwapchain->render_pass;
	pipeline_create_info.subpass = 0;

	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex = -1;

	vkCreateGraphicsPipelines(pDevice->logical, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline);

#pragma endregion

	//Destroy Module
	vkDestroyShaderModule(pDevice->logical, shader_vertex_module, nullptr);
	vkDestroyShaderModule(pDevice->logical, shader_fragment_module, nullptr);
}
