#include "pch.h"
#include "VkObj_RenderPipeline.h"
#include "../Maths.h"

void VkObj_RenderPipeline::init(VkObj_DeviceProperties &device, VkObj_Swapchain &swapchain)
{
	pDevice = &device;
	pSwapchain = &swapchain;
}

// void VkObj_RenderPipeline::create_pipeline(const char *vertex_shader, const char *fragment_shader, bool enable_depth, VkCompareOp depth_op, bool enable_culling)
// {
// 	VkStruct_Pipeline new_pipeline;
// 	new_pipeline.vertex_shader_name = vertex_shader;
// 	new_pipeline.fragment_shader_name = fragment_shader;
// 	new_pipeline.depth_enabled = enable_depth;
// 	new_pipeline.culling_enabled = enable_culling;
// 	new_pipeline.depth_op = depth_op;
// //	CreateDescriptorPool(new_pipeline.descriptor_pool);
// //	CreateDescriptorSetLayout(new_pipeline.descriptor_set_layout);
// 	CreateGraphicsPipeline(vertex_shader, fragment_shader, new_pipeline.descriptor_set_layout, new_pipeline.pipeline_layout, new_pipeline.pipeline, enable_depth, depth_op, enable_culling);
// 	pipelines.push_back(new_pipeline);
// }

void VkObj_RenderPipeline::create_pipeline(const uint32_t &pipeline_mask, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader)
{
	VkStruct_Pipeline new_pipeline;
	new_pipeline.vertex_shader_name = vertex_shader;
	new_pipeline.fragment_shader_name = fragment_shader;
	new_pipeline.geometry_shader_name = geometry_shader;
	new_pipeline.pipeline_mask = pipeline_mask;

	CreateDescriptorPool(pipeline_mask, new_pipeline.descriptor_pool);
	CreateDescriptorSetLayout(pipeline_mask, new_pipeline.descriptor_set_layout);
	CreateGraphicsPipeline(pipeline_mask, vertex_shader, fragment_shader, geometry_shader, new_pipeline.descriptor_set_layout, new_pipeline.pipeline_layout, new_pipeline.pipeline);
	pipelines.push_back(new_pipeline);
}

// void VkObj_RenderPipeline::create_pipeline_normalmaps(const char *vertex_shader, const char *fragment_shader, bool enable_depth /*= true*/, VkCompareOp depth_op /*= VK_COMPARE_OP_LESS*/, bool enable_culling /*= true*/)
// {
// 	VkStruct_Pipeline new_pipeline;
// 	new_pipeline.vertex_shader_name = vertex_shader;
// 	new_pipeline.fragment_shader_name = fragment_shader;
// 	new_pipeline.depth_enabled = enable_depth;
// 	new_pipeline.culling_enabled = enable_culling;
// 	new_pipeline.depth_op = depth_op;
// 	CreateDescriptorPool_NM(new_pipeline.descriptor_pool);
// 	CreateDescriptorSetLayout_NM(new_pipeline.descriptor_set_layout);
// 	CreateGraphicsPipeline(vertex_shader, fragment_shader, new_pipeline.descriptor_set_layout, new_pipeline.pipeline_layout, new_pipeline.pipeline, enable_depth, depth_op, enable_culling);
// 	pipelines.push_back(new_pipeline);
// }

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
		CreateDescriptorPool(pipelines[i].pipeline_mask, pipelines[i].descriptor_pool);
		CreateDescriptorSetLayout(pipelines[i].pipeline_mask, pipelines[i].descriptor_set_layout);
		CreateGraphicsPipeline(pipelines[i].pipeline_mask, pipelines[i].vertex_shader_name, pipelines[i].fragment_shader_name, pipelines[i].geometry_shader_name,
			pipelines[i].descriptor_set_layout, pipelines[i].pipeline_layout, pipelines[i].pipeline);
	}

}

