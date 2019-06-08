#include "pch.h"
#include "Mesh.h"

//   _____                         _                           _
//  / ____|                       | |                         | |
// | |        ___    _ __    ___  | |_   _ __   _   _    ___  | |_    ___    _ __
// | |       / _ \  | '_ \  / __| | __| | '__| | | | |  / __| | __|  / _ \  | '__|
// | |____  | (_) | | | | | \__ \ | |_  | |    | |_| | | (__  | |_  | (_) | | |
//  \_____|  \___/  |_| |_| |___/  \__| |_|     \__,_|  \___|  \__|  \___/  |_|
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices)
{
	prv_Vertices = vertices;
	prv_Indices = indices;
	
}

Mesh::Mesh(const char* fbx_filename, float _scale)
{
	prv_FBXFilename = fbx_filename;
	scale = _scale;
	ImportFbx(fbx_filename);
}

Mesh::Mesh(const OBJ_VERT* object_vertices, const unsigned int &vertices_size, const unsigned int* object_indices, const unsigned int & indices_size)
{
	prv_Vertices.resize(vertices_size);
	for (uint32_t i = 0; i < prv_Vertices.size(); ++i)
	{
		prv_Vertices[i].position.x = object_vertices[i].pos[0];
		prv_Vertices[i].position.y = object_vertices[i].pos[1];
		prv_Vertices[i].position.z = object_vertices[i].pos[2];

		prv_Vertices[i].uv.x = object_vertices[i].uvw[0];
		prv_Vertices[i].uv.y = object_vertices[i].uvw[1];

		prv_Vertices[i].normal.x = object_vertices[i].nrm[0];
		prv_Vertices[i].normal.x = object_vertices[i].nrm[0];
		prv_Vertices[i].normal.x = object_vertices[i].nrm[0];
	}

	prv_Indices.resize(indices_size);
	for (uint32_t i = 0; i < prv_Indices.size(); ++i)
		prv_Indices[i] = object_indices[i];
}

Mesh::~Mesh()
{
	cleanup();
}

//  _____            _       _   _
// |  __ \          | |     | | (_)
// | |__) |  _   _  | |__   | |  _    ___
// |  ___/  | | | | | '_ \  | | | |  / __|
// | |      | |_| | | |_) | | | | | | (__
// |_|       \__,_| |_.__/  |_| |_|  \___|
void Mesh::init()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
}

void Mesh::cleanup()
{
	if (index_buffer)	{ vkDestroyBuffer(myContext.device.logical, index_buffer, nullptr);	 index_buffer  = nullptr; }
	if (index_memory)	{ vkFreeMemory(myContext.device.logical, index_memory, nullptr);	 index_memory  = nullptr; }
	if (vertex_buffer)	{ vkDestroyBuffer(myContext.device.logical, vertex_buffer, nullptr); vertex_buffer = nullptr; }
	if (vertex_memory)	{ vkFreeMemory(myContext.device.logical, vertex_memory, nullptr);	 vertex_memory = nullptr; }
}

//  _____           _                   _
// |  __ \         (_)                 | |
// | |__) |  _ __   _  __   __   __ _  | |_    ___
// |  ___/  | '__| | | \ \ / /  / _` | | __|  / _ \
// | |      | |    | |  \ V /  | (_| | | |_  |  __/
// |_|      |_|    |_|   \_/    \__,_|  \__|  \___|
void Mesh::CreateVertexBuffer()
{
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	vk_create_buffer(myContext.device.physical, myContext.device.logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer, staging_buffer_memory);

	void* data;
	vkMapMemory(myContext.device.logical, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), CAST(uint32_t, buffer_size));
	vkUnmapMemory(myContext.device.logical, staging_buffer_memory);

	vk_create_buffer(myContext.device.physical, myContext.device.logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertex_buffer, vertex_memory);

	vk_copy_buffer(myContext.device.logical, myContext.command_pool, myContext.device.q_graphics, staging_buffer, vertex_buffer, buffer_size);

	vkDestroyBuffer(myContext.device.logical, staging_buffer, nullptr);
	vkFreeMemory(myContext.device.logical, staging_buffer_memory, nullptr);
}

