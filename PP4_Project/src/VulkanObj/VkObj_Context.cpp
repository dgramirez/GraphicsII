#include "VkObj_Context.h"
#include "../Object.h"

std::vector<char> vk_read_shader_file(const std::string& filename);
VkShaderModule vk_create_shader_module(const VkDevice &device, const std::vector<char>& shader);

VkObj_Context::VkObj_Context()
{

}

bool VkObj_Context::CreateQueryPool()
{
	VkQueryPoolCreateInfo query_pool_create_info = {};
	query_pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	query_pool_create_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
	query_pool_create_info.queryCount = 16; //TODO: FIGURE OUT HOW MANY QUERY COUNTS I SHOULD HAVE!

	query_pool.resize(MAX_FRAMES);
	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		vkCreateQueryPool(device.logical, &query_pool_create_info, nullptr, &query_pool[i]);

	return true;
}

bool VkObj_Context::CreateCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info = {};
	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.queueFamilyIndex = device.q_family.graphics.value();

	CHECK_VKRESULT(r, vkCreateCommandPool(device.logical, &command_pool_create_info, nullptr, &command_pool), "Failed to create Command Pool!");
	return true;
}

bool VkObj_Context::CreateCommandBuffer(Object3D &object, uint32_t index)
{
	//Resize the Command Buffer to the Swapchain's buffer size
	object.command_buffer.resize(VkObj_Swapchain::swapchain_size);

	//Allocate Command buffer Information
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = command_pool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = CAST(uint32_t, object.command_buffer.size());

	//Allocate Command Buffer
	CHECK_VKRESULT(r, vkAllocateCommandBuffers(device.logical, &command_buffer_allocate_info, object.command_buffer.data()), "Failed to Allocate Command Buffer!");

// 	//Loop through all the allocated command buffer and set up Begin Info's (Command Buffer, Render Pass)
// 	for (unsigned int i = 0; i < object.command_buffer.size(); ++i)
// 	{
// 		//Create the Command Buffer's Begin Info
// 		VkCommandBufferBeginInfo command_buffer_begin_info = {};
// 		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
// 		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
// 		command_buffer_begin_info.pInheritanceInfo = nullptr;
// 
// 		//Begin the command buffer's begin process
// 		CHECK_VKRESULT(b, vkBeginCommandBuffer(object.command_buffer[i], &command_buffer_begin_info), "Failed to Begin Command Buffer's Begin Process at index " << i << "!");
// 
// 		//Create the Render Pass Begin Info
// 		VkRenderPassBeginInfo render_pass_begin_info = {};
// 		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
// 		render_pass_begin_info.framebuffer = swapchain.frame_buffers[i];
// 		render_pass_begin_info.renderArea.offset = { 0, 0 };
// 		render_pass_begin_info.renderArea.extent = swapchain.extent2D;
// 
// 		//Clear Value for both Screen and Depth Buffer
// 		render_pass_begin_info.renderPass = swapchain.render_pass;
// 		std::array<VkClearValue, 2> clear_color = {};
// 		clear_color[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
// 		clear_color[1].depthStencil = { 1.0f, 128 };
// 		render_pass_begin_info.clearValueCount = CAST(uint32_t, clear_color.size());
// 		render_pass_begin_info.pClearValues = clear_color.data();
// 
// 		/*
// 		 * Starting of the Drawing Stuff Entering Command Buffer {NOT THE ACTUAL DRAW!}
// 		 */
// 		vkCmdBeginRenderPass(object.command_buffer[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
// 
// 			std::array<VkBuffer, 1> vertex_buffer = { pObjectList[0][0].vertex_buffer };
// 			std::array<VkBuffer, 1> vertex_buffer2 = { pObjectList[0][1].vertex_buffer };
// 			VkDeviceSize offsets[] = { 0 };
// 
// 			vkCmdBindPipeline(object.command_buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0]);
// 			vkCmdBindDescriptorSets(object.command_buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, *object.pipeline_layout, 0, 1, &object.descriptor_set[i], 0, nullptr);
// 			
// // 			vkCmdBindVertexBuffers(object.command_buffer[i], 0, 1, vertex_buffer.data(), offsets);
// // 			vkCmdBindIndexBuffer(object.command_buffer[i], object.index_buffer, 0, VK_INDEX_TYPE_UINT32);
// // 			vkCmdDrawIndexed(object.command_buffer[i], CAST(uint32_t, object.indices.size()), 1, 0, 0, 0);
// // 
// // 			vkCmdBindVertexBuffers(object.command_buffer[i], 0, 1, vertex_buffer.data(), offsets);
// // 			vkCmdBindIndexBuffer(object.command_buffer[i], object.index_buffer, 0, VK_INDEX_TYPE_UINT32);
// // 			vkCmdDrawIndexed(object.command_buffer[i], CAST(uint32_t, object.indices.size()), 1, 0, 0, 0);
// 
//  			vkCmdBindVertexBuffers(object.command_buffer[i], 0, 1, vertex_buffer.data(), offsets);
//  			vkCmdBindIndexBuffer(object.command_buffer[i], pObjectList[0][0].index_buffer, 0, VK_INDEX_TYPE_UINT32);
//  			vkCmdDrawIndexed(object.command_buffer[i], CAST(uint32_t, pObjectList[0][0].indices.size()), 1, 0, 0, 0);
//  
//  			vkCmdBindVertexBuffers(object.command_buffer[i], 0, 1, vertex_buffer2.data(), offsets);
//  			vkCmdBindIndexBuffer(object.command_buffer[i], pObjectList[0][1].index_buffer, 0, VK_INDEX_TYPE_UINT32);
//  			vkCmdDrawIndexed(object.command_buffer[i], CAST(uint32_t, pObjectList[0][1].indices.size()), 1, 0, 0, 0);
// 
// 		
// 		vkCmdEndRenderPass(object.command_buffer[i]);
// 		/*
// 		 * Ending of the Drawing Stuff Entering Command Buffer {NOT THE ACTUAL DRAW!}
// 		 */
// 
// 		 //End the Command Buffer
// 		CHECK_VKRESULT(e, vkEndCommandBuffer(object.command_buffer[i]), "Failed to End Command Buffer at index " << i << ",");
// 	}

	//Command Buffer has been created successfully
	return true;
}