void VkObj_RenderPipeline::CreateDescriptorPool(const uint32_t &pipeline_mask, VkDescriptorPool &descriptor_pool)
{
	const uint32_t SIZE_OF_POOL = 0xFF;

	std::vector<VkDescriptorPoolSize> descriptor_pool_size = {};

	VkDescriptorPoolSize geometry_ubo;
	if (pipeline_mask & PMASK_GEOSHADER)
	{
		geometry_ubo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		geometry_ubo.descriptorCount = SIZE_OF_POOL;
		descriptor_pool_size.push_back(geometry_ubo);
	}

	VkDescriptorPoolSize vertex_ubo = {};
	vertex_ubo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vertex_ubo.descriptorCount = SIZE_OF_POOL;
	descriptor_pool_size.push_back(vertex_ubo);

	VkDescriptorPoolSize color_map_sampler = {};
	if (pipeline_mask & PMASK_COLORMAP)
	{
		color_map_sampler.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		color_map_sampler.descriptorCount = SIZE_OF_POOL;
		descriptor_pool_size.push_back(color_map_sampler);
	}

	VkDescriptorPoolSize normal_map_sampler = {};
	if (pipeline_mask & PMASK_NORMALMAP)
	{
		normal_map_sampler.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normal_map_sampler.descriptorCount = SIZE_OF_POOL;
		descriptor_pool_size.push_back(normal_map_sampler);
	}

	VkDescriptorPoolSize ambient_map_sampler = {};
	if (pipeline_mask & PMASK_AMBIENTMAP)
	{
		ambient_map_sampler.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		ambient_map_sampler.descriptorCount = SIZE_OF_POOL;
		descriptor_pool_size.push_back(ambient_map_sampler);
	}

	VkDescriptorPoolSize specular_map_sampler = {};
	if (pipeline_mask & PMASK_SPECMAP)
	{
		specular_map_sampler.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		specular_map_sampler.descriptorCount = SIZE_OF_POOL;
		descriptor_pool_size.push_back(specular_map_sampler);
	}

	const auto SIZE_OF_SETS = SIZE_OF_POOL * descriptor_pool_size.size();
	VkDescriptorPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = CAST(uint32_t, descriptor_pool_size.size());
	create_info.pPoolSizes = descriptor_pool_size.data();
	create_info.maxSets = CAST(uint32_t,SIZE_OF_SETS);

	vkCreateDescriptorPool(pDevice->logical, &create_info, nullptr, &descriptor_pool);

// 	std::array<VkDescriptorPoolSize, 2> descriptor_pool_size = {};
// 	descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
// 	descriptor_pool_size[0].descriptorCount = 0xFF;
// 	descriptor_pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
// 	descriptor_pool_size[1].descriptorCount = 0xFF;
// 
// 	VkDescriptorPoolCreateInfo create_info = {};
// 	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
// 	create_info.poolSizeCount = CAST(uint32_t, descriptor_pool_size.size());
// 	create_info.pPoolSizes = descriptor_pool_size.data();
// 	create_info.maxSets = 0xFF * 3;
// 
// 	vkCreateDescriptorPool(pDevice->logical, &create_info, nullptr, &descriptor_pool);
}

