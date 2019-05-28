#include "Object.h"

VkObj_DeviceProperties *Object3D::pDevice = nullptr;
VkCommandPool *Object3D::pCommandPool = nullptr;
VkDescriptorSetLayout *Object3D::pDescriptorSetLayout = nullptr;

Object3D::Object3D(const char* fbx_filename, Texture* texture_dot_h, float _scale)
{
	scale = _scale;
	ImportFbx(fbx_filename);
	if (!texture_dot_h)
		prv_Texture = new Texture(prv_TextureFilename);
}

Object3D::Object3D(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, Texture* texture_dot_h)
{
	prv_Vertices = vertices;
	prv_Indices = indices;
	prv_Texture = texture_dot_h;
}

Object3D::~Object3D()
{
	return;
}

void Object3D::set_static_contexts(VkObj_DeviceProperties &device, VkCommandPool &command_pool)
{
	pDevice = &device;
	pCommandPool = &command_pool;
}

void Object3D::init(const uint32_t &sizeof_ubuffer, VkPipelineLayout &graphics_pipeline_layout, VkPipeline &graphic_pipeline, const uint32_t &swapchain_vec_size)
{
	ubuffer_range = sizeof_ubuffer;
	pipeline_layout = &graphics_pipeline_layout;
	pipeline = &graphic_pipeline;
	swapchain_size = swapchain_vec_size;

	prv_Texture->init(*pDevice, *pCommandPool);
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffer();
}

void Object3D::set_image_view(const VkComponentSwizzle &red, const VkComponentSwizzle &green, const VkComponentSwizzle &blue, const VkComponentSwizzle &alpha)
{
// 	//Image View Create Info
// 	VkImageViewCreateInfo create_info = {};
// 	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
// 	create_info.image = image;
// 	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
// 	create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
// 	create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
// 	create_info.subresourceRange.baseMipLevel = 0;
// 	create_info.subresourceRange.levelCount = texture->mip_levels;
// 	create_info.subresourceRange.baseArrayLayer = 0;
// 	create_info.subresourceRange.layerCount = 1;
// 	create_info.components.r = component_r;
// 	create_info.components.g = component_g;
// 	create_info.components.b = component_b;
// 	create_info.components.a = component_a;
// 
// 	//Create the Surface (With Results) [VK_SUCCESS = 0]
// 	vkCreateImageView(pDevice->logical, &create_info, nullptr, &image_view), "Failed to Create Image View";
}