void Mesh::CreateIndexBuffer()
{
	VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	vk_create_buffer(myContext.device.physical, myContext.device.logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

	void* data;
	vkMapMemory(myContext.device.logical, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices.data(), CAST(uint32_t, buffer_size));
	vkUnmapMemory(myContext.device.logical, staging_buffer_memory);

	vk_create_buffer(myContext.device.physical, myContext.device.logical, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		index_buffer, index_memory);

	vk_copy_buffer(myContext.device.logical, myContext.command_pool, myContext.device.q_graphics, staging_buffer, index_buffer, buffer_size);

	vkDestroyBuffer(myContext.device.logical, staging_buffer, nullptr);
	vkFreeMemory(myContext.device.logical, staging_buffer_memory, nullptr);
}

void Mesh::SetupTangent()
{
	uint32_t indices_size = CAST(uint32_t,prv_Indices.size());
	for (uint32_t i = 0; i < indices_size; i += 3)
	{
		glm::vec3 vert0 = prv_Vertices[i].position;
		glm::vec3 vert1 = prv_Vertices[i + 1].position;
		glm::vec3 vert2 = prv_Vertices[i + 2].position;

		glm::vec3 edge0 = vert1 - vert0;
		glm::vec3 edge1 = vert2 - vert0;
	}
}

// .----------------.  .----------------.  .----------------.        .----------------.  .----------------.  .----------------.
//| .--------------. || .--------------. || .--------------. |      | .--------------. || .--------------. || .--------------. |
//| |  _________   | || |   ______     | || |  ____  ____  | |      | |    _______   | || |  ________    | || |  ___  ____   | |
//| | |_   ___  |  | || |  |_   _ \    | || | |_  _||_  _| | |      | |   /  ___  |  | || | |_   ___ `.  | || | |_  ||_  _|  | |
//| |   | |_  \_|  | || |    | |_) |   | || |   \ \  / /   | |      | |  |  (__ \_|  | || |   | |   `. \ | || |   | |_/ /    | |
//| |   |  _|      | || |    |  __'.   | || |    > `' <    | |      | |   '.___`-.   | || |   | |    | | | || |   |  __'.    | |
//| |  _| |_       | || |   _| |__) |  | || |  _/ /'`\ \_  | |      | |  |`\____) |  | || |  _| |___.' / | || |  _| |  \ \_  | |
//| | |_____|      | || |  |_______/   | || | |____||____| | |      | |  |_______.'  | || | |________.'  | || | |____||____| | |
//| |              | || |              | || |              | |      | |              | || |              | || |              | |
//| '--------------' || '--------------' || '--------------' |      | '--------------' || '--------------' || '--------------' |
// '----------------'  '----------------'  '----------------'        '----------------'  '----------------'  '----------------'
void Mesh::ImportFbx(const char* fbx_filename)
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

void Mesh::ProcessFbxMesh(FbxNode* node)
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
					prv_Vertices[j].position = { vert.mData[0] / scale, vert.mData[1] / scale, vert.mData[2] / scale };

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

void Mesh::SetUVs(FbxArray<FbxVector2>& uv, const FbxMesh* mesh)
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

void Mesh::Compactify(const std::vector<Vertex>& vertex2)
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

void Mesh::GetTextureFilename(FbxNode* child_node, const char* return_value)
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
					memset(filename, 0, (length + 1) * sizeof(wchar_t));

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

					int64_t str_len = (int32_t)wcslen(filename);
					char* texture_filename = new char[str_len + 1];
					for (int64_t i = str_len; i >= 0; --i)
						texture_filename[i] = (char)filename[i];

// 					int64_t str_filelen = strlen(prv_TextureFileLocation);
// 					if (str_filelen)
// 					{
// 						char* total_filename = new char[str_filelen + str_len + 1];
// 						ZeroMemory(total_filename, str_filelen + str_len + 1);
// 						strcat(total_filename, prv_TextureFileLocation);
// 						strcat(total_filename, texture_filename);
// 						prv_TextureFilename = total_filename;
// 						delete[] filename;
// 						delete[] texture_filename;
// 					}
// 					else
// 					{
						prv_TextureFilename = texture_filename;
						delete[] filename;
//					}


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