void VkObj_RenderPipeline::CreateDescriptorSetLayout(const uint32_t &pipeline_mask, VkDescriptorSetLayout &descriptor_set_layout)
{
	uint32_t binding = 0;
	std::vector< VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayoutBinding gs_ubo_layout_binding = {};
	if (pipeline_mask & PMASK_GEOSHADER)
	{
		gs_ubo_layout_binding.binding = binding++;
		gs_ubo_layout_binding.descriptorCount = 1;
		gs_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		gs_ubo_layout_binding.pImmutableSamplers = nullptr;
		gs_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		bindings.push_back(gs_ubo_layout_binding);
	}

	VkDescriptorSetLayoutBinding vs_ubo_layout_binding = {};
	vs_ubo_layout_binding.binding = binding++;
	vs_ubo_layout_binding.descriptorCount = 1;
	vs_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vs_ubo_layout_binding.pImmutableSamplers = nullptr;
	vs_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings.push_back(vs_ubo_layout_binding);

	
	VkDescriptorSetLayoutBinding colormap_layout = {};
	if (pipeline_mask & PMASK_COLORMAP)
	{
		colormap_layout.binding = binding++;
		colormap_layout.descriptorCount = 1;
		colormap_layout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		colormap_layout.pImmutableSamplers = nullptr;
		colormap_layout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindings.push_back(colormap_layout);
	}

	VkDescriptorSetLayoutBinding normalmap_layout = {};
	if (pipeline_mask & PMASK_NORMALMAP)
	{
		normalmap_layout.binding = binding++;
		normalmap_layout.descriptorCount = 1;
		normalmap_layout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalmap_layout.pImmutableSamplers = nullptr;
		normalmap_layout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindings.push_back(normalmap_layout);
	}

	VkDescriptorSetLayoutBinding ambientmap_layout = {};
	if (pipeline_mask & PMASK_AMBIENTMAP)
	{
		ambientmap_layout.binding = binding++;
		ambientmap_layout.descriptorCount = 1;
		ambientmap_layout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		ambientmap_layout.pImmutableSamplers = nullptr;
		ambientmap_layout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindings.push_back(ambientmap_layout);
	}

	VkDescriptorSetLayoutBinding specmap_layout = {};
	if (pipeline_mask & PMASK_SPECMAP)
	{
		specmap_layout.binding = binding++;
		specmap_layout.descriptorCount = 1;
		specmap_layout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		specmap_layout.pImmutableSamplers = nullptr;
		specmap_layout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindings.push_back(specmap_layout);
	}

	VkDescriptorSetLayoutCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	create_info.bindingCount = CAST(uint32_t, bindings.size());
	create_info.pBindings = bindings.data();

	vkCreateDescriptorSetLayout(pDevice->logical, &create_info, nullptr, &descriptor_set_layout);
}

// void VkObj_RenderPipeline::CreateGraphicsPipeline(const char *vertex_shader, const char *fragment_shader, VkDescriptorSetLayout &descriptor_set_layout, VkPipelineLayout &pipeline_layout, VkPipeline &pipeline, bool enable_depth, VkCompareOp depth_op, bool enable_culling)
// {
// #pragma region Check and copy shader files
// 
// 	std::vector<char> shader_vertex_file = vk_read_shader_file(vertex_shader);
// 	std::vector<char> shader_fragment_file = vk_read_shader_file(fragment_shader);
// 
// 	if (shader_vertex_file.empty() || shader_fragment_file.empty())
// 	{
// #ifdef _DEBUG
// 		while(true)
// 			LOG("Failed to Load Shaders.");
// #endif
// 	}
// 
// 	VkShaderModule shader_vertex_module = vk_create_shader_module(pDevice->logical, shader_vertex_file);
// 	VkShaderModule shader_fragment_module = vk_create_shader_module(pDevice->logical, shader_fragment_file);
// 
// #pragma endregion In case I forget, glslangValidator.exe is NOT in the project. its with the actual SDK
// 
// #pragma region Shaders Create Info
// 
// 	VkPipelineShaderStageCreateInfo vertex_shader_create_info = {};
// 	vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 	vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
// 	vertex_shader_create_info.module = shader_vertex_module;
// 	vertex_shader_create_info.pName = "main";
// 
// 	VkPipelineShaderStageCreateInfo fragment_shader_create_info = {};
// 	fragment_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 	fragment_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
// 	fragment_shader_create_info.module = shader_fragment_module;
// 	fragment_shader_create_info.pName = "main";
// 
// 	VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_create_info, fragment_shader_create_info };
// 
// #pragma endregion
// 
// #pragma region Assembly and Vertex Input
// 
// 	VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
// 	assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
// 	assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
// 	assembly_create_info.primitiveRestartEnable = false;
// 
// 	auto vertex_binding_description = Vertex::get_binding_description();
// 	auto vertex_attribute_description = Vertex::get_attribute_description();
// 
// 	VkPipelineVertexInputStateCreateInfo input_vertex_info = {};
// 	input_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
// 	input_vertex_info.vertexBindingDescriptionCount = 1;
// 	input_vertex_info.pVertexBindingDescriptions = &vertex_binding_description;
// 	input_vertex_info.vertexAttributeDescriptionCount = CAST(uint32_t, vertex_attribute_description.size());
// 	input_vertex_info.pVertexAttributeDescriptions = vertex_attribute_description.data();
// 
// #pragma endregion
// 
// #pragma region Viewport and Scissor Setup and Create Info
// 
// 	VkViewport viewport = {};
// 	viewport.x = 0.0f;
// 	viewport.y = 0.0f;
// 	viewport.width = (float)pSwapchain->extent2D.width;
// 	viewport.height = (float)pSwapchain->extent2D.height;
// 	viewport.minDepth = 0.0f;
// 	viewport.maxDepth = 1.0f;
// 
// 	VkRect2D scissor = {};
// 	scissor.offset = { 0,0 };
// 	scissor.extent = pSwapchain->extent2D;
// 
// 	VkPipelineViewportStateCreateInfo viewport_create_info = {};
// 	viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
// 	viewport_create_info.viewportCount = 1;
// 	viewport_create_info.pViewports = &viewport;
// 	viewport_create_info.scissorCount = 1;
// 	viewport_create_info.pScissors = &scissor;
// 
// #pragma endregion
// 
// #pragma region Rasterizer
// 
// 	VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
// 	rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
// 	rasterization_create_info.rasterizerDiscardEnable = false;
// 	rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
// 	rasterization_create_info.lineWidth = 1.0f;
// 	rasterization_create_info.cullMode = enable_culling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
// 	rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
// 	rasterization_create_info.depthClampEnable = false;
// 	rasterization_create_info.depthBiasEnable = false;
// 	rasterization_create_info.depthBiasClamp = 0.0f;
// 	rasterization_create_info.depthBiasConstantFactor = 0.0f;
// 	rasterization_create_info.depthBiasSlopeFactor = 0.0f;
// 
// #pragma endregion
// 
// #pragma region Multisampling
// 
// 	VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
// 	multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
// 	multisample_create_info.sampleShadingEnable = VK_TRUE;
// 	multisample_create_info.rasterizationSamples = pDevice->msaa_support;
// 	multisample_create_info.minSampleShading = 1.0f;
// 	multisample_create_info.pSampleMask = nullptr;
// 	multisample_create_info.alphaToCoverageEnable = false;
// 	multisample_create_info.alphaToOneEnable = false;
// 
// #pragma endregion
// 
// #pragma region Depth and Stencil
// 
// 	VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
// 	depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
// 	if (enable_depth)
// 	{
// 		depth_stencil_create_info.depthTestEnable = true;
// 		depth_stencil_create_info.depthWriteEnable = true;
// 		depth_stencil_create_info.depthCompareOp = depth_op;
// 		depth_stencil_create_info.depthBoundsTestEnable = false;
// 		depth_stencil_create_info.minDepthBounds = 0.0f;
// 		depth_stencil_create_info.maxDepthBounds = 1.0f;
// 		depth_stencil_create_info.stencilTestEnable = false;
// 	}
// 
// #pragma endregion 
// 
// #pragma region Color Blending
// 
// 	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
// 	color_blend_attachment_state.colorWriteMask =
// 		VK_COLOR_COMPONENT_R_BIT |
// 		VK_COLOR_COMPONENT_G_BIT |
// 		VK_COLOR_COMPONENT_B_BIT |
// 		VK_COLOR_COMPONENT_A_BIT;
// 	color_blend_attachment_state.blendEnable = VK_TRUE;
// 	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
// 	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
// 	color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
// 	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
// 	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
// 	color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
// 
// 	VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
// 	color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
// 	color_blend_create_info.logicOpEnable = VK_FALSE;
// 	color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
// 	color_blend_create_info.attachmentCount = 1;
// 	color_blend_create_info.pAttachments = &color_blend_attachment_state;
// 	color_blend_create_info.blendConstants[0] = 0.0f;
// 	color_blend_create_info.blendConstants[1] = 0.0f;
// 	color_blend_create_info.blendConstants[2] = 0.0f;
// 	color_blend_create_info.blendConstants[3] = 0.0f;
// 
// #pragma endregion
// 
// #pragma region Dynamic State
// 
// 	// 	VkDynamicState dynamicStates[] = {
// 	// 		VK_DYNAMIC_STATE_VIEWPORT,
// 	// 		VK_DYNAMIC_STATE_LINE_WIDTH
// 	// 	};
// 	// 
// 	// 	VkPipelineDynamicStateCreateInfo dynamic_create_info;
// 	//	dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
// 	//	dynamic_create_info.dynamicStateCount = 2;
// 	//	dynamic_create_info.pDynamicStates = dynamicStates;
// 
// #pragma endregion (Note: Nothing going on here. using nullptr. Used for Reference)
// 
// #pragma region Pipeline Layout
// 
// 	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
// 	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
// 	pipeline_layout_create_info.setLayoutCount = 1;
// 	pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout;
// 	pipeline_layout_create_info.pushConstantRangeCount = 0;
// 	pipeline_layout_create_info.pPushConstantRanges = nullptr;
// 
// 	vkCreatePipelineLayout(pDevice->logical, &pipeline_layout_create_info, nullptr, &pipeline_layout);
// 
// #pragma endregion
// 
// #pragma region Pipeline Creation
// 
// 	VkGraphicsPipelineCreateInfo pipeline_create_info = {};
// 	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
// 	pipeline_create_info.stageCount = 2;
// 	pipeline_create_info.pStages = shader_stages;
// 	pipeline_create_info.pInputAssemblyState = &assembly_create_info;
// 	pipeline_create_info.pVertexInputState = &input_vertex_info;
// 	pipeline_create_info.pViewportState = &viewport_create_info;
// 	pipeline_create_info.pRasterizationState = &rasterization_create_info;
// 	pipeline_create_info.pMultisampleState = &multisample_create_info;
// 	pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
// 	pipeline_create_info.pColorBlendState = &color_blend_create_info;
// 	pipeline_create_info.pDynamicState = nullptr;
// 
// 	pipeline_create_info.layout = pipeline_layout;
// 	pipeline_create_info.renderPass = pSwapchain->render_pass;
// 	pipeline_create_info.subpass = 0;
// 
// 	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
// 	pipeline_create_info.basePipelineIndex = -1;
// 
// 	vkCreateGraphicsPipelines(pDevice->logical, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline);
// 
// #pragma endregion
// 
// 	//Destroy Module
// 	vkDestroyShaderModule(pDevice->logical, shader_vertex_module, nullptr);
// 	vkDestroyShaderModule(pDevice->logical, shader_fragment_module, nullptr);
// }

void VkObj_RenderPipeline::CreateGraphicsPipeline(const uint32_t & pipeline_mask, const char *vertex_shader, const char *fragment_shader, const char *geometry_shader, VkDescriptorSetLayout &descriptor_set_layout, VkPipelineLayout &pipeline_layout, VkPipeline &pipeline)
{
#pragma region Shader information
	
	//Stage Setup
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

	//Geometry Shader
	std::vector<char> shader_geometry_file;
	VkShaderModule shader_geometry_module = nullptr;
	if (pipeline_mask & PMASK_GEOSHADER)
	{
		shader_geometry_file = vk_read_shader_file(geometry_shader);
		shader_geometry_module = vk_create_shader_module(pDevice->logical, shader_geometry_file);

		VkPipelineShaderStageCreateInfo geometry_shader_create_info = {};
		geometry_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		geometry_shader_create_info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		geometry_shader_create_info.module = shader_geometry_module;
		geometry_shader_create_info.pName = "main";

		shader_stages.push_back(geometry_shader_create_info);
	}

	//Vertex Shader
	std::vector<char> shader_vertex_file = vk_read_shader_file(vertex_shader);
	VkShaderModule shader_vertex_module = vk_create_shader_module(pDevice->logical, shader_vertex_file);

	VkPipelineShaderStageCreateInfo vertex_shader_create_info = {};
	vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_shader_create_info.module = shader_vertex_module;
	vertex_shader_create_info.pName = "main";

	shader_stages.push_back(vertex_shader_create_info);

	//Fragment Shader
	std::vector<char> shader_fragment_file = vk_read_shader_file(fragment_shader);
	VkShaderModule shader_fragment_module = vk_create_shader_module(pDevice->logical, shader_fragment_file);

	VkPipelineShaderStageCreateInfo fragment_shader_create_info = {};
	fragment_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_shader_create_info.module = shader_fragment_module;
	fragment_shader_create_info.pName = "main";

	shader_stages.push_back(fragment_shader_create_info);
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
	rasterization_create_info.polygonMode = (pipeline_mask & PMASK_WIREFRAME) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	rasterization_create_info.lineWidth = 1.0f;
	rasterization_create_info.cullMode = (pipeline_mask & PMASK_NOCULLING) ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
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
	if ( !(pipeline_mask & PMASK_NODEPTH) )
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
	pipeline_create_info.stageCount = CAST(uint32_t,shader_stages.size());
	pipeline_create_info.pStages = shader_stages.data();
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
	if (shader_vertex_module)	vkDestroyShaderModule(pDevice->logical, shader_vertex_module, nullptr);
	if (shader_fragment_module) vkDestroyShaderModule(pDevice->logical, shader_fragment_module, nullptr);
	if (shader_geometry_module) vkDestroyShaderModule(pDevice->logical, shader_geometry_module, nullptr);
}

void VkObj_RenderPipeline::CreateDescriptorPool_NM(VkDescriptorPool &descriptor_pool)
{
	std::array<VkDescriptorPoolSize, 3> descriptor_pool_size = {};
	descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_pool_size[0].descriptorCount = 0xFF;
	descriptor_pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_pool_size[1].descriptorCount = 0xFF;
	descriptor_pool_size[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_pool_size[2].descriptorCount = 0xFF;

	VkDescriptorPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = CAST(uint32_t, descriptor_pool_size.size());
	create_info.pPoolSizes = descriptor_pool_size.data();
	create_info.maxSets = 0xFF * 3;

	vkCreateDescriptorPool(pDevice->logical, &create_info, nullptr, &descriptor_pool);
}

void VkObj_RenderPipeline::CreateDescriptorSetLayout_NM(VkDescriptorSetLayout &descriptor_set_layout)
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

	VkDescriptorSetLayoutBinding normal_sampler_layout_binding = {};
	normal_sampler_layout_binding.binding = 2;
	normal_sampler_layout_binding.descriptorCount = 1;
	normal_sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normal_sampler_layout_binding.pImmutableSamplers = nullptr;
	normal_sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 3> bindings = { mvp_layout_binding, sampler_layout_binding, normal_sampler_layout_binding };
	VkDescriptorSetLayoutCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	create_info.bindingCount = CAST(uint32_t, bindings.size());
	create_info.pBindings = bindings.data();

	vkCreateDescriptorSetLayout(pDevice->logical, &create_info, nullptr, &descriptor_set_layout);
}