bool VkObj_Context::CreatePipelines()
{
#pragma region Check and copy shader files

	std::vector<char> shader_vertex_file = vk_read_shader_file("src/shaders/vert.spv");
	std::vector<char> shader_fragment_file = vk_read_shader_file("src/shaders/frag.spv");

	if (shader_vertex_file.empty() || shader_fragment_file.empty())
	{
		LOG("Failed to Load Shaders.");
		return false;
	}

	VkShaderModule shader_vertex_module = vk_create_shader_module(device.logical, shader_vertex_file);
	VkShaderModule shader_fragment_module = vk_create_shader_module(device.logical, shader_fragment_file);

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
	viewport.width = (float)swapchain.extent2D.width;
	viewport.height = (float)swapchain.extent2D.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = swapchain.extent2D;

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
	multisample_create_info.rasterizationSamples = device.msaa_support;
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
	
	pipeline_layout.push_back(VkPipelineLayout());
	CHECK_VKRESULT(pl, vkCreatePipelineLayout(device.logical, &pipeline_layout_create_info, nullptr, &pipeline_layout[pipeline_layout.size() - 1]), "Failed to create Pipeline Layout!");

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

	pipeline_create_info.layout = pipeline_layout[pipeline_layout.size() - 1];
	pipeline_create_info.renderPass = swapchain.render_pass;
	pipeline_create_info.subpass = 0;

	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex = -1;

	pipelines.push_back(VkPipeline());
	CHECK_VKRESULT(gp, vkCreateGraphicsPipelines(device.logical, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipelines[pipelines.size() - 1]), "Pipeline Creation Has Failed!");

#pragma endregion

	//Destroy Module
	vkDestroyShaderModule(device.logical, shader_vertex_module, nullptr);
	vkDestroyShaderModule(device.logical, shader_fragment_module, nullptr);

	return true;
}

bool VkObj_Context::CreateDescriptorPool()
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
	create_info.maxSets = 0xFF;

	CHECK_VKRESULT(r, vkCreateDescriptorPool(device.logical, &create_info, nullptr, &descriptor_pool), "Failed to create a Desriptor Pool! Error Code: ");

	return true;
}

bool VkObj_Context::CreateDescriptorSetLayout()
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

	CHECK_VKRESULT(a, vkCreateDescriptorSetLayout(device.logical, &create_info, nullptr, &descriptor_set_layout), "Failed to create Descriptor Set Layout!");

	return true;
}

void VkObj_Context::shutdown()
{
	vkDestroyDescriptorSetLayout(device.logical, descriptor_set_layout, nullptr);
	
	for (uint32_t i = 0; i < pipelines.size(); ++i)
		vkDestroyPipeline(device.logical, pipelines[i], nullptr);

	vkDestroyRenderPass(device.logical, swapchain.render_pass_no_clear, nullptr);
	
	if (command_pool)	vkDestroyCommandPool(device.logical, command_pool, nullptr);
	
	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		if (query_pool[i])	vkDestroyQueryPool(device.logical, query_pool[i], nullptr);

	vkDestroyDescriptorPool(device.logical, descriptor_pool, nullptr);
	
	device.shutdown();
	window.shutdown();
}

bool VkObj_Context::init(GLFWwindow *win, std::vector<Object3D> &object_list)
{
	//Create Instance, Validation Layer, and Surface
	window.init("New Vulkan Application", win);

	//Create Physical and Logical Device
	device.init(window);

	//Create the Pools
	CreateQueryPool();
	CreateCommandPool();

	//Create the swapchain
	swapchain.init(window, device, command_pool); 

	CreateDescriptorPool();
	CreateDescriptorSetLayout();
	CreatePipelines();
	pObjectList = &object_list;

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