void Object3D::ProcessFbxMesh(FbxNode* node)
{
	//FBX Mesh stuff
	int childrenCount = node->GetChildCount();

	LOG("\nName:" << node->GetName())

	for (int i = 0; i < childrenCount; i++)
	{
		FbxNode *childNode = node->GetChild(i);
		FbxMesh *mesh = childNode->GetMesh();
		FbxArray<FbxVector4> normalsVec;
		FbxArray<FbxVector2> TexUV;

		if (mesh != NULL)
		{
			LOG("\nMesh:" << childNode->GetName())

			// Get index count from mesh
			int* PolygonIndices = mesh->GetPolygonVertices();
			prv_Indices.resize(mesh->GetPolygonVertexCount());
			LOG("\nIndice Count:" << indices.size())

 			// No need to allocate int array, FBX does for us
			for (uint32_t i = 0; i < indices.size(); ++i)
				prv_Indices[i] = PolygonIndices[i];

 			prv_Vertices.resize(mesh->GetControlPointsCount());
 			LOG("\nVertex Count:" << prv_Vertices.size())
 
 			// Get UV from mesh
 			SetUVs(TexUV, mesh);


 			// Get Normal count from mesh
 			mesh->GetPolygonVertexNormals(normalsVec);
 			LOG("\nNormalVec Count:" << normalsVec.Size())
 
			// Declare a new array for the second vertex array
			// Note the size is numIndices not numVertices
 			std::vector<Vertex> vertices2(prv_Indices.size());
 
 			LOG("\nindex count BEFORE/AFTER compaction " << prv_Indices.size())
 			LOG("\nvertex count ORIGINAL (FBX source): " << prv_Vertices.size())

 			//================= Process Vertices ===================
 			for (int j = 0; j < prv_Vertices.size(); j++)
 			{
 				FbxVector4 vert = mesh->GetControlPointAt(j);
				prv_Vertices[j].position = { vert.mData[0] / scale, vert.mData[1] / scale, vert.mData[2] / scale, 1.0f };
 
 				// Generate random normal
 				vertices[j].normal = RAND_NORMAL;
 			}
 
 			// align (expand) vertex array and set the normals
 			for (int j = 0; j < prv_Indices.size(); ++j)
 			{
 				vertices2[j].position = vertices[indices[j]].position;
 				vertices2[j].normal = glm::vec3(normalsVec[j].mData[0], normalsVec[j].mData[1], normalsVec[j].mData[2]);
 				vertices2[j].uv = glm::vec2(TexUV[j].mData[0], TexUV[j].mData[1]);
 			}
 
 			// vertices is an "out" var so make sure it points to the new array
 			// and clean up first array
 
 			// make new indices to match the new vertex(2) array
 			for (int j = 0; j < prv_Indices.size(); j++)
 			{
				prv_Indices[j] = j;
 			}
 
			LOG("\nvertex count AFTER expansion: " << prv_Indices.size())
 

			if (true)
 			{
				Compactify(vertices2);
 			}
 			else
 			{
				prv_Vertices.clear();
				prv_Vertices.resize(prv_Indices.size());
				for (uint32_t i = 0; i < indices.size(); ++i)
					prv_Vertices[i] = vertices2[i];
 			}

 			GetTextureFilename(childNode, prv_TextureFilename);
// 
 		}
// 
 		ProcessFbxMesh(childNode);
 	}
}

void Object3D::Compactify(const std::vector<Vertex>& vertex2)
{
	/*Compactify*/
	const float epsilon = 0.0001f;
	size_t indices_count = prv_Indices.size();
	
	prv_Indices.clear();
	prv_Vertices.clear();

	for (int i = 0; i < indices_count; ++i)
	{
		for (unsigned int j = 0; j <= prv_Vertices.size(); ++j)
		{
			if (j == prv_Vertices.size())
			{
				prv_Indices.push_back(CAST(uint32_t, prv_Vertices.size()));
				prv_Vertices.push_back(Vertex(vertex2[i]));
				break;
			}
			else if ((abs(vertex2[i].position.x - prv_Vertices[j].position.x)) < epsilon &&
				(abs(vertex2[i].position.y - prv_Vertices[j].position.y)) < epsilon &&
				(abs(vertex2[i].position.z - prv_Vertices[j].position.z)) < epsilon &&
	
				(abs(vertex2[i].normal.x - prv_Vertices[j].normal.x)) < epsilon &&
				(abs(vertex2[i].normal.y - prv_Vertices[j].normal.y)) < epsilon &&
				(abs(vertex2[i].normal.z - prv_Vertices[j].normal.z)) < epsilon &&
	
				(abs(vertex2[i].position.x - prv_Vertices[j].position.x)) < epsilon &&
				(abs(vertex2[i].position.y - prv_Vertices[j].position.y)) < epsilon)
			{
				prv_Indices.push_back(j);
				break;
			}
	
		}
	}
	
	
	LOG("\nvertex count AFTER compaction: " << prv_Vertices.size())
	LOG("\nSize reduction: " << ((float)(prv_Indices.size() - prv_Vertices.size()) / (float)(prv_Indices.size())) * 100.00f << "%")
	LOG("\nor " << prv_Vertices.size() / (float)prv_Indices.size() << " of the expanded size")
}

void Object3D::SetUVs(FbxArray<FbxVector2>& uv, const FbxMesh* mesh)
{
 	//get all UV set names
 	FbxStringList lUVSetNameList;
 	mesh->GetUVSetNames(lUVSetNameList);
 
 	//iterating over all uv sets
 	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
 	{
 		//get all UV set names
 		FbxStringList lUVSetNameList;
 		mesh->GetUVSetNames(lUVSetNameList);
 
 		//iterating over all uv sets
 		for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
 		{
 			//get lUVSetIndex-th uv set
 			const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
 			const FbxGeometryElementUV* lUVElement = mesh->GetElementUV(lUVSetName);
 
 			if (!lUVElement)
 				continue;
 
 			// only support mapping mode eByPolygonVertex and eByControlPoint
 			if (//lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint &&
 				lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex)
 				return;
 
 			//index array, where holds the index referenced to the uv data
 			const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
 			const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
 
 			//iterating through the data by polygon
 			const int lPolyCount = mesh->GetPolygonCount();
 
 			/*if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
 			{
 				for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
 				{
 					// build the max index array that we need to pass into MakePoly
 					const int lPolySize = mesh->GetPolygonSize(lPolyIndex);
 					for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
 					{
 						FbxVector2 lUVValue;
 
 						//get the index of the current vertex in control points array
 						int lPolyVertIndex = mesh->GetPolygonVertex(lPolyIndex, lVertIndex);
 
 						//the UV index depends on the reference mode
 						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;
 
 						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
 
 						//User TODO:
 						//Print out the value of UV(lUVValue) or log it to a file
 					}
 				}
 			}
 			else */if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
 			{
 				int lPolyIndexCounter = 0;
 				for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
 				{
 					// build the max index array that we need to pass into MakePoly
 					const int lPolySize = mesh->GetPolygonSize(lPolyIndex);
 					for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
 					{
 						if (lPolyIndexCounter < lIndexCount)
 						{
 							/*FbxVector2 lUVValue;
 
 							//the UV index depends on the reference mode
 							int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;
 
 							lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
 
 							//User TODO:
 							//Print out the value of UV(lUVValue) or log it to a file
 
 							lPolyIndexCounter++;*/
 							int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;
 							uv.Add(lUVElement->GetDirectArray().GetAt(lUVIndex));
 							lPolyIndexCounter++;
 						}
 					}
 				}
 			}
 		}
 	}
}

void Object3D::GetTextureFilename(FbxNode* child_node, const char* return_value)
{
 	//================= Texture ========================================
 	int materialCount = child_node->GetSrcObjectCount<FbxSurfaceMaterial>();
 
 	for (int index = 0; index < materialCount; index++)
 	{
 		FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)child_node->GetSrcObject<FbxSurfaceMaterial>(index);
 
 		if (material != NULL)
 		{
 			LOG("material: " << material->GetName())
 			// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
 			FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
 
 			// Check if it's layered textures
 			int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
 
 			if (layeredTextureCount > 0)
 			{
 				for (int j = 0; j < layeredTextureCount; j++)
 				{
 					FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
 					int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();
 
 					for (int k = 0; k < lcount; k++)
 					{
 						FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
 						// Then, you can get all the properties of the texture, include its name
 						const char* textureName = texture->GetFileName();
 						LOG(textureName)
 					}
 				}
 			}
 			else
 			{
 				// Directly get textures
 				int textureCount = prop.GetSrcObjectCount<FbxTexture>();
 				for (int j = 0; j < textureCount; j++)
 				{
 					FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxTexture>(j));
 					// Then, you can get all the properties of the texture, include its name
 					const char* textureName = texture->GetFileName();
 					LOG(textureName)
 
 					/*Reading Filename:
 						The idea is to read it backwards, convert extension to dds, and then swap the letters.
 						for example: Crate.jpg [Scale = 50.0f]
 						Step 0: \0\0\0\0\0\0\0\0\0\0
 						Step 1: \0gpj.etarC
 						Step 2: \0sdd.etarC
 						Step 3: Crate.dds\0
 						Step 4: CreateDDSTextureFromFile(..,filename,..,..)
 
 						example2: .\\axeTexture.bmp [Scale = 5.0f]
 						Step 0: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0
 						Step 1: \0pmb.erutxeTexa\0\0\0
 						Step 2: \0ssd.erutxeTexa\0\0\0
 						Step 3: axeTexture.dds\0\0\0\0
 						Step 4: CreateDDSTextureFromFile(..,filename,..,..)
 					 */
 
 					FbxString str = texture->GetRelativeFileName();
 					uint32_t length = CAST(uint32_t, str.GetLen());
 					wchar_t* filename = new wchar_t[length + 1];
 					memset(filename,0, (length + 1) * sizeof(wchar_t));
 
 					for (int i = 0; length >= 0; --length)
 					{
 						if (str[length] == '\\')
 							break;
 
 						filename[i++] = str[length];
 					}
//  					filename[1] = 's';
//  					filename[2] = 'd';
//  					filename[3] = 'd';
 
 					const uint32_t inverse = CAST(uint32_t, (str.GetLen() - length - 1));
 					const uint32_t invdiv2 = CAST(uint32_t, ceil(inverse * 0.5f));
 
 					for (uint32_t i = 0; i < invdiv2; ++i)
 					{
 						filename[i] ^= filename[inverse - i];
 						filename[inverse - i] ^= filename[i];
 						filename[i] ^= filename[inverse - i];
 					}

					int32_t str_len = (int32_t)wcslen(filename);
					char* texture_filename = new char[str_len + 1];
					for (int32_t i = str_len; i >= 0; --i)
						texture_filename[i] = (char)filename[i];
					prv_TextureFilename = texture_filename;
 					//HRESULT hr;
 
 					// Load the Texture
 					//hr = CreateDDSTextureFromFile(g_pd3dDevice, filename, nullptr, &g_pShaderResource);
 					//if (hr != S_OK)
 						//std::cout << "THERE IS SOMETHING WRONG!!!!!!!!!!!";
 					FbxProperty p = texture->RootProperty.Find("Filename");
					if (p.IsValid()) {
						LOG(p.Get<FbxString>() << std::endl)
					}
 				}
 			}
 		}
 	}
}

void Object3D::ImportFbx(const char* fbx_filename)
{
	// Change the following filename to a suitable filename value.
	const char* lFilename = fbx_filename;

	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.
	lImporter->Destroy();

	// Process the scene and build DirectX Arrays
	ProcessFbxMesh(lScene->GetRootNode());
}

void Object3D::CreateImage()
{
// 	//Get the image size for the texture
// 	VkDeviceSize image_size = prv_Texture->width * prv_Texture->height * sizeof(unsigned int);
// 
// 	//Get the staging bugger and memory needed to allocate
// 	VkBuffer staging_buffer;
// 	VkDeviceMemory staging_buffer_memory;
// 
// 	//Create the staging buffer
// 	vk_create_buffer(pDevice->physical, pDevice->logical, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
// 		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
// 
// 	void* pixel_data;
// 	if (texture->data_h)
// 		pixel_data = (void*)texture->data_h;
// 	else
// 		pixel_data = (void*)texture->data;
// 
// 	//Allocate the data into the buffer
// 	void* data = nullptr;
// 	vkMapMemory(pDevice->logical, staging_buffer_memory, 0, image_size, 0, &data);
// 	memcpy(data, pixel_data, (unsigned int)image_size);
// 	vkUnmapMemory(pDevice->logical, staging_buffer_memory);
// 
// 	VkExtent3D extent = { texture->width, texture->height, 1 };
// 	//Create the image, using appropriate information (Mip Levels, Texture data, etc.)
// 	vk_create_image(pDevice->physical, pDevice->logical, extent, texture->mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
// 		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, image_memory);
// 
// 	//Transition, using memory barriers, from Undefined Layout to Transfer to Destination (Optimal)
// 	vk_transition_image_layout(pDevice->logical, *pCommandPool, pDevice->q_graphics, texture->mip_levels, image, 
// 		VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
// 
// 	//Copy the buffer to the image
// 	vk_copy_buffer_to_image(pDevice->logical, *pCommandPool, pDevice->q_graphics, staging_buffer, image, extent);
// 
// 	//Destroy memory created from staging buffer
// 	vkDestroyBuffer(pDevice->logical, staging_buffer, nullptr);
// 	vkFreeMemory(pDevice->logical, staging_buffer_memory, nullptr);
// 
// 	//Create the mipmaps for texture
// 	vk_create_mipmaps(pDevice->logical, *pCommandPool, pDevice->q_graphics, image, texture->width, texture->height, texture->mip_levels);
// 
// 	//Create Image View
// 	set_image_view();
}

void Object3D::CreateSampler()
{
// 	//Create the sampler create info
// 	VkSamplerCreateInfo sampler_create_info = {};
// 	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
// 	sampler_create_info.magFilter = VK_FILTER_LINEAR;
// 	sampler_create_info.minFilter = VK_FILTER_LINEAR;
// 	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
// 	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
// 	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
// 	sampler_create_info.anisotropyEnable = false;
// 	sampler_create_info.maxAnisotropy = 1;
// 	sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
// 	sampler_create_info.unnormalizedCoordinates = false;
// 	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
// 	sampler_create_info.mipLodBias = 0.0f;
// 	sampler_create_info.minLod = 0.0f;
// 	sampler_create_info.maxLod = 0.0f;
// 
// 	vkCreateSampler(pDevice->logical, &sampler_create_info, nullptr, &sampler);
}

void Object3D::CreateVertexBuffer()
{
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	vk_create_buffer(pDevice->physical, pDevice->logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer, staging_buffer_memory);

	void* data;
	vkMapMemory(pDevice->logical, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), CAST(uint32_t, buffer_size));
	vkUnmapMemory(pDevice->logical, staging_buffer_memory);

	vk_create_buffer(pDevice->physical, pDevice->logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertex_buffer, vertex_memory);

	vk_copy_buffer(pDevice->logical, *pCommandPool, pDevice->q_graphics, staging_buffer, vertex_buffer, buffer_size);

	vkDestroyBuffer(pDevice->logical, staging_buffer, nullptr);
	vkFreeMemory(pDevice->logical, staging_buffer_memory, nullptr);
}

void Object3D::CreateIndexBuffer()
{
	VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	vk_create_buffer(pDevice->physical, pDevice->logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

	void* data;
	vkMapMemory(pDevice->logical, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices.data(), CAST(uint32_t, buffer_size));
	vkUnmapMemory(pDevice->logical, staging_buffer_memory);

	vk_create_buffer(pDevice->physical, pDevice->logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		index_buffer, index_memory);

	vk_copy_buffer(pDevice->logical, *pCommandPool, pDevice->q_graphics, staging_buffer, index_buffer, buffer_size);

	vkDestroyBuffer(pDevice->logical, staging_buffer, nullptr);
	vkFreeMemory(pDevice->logical, staging_buffer_memory, nullptr);
}

void Object3D::CreateUniformBuffer()
{
	VkDeviceSize buffer_size = sizeof(Mvp_object);

	uniform_buffer.resize(swapchain_size);
	uniform_memory.resize(swapchain_size);

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		vk_create_buffer(pDevice->physical, pDevice->logical, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniform_buffer[i], uniform_memory[i]);
	}
}

bool Object3D::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> descriptor_pool_size = {};
	descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_pool_size[0].descriptorCount = swapchain_size;
	descriptor_pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_pool_size[1].descriptorCount = swapchain_size;

	VkDescriptorPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = CAST(uint32_t, descriptor_pool_size.size());
	create_info.pPoolSizes = descriptor_pool_size.data();
	create_info.maxSets = swapchain_size;

	CHECK_VKRESULT(r, vkCreateDescriptorPool(pDevice->logical, &create_info, nullptr, &descriptor_pool), "Failed to create a Desriptor Pool! Error Code: ");

	return true;
}

bool Object3D::CreateDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> descriptor_set_layout_vector(swapchain_size, *pDescriptorSetLayout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate_info.descriptorSetCount = swapchain_size;
	descriptor_set_allocate_info.descriptorPool = descriptor_pool;
	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout_vector.data();

	descriptor_set.resize(swapchain_size); //Was Based on Swapchain Size
	CHECK_VKRESULT(r, vkAllocateDescriptorSets(pDevice->logical, &descriptor_set_allocate_info, descriptor_set.data()), "Failed to Allocate Descriptor Set!");

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		VkDescriptorBufferInfo descriptor_buffer_info = {};
		descriptor_buffer_info.buffer = uniform_buffer[i];
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = ubuffer_range;

		VkDescriptorImageInfo descriptor_image_info = {};
		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_image_info.imageView = image_view;
		descriptor_image_info.sampler = sampler;

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

		vkUpdateDescriptorSets(pDevice->logical, CAST(uint32_t, write_descriptor_set.size()), write_descriptor_set.data(), 0, nullptr);
	}

	return true;
}

bool Object3D::CreateDescriptorSet(const VkDescriptorPool &descriptor_pool, const VkDescriptorSetLayout &descriptor_set_layout)
{
	std::vector<VkDescriptorSetLayout> descriptor_set_layout_vector(swapchain_size, descriptor_set_layout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate_info.descriptorSetCount = swapchain_size;
	descriptor_set_allocate_info.descriptorPool = descriptor_pool;
	descriptor_set_allocate_info.pSetLayouts = descriptor_set_layout_vector.data();

	descriptor_set.resize(swapchain_size); //Was Based on Swapchain Size
	CHECK_VKRESULT(r, vkAllocateDescriptorSets(pDevice->logical, &descriptor_set_allocate_info, descriptor_set.data()), "Failed to Allocate Descriptor Set!");

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		VkDescriptorBufferInfo descriptor_buffer_info = {};
		descriptor_buffer_info.buffer = uniform_buffer[i];
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = ubuffer_range;

		VkDescriptorImageInfo descriptor_image_info = {};
		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_image_info.imageView = prv_Texture->image_view;
		descriptor_image_info.sampler = prv_Texture->sampler;

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

		vkUpdateDescriptorSets(pDevice->logical, CAST(uint32_t, write_descriptor_set.size()), write_descriptor_set.data(), 0, nullptr);
	}

	return true;
}

void Object3D::UpdateUniformBuffer(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view)
{
	if (uniformFctn)
		uniformFctn(device, swapchain_extent, current_image, model, uniform_memory, view);
}

void Object3D::reset()
{
	if (!command_buffer.empty())
	{
		vkFreeCommandBuffers(pDevice->logical, *pCommandPool, command_buffer.size(), command_buffer.data());
		command_buffer.clear();
	}

	for (uint32_t i = 0; i < swapchain_size; ++i)
	{
		vkDestroyBuffer(pDevice->logical, uniform_buffer[i], nullptr);
		vkFreeMemory(pDevice->logical, uniform_memory[i], nullptr);
	}
	uniform_buffer.clear();
	uniform_memory.clear();
}

void Object3D::cleanup()
{
	if (prv_Texture)	prv_Texture->cleanup(*pDevice);
	if (index_buffer)	vkDestroyBuffer(pDevice->logical, index_buffer, nullptr);
	if (index_memory)	vkFreeMemory(pDevice->logical, index_memory, nullptr);
	if (vertex_buffer)	vkDestroyBuffer(pDevice->logical, vertex_buffer, nullptr);
	if (vertex_memory)	vkFreeMemory(pDevice->logical, vertex_memory, nullptr);
}
