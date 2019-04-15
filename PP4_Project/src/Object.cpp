#include "Object.h"

Object3D::Object3D(const char* fbx_filename)
{
	ImportFbx(fbx_filename);
	prv_Texture = new Texture(prv_TextureFilename);
}

Object3D::Object3D(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, Texture* texture_dot_h)
{
	prv_Vertices = vertices;
	prv_Indices = indices;
	prv_Texture = texture_dot_h;
}

Object3D::Object3D(const char* fbx_filename, Texture* texture_dot_h)
{
	ImportFbx(fbx_filename);
	prv_Texture = texture_dot_h;
}

Object3D::~Object3D()
{
	if (prv_Texture->on_heap)	delete prv_Texture; 
		else prv_Texture = nullptr;
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
	int indices_count = (uint32_t)prv_Indices.size();
	
	prv_Indices.clear();
	prv_Vertices.clear();

	for (int i = 0; i < indices_count; ++i)
	{
		for (unsigned int j = 0; j <= prv_Vertices.size(); ++j)
		{
			if (j == prv_Vertices.size())
			{
				prv_Indices.push_back((uint32_t)prv_Vertices.size());
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
 					uint32_t length = (uint32_t)str.GetLen();
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
 
 					const uint32_t inverse = (uint32_t)(str.GetLen() - length - 1);
 					const uint32_t invdiv2 = (uint32_t)ceil(inverse * 0.5f);
 
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