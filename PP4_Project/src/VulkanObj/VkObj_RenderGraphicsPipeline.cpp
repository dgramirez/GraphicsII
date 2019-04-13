#include "VkObj_RenderGraphicsPipeline.h"

bool vk_create_render_pass(const VkPhysicalDevice &physical_device, const VkDevice &device, const VkFormat &swapchain_format, const VkSampleCountFlagBits &msaa, VkRenderPass &render_pass)
{
	VkAttachmentDescription color_attachment_description = {};
	color_attachment_description.format = swapchain_format;
	color_attachment_description.samples = msaa;
	color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_attachment_reference = {};
	color_attachment_reference.attachment = 0;
	color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription color_attachment_resolve = {};
	color_attachment_resolve.format = swapchain_format;
	color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_resolve_reference = {};
	color_attachment_resolve_reference.attachment = 2;
	color_attachment_resolve_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_attachment_description = {};
	depth_attachment_description.format = vk_get_depth_format(physical_device);
	depth_attachment_description.samples = msaa;
	depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_reference = {};
	depth_attachment_reference.attachment = 1;
	depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_description = {};
	subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_attachment_reference;
	subpass_description.pDepthStencilAttachment = &depth_attachment_reference;
	subpass_description.pResolveAttachments = &color_attachment_resolve_reference;

	VkSubpassDependency subpass_dependency = {};
	subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass = 0;
	subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstAccessMask =
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { color_attachment_description, depth_attachment_description, color_attachment_resolve };
	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = (uint32_t)attachments.size();
	render_pass_create_info.pAttachments = attachments.data();
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pSubpasses = &subpass_description;
	render_pass_create_info.dependencyCount = 1;
	render_pass_create_info.pDependencies = &subpass_dependency;

	VkResult r = vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass);
	if (r) {
		LOG("Failed to create Render Pass! Error Code: " << r);
		return false;
	}

	return true;
}

bool vk_create_graphics_pipeline(const VkDevice &device, const VkExtent2D &swapchain_extent, const VkDescriptorSetLayout &descriptor_set_layout, const VkSampleCountFlagBits &msaa,
	const VkRenderPass &render_pass, VkPipelineLayout &graphics_pipeline_layout, VkPipeline &graphics_pipeline)
{
#pragma region Check and copy shader files

	std::vector<char> shader_vertex_file = vk_read_shader_file("src/shaders/vert.spv");
	std::vector<char> shader_fragment_file = vk_read_shader_file("src/shaders/frag.spv");

	if (shader_vertex_file.empty() || shader_fragment_file.empty())
	{
		LOG("Failed to Load Shaders.");
		return false;
	}

	VkShaderModule shader_vertex_module = vk_create_shader_module(device, shader_vertex_file);
	VkShaderModule shader_fragment_module = vk_create_shader_module(device, shader_fragment_file);

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
	input_vertex_info.vertexAttributeDescriptionCount = (uint32_t)vertex_attribute_description.size();
	input_vertex_info.pVertexAttributeDescriptions = vertex_attribute_description.data();

#pragma endregion

#pragma region Viewport and Scissor Setup and Create Info

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchain_extent.width;
	viewport.height = (float)swapchain_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = swapchain_extent;

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
	rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
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
	multisample_create_info.sampleShadingEnable = false;
	multisample_create_info.rasterizationSamples = msaa;
	multisample_create_info.minSampleShading = 1.0f;
	multisample_create_info.pSampleMask = nullptr;
	multisample_create_info.alphaToCoverageEnable = false;
	multisample_create_info.alphaToOneEnable = false;

#pragma endregion

#pragma region Depth and Stencil

	VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
	depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_create_info.depthTestEnable = true;
	depth_stencil_create_info.depthWriteEnable = true;
	depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_create_info.depthBoundsTestEnable = false;
	depth_stencil_create_info.minDepthBounds = 0.0f;
	depth_stencil_create_info.maxDepthBounds = 1.0f;
	depth_stencil_create_info.stencilTestEnable = false;

#pragma endregion (Note: Nothing Going on here. using nullptr. Used for reference)

#pragma region Color Blending

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
	color_blend_attachment_state.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment_state.blendEnable = VK_TRUE;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
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
	// 	dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	// 	dynamic_create_info.dynamicStateCount = 2;
	// 	dynamic_create_info.pDynamicStates = dynamicStates;

#pragma endregion (Note: Nothing going on here. using nullptr. Used for Reference)

#pragma region Pipeline Layout

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &graphics_pipeline_layout))
	{
		LOG("Failed to create Pipeline Layout!");
		return false;
	}

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

	pipeline_create_info.layout = graphics_pipeline_layout;
	pipeline_create_info.renderPass = render_pass;
	pipeline_create_info.subpass = 0;

	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &graphics_pipeline))
	{
		LOG("Pipeline Creation Has Failed!");
		return false;
	}

#pragma endregion

	//Destroy Module
	vkDestroyShaderModule(device, shader_vertex_module, nullptr);
	vkDestroyShaderModule(device, shader_fragment_module, nullptr);

	return true;
}

std::vector<char> vk_read_shader_file(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		LOG("FAILED TO GET FILENAME");
		return std::vector<char>();
	}

	unsigned int file_size = (unsigned int)file.tellg();
	std::vector<char> content(file_size);

	file.seekg(0);
	file.read(content.data(), file_size);

	file.close();

	return content;
}

VkShaderModule vk_create_shader_module(const VkDevice &device, const std::vector<char>& shader)
{
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = shader.size();
	create_info.pCode = (const uint32_t*)shader.data();

	VkShaderModule shader_module;
	VkResult r = vkCreateShaderModule(device, &create_info, nullptr, &shader_module);
	if (r) {
		LOG("Failed to create Shader Module! Error Code: " << r);
	}

	return shader_module;
}
