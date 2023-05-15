#include "EnginePCH.h"
#include "AssimpLoader.h"
#include <assimp/Exporter.hpp>
#include <assimp/DefaultLogger.hpp>
#include "Path/PathManager.h"
#include "AssetManager.h"
#include "Assets/Mesh/SkeletalMeshAsset.h"
#include "Assets/Mesh/Skeleton.h"
#include "ASsets/Animation/AnimationSequence.h"


static Matrix Convert_Matrix(const aiMatrix4x4& transform)
{
	return Matrix
	(
		transform.a1, transform.b1, transform.c1, transform.d1,
		transform.a2, transform.b2, transform.c2, transform.d2,
		transform.a3, transform.b3, transform.c3, transform.d3,
		transform.a4, transform.b4, transform.c4, transform.d4
	);
}

static SVector4 convert_vector4(const aiColor4D& ai_color)
{
	return SVector4(ai_color.r, ai_color.g, ai_color.b, ai_color.a);
}

static SVector3 convert_vector3(const aiVector3D& ai_vector)
{
	return SVector3(ai_vector.x, ai_vector.y, ai_vector.z);
}

static SVector2 convert_vector2(const aiVector2D& ai_vector)
{
	return SVector2(ai_vector.x, ai_vector.y);
}

static SVector4 convert_quaternion(const aiQuaternion& ai_quaternion)
{
	return SVector4(ai_quaternion.x, ai_quaternion.y, ai_quaternion.z, ai_quaternion.w);
}

static  void set_entity_transform(const aiNode* node, SAssimpNode* converting)
{
	if (!converting)
		return;

	// Convert to engine matrix
	const Matrix matrix_engine = Convert_Matrix(node->mTransformation);

	matrix_engine.DecomposeMatrix(converting->relScale3D, converting->relRotQuat, converting->relPos);

	converting->testMatrix = matrix_engine;	
	converting->testMatrixInv = matrix_engine;
	converting->testMatrixInv.Inverse();
	// Apply position, rotation and scale
	converting->testGlobalTrans = converting->ParentTransform.ToMatrixWithScale()* converting->testMatrix;
	
}



AssimpLoader::AssimpLoader()
{
	m_NumBones = 0;
}

AssimpLoader::~AssimpLoader()
{
	size_t entitySize = m_EntityNodes.size();
	for(size_t i =0; i< entitySize;i++)
	{
		SAFE_DELETE(m_EntityNodes[i]);
	}
}

bool AssimpLoader::Load( MeshAsset*& model, const char* FullPath)
{
	m_FullPath = FullPath;

	fs::path assetMetaPath = FullPath;
	
 	AssetData* converted =  g_Engine->Get<AssetManager>()->FindAssetFromFileName(assetMetaPath.filename().string());
	if(converted)
	{
		return true;
	}

	if (assetMetaPath.extension() != ".meta")
	{
		assetMetaPath.replace_extension(".meta");
		//assetMetaPath += ".meta";
	}


	//로드 대상이 이미 Convert를 끝낸 파일을 가지고 있는지?	
	if (FileSystem::FileExists(assetMetaPath))
	{
		MeshAsset* meshAsset = (MeshAsset*)g_Engine->Get<AssetManager>()->Load(AssetInfo(assetMetaPath.string()));
		//Load From Binary
		if (meshAsset->GetMeshCount() != 0)
			return true;
	}
		

	//aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
	constexpr auto ASSIMP_LOAD_FLAGS =
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		//aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals		|
		aiProcess_GenUVCoords			|
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality	|     // re-order triangles for better vertex cache locality.
		//aiProcess_LimitBoneWeights		|
		aiProcess_SortByPType |
		aiProcess_FindDegenerates		|          // convert degenerate primitives to proper lines or points.
		aiProcess_FindInvalidData |
		aiProcess_FindInstances |
		aiProcess_ValidateDataStructure |
		aiProcess_EmbedTextures |
		aiProcess_TransformUVCoords |
		aiProcess_OptimizeMeshes;

		
	Assimp::Importer importer;	
	//importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 1);	
	
	
	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_OPTIMIZE_EMPTY_ANIMATION_CURVES, false);
	
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);	
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);


	Assimp::DefaultLogger::create("AssimpLog_Cpp.txt", Assimp::Logger::VERBOSE,
								  aiDefaultLogStream_STDOUT | aiDefaultLogStream_DEBUGGER | aiDefaultLogStream_FILE);
	

	
	const aiScene* scene = importer.ReadFile(FullPath, ASSIMP_LOAD_FLAGS);	



	//scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || 
	if (!scene || !scene->mRootNode)
	{		
		return false;
	}		

	m_pScene = scene;
	
	m_GlobalInverseTransform = Convert_Matrix(m_pScene->mRootNode->mTransformation);
	m_GlobalInverseTransform.Inverse();


	

	SAssimpNode* new_entity = new SAssimpNode;
	m_EntityNodes.push_back(new_entity);

	ParseNode(scene->mRootNode, nullptr, new_entity, scene);

	AssetInfo assetInfo(assetMetaPath.string());	
	

	Converting(model,scene);	

	importer.FreeScene();	
	Assimp::DefaultLogger::kill();	
	


	return true;
}

void AssimpLoader::Converting(class MeshAsset*& model, const aiScene* pAiScene)
{
	Renderer* rdr = g_Engine->Get<Renderer>();


	bool createMeshAsset = true;
	Skeleton* convertedSkeleton = nullptr;
	XClass* createMeshType = MeshAsset::StaticClass();
	if (m_BoneInfo.size() > 0)
	{
		createMeshType = SkeletalMeshAsset::StaticClass();		
	} 
	else
	{
		createMeshType = MeshAsset::StaticClass();		
	}

	size_t entSize = m_EntityNodes.size();
	aiString texture_path;
	for(size_t i=0;i< entSize;i++)
	{
		//Convert Mesh Vertices
		int32 meshID = INDEX_NONE;
		if(m_EntityNodes[i]->Vertices.size())
		{					
			std::string meshName = m_EntityNodes[i]->Name;			
			
			AssetData* found = g_Engine->Get<AssetManager>()->FindAssetFromName(meshName, MeshAsset::StaticClass());
			if(found)
			{
				createMeshAsset = false;
				model = (MeshAsset*)found;
			}
			else if(createMeshAsset)
			{
				createMeshAsset = false;//Created
				fs::path meshPath = m_FullPath;
				meshPath._Remove_filename_and_separator();
				meshPath /= meshName;
				model = (MeshAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(meshPath.string()),createMeshType);
				model->SetFromFile(m_FullPath.filename().string());				
			}
			Mesh convertedMesh = Mesh(m_EntityNodes[i]->Name);
			size_t BoneCnt = m_EntityNodes[i]->Bones.size();
			for (size_t b = 0; b < BoneCnt; b++)
			{
				m_EntityNodes[i]->Vertices[b].BlendWeight = SVector4(0.0);
				m_EntityNodes[i]->Vertices[b].BlendIndex = SVector4(0.0);
				//maybe 4
				for(size_t x = 0;x<m_EntityNodes[i]->Bones[b].vecWeigths.size(); x++)
				{
					m_EntityNodes[i]->Vertices[b].BlendWeight[(int32)x] = m_EntityNodes[i]->Bones[b].vecWeigths[x];
				}

				for (size_t z = 0; z < m_EntityNodes[i]->Bones[b].vecIndex.size(); z++)
				{
					m_EntityNodes[i]->Vertices[b].BlendIndex[(int32)z] = (float)m_EntityNodes[i]->Bones[b].vecIndex[z];
				}				
			}

			convertedMesh.Initialize<SVertex>(m_EntityNodes[i]->Vertices, m_EntityNodes[i]->Indices);
			meshID = model->AddMesh(convertedMesh);

			if(createMeshType->IsA(SkeletalMeshAsset::StaticClass()))
			{				
				fs::path modelPath = model->GetInfo().GetSPath();
				std::string SkeletonFolderName = modelPath.filename().replace_extension("").replace_extension("").string();
				modelPath._Remove_filename_and_separator();

				const SPathInfo* skelRootInfo = g_Engine->Get<PathManager>()->FindPath(SKELETON_PATH);
				fs::path skelRootPath = skelRootInfo->aPath;
				skelRootPath /= SkeletonFolderName;				
				//Create Skeleton 
				Skeleton* skeleton = (Skeleton*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(skelRootPath.string()), Skeleton::StaticClass());
				if(skeleton)
				{
					if(!skeleton->m_BoneInfos.size())
					{
						size_t BoneCnt = m_BoneInfo.size();
						skeleton->m_BoneInfos.resize(BoneCnt);
						for (size_t b = 0; b < BoneCnt; b++)
						{					

							Matrix matReflect = Matrix(1.0, 0.0, 0.0, 0.0,
													   0.0, 0.0, 1.0, 0.0,
													   0.0, -1.0, 0.0, 0.0,
													   0.0, 0.0, 0.0, 1.0);		
							Matrix matReflect2 = Matrix(1.0, 0.0, 0.0, 0.0,
													   0.0, 0.0, -1.0, 0.0,
													   0.0, 1.0, 0.0, 0.0,
													   0.0, 0.0, 0.0, 1.0);

							skeleton->m_BoneInfos[b] = m_BoneInfo[b];
							skeleton->m_BoneInfos[b].matOffset = matReflect * m_BoneInfo[b].matOffset * matReflect2;
							m_BoneInfo[b].matOffset = skeleton->m_BoneInfos[b].matOffset;
							
							
							
						}
						
						convertedSkeleton = skeleton;
					}
					else
					{
						//AlreadyCraeted
					}

					SkeletalMeshAsset* skeletalModel = (SkeletalMeshAsset*)model;
					convertedSkeleton = skeletalModel->m_Skeleton = skeleton;
					
				}		
				
				
			}
			
		}
		
		

		if (m_EntityNodes[i]->MeshMaterial)
		{
			//FileName.FBX.meta
			fs::path modelPath = model->GetInfo().GetSPath();	
		
			std::string MaterialTextureFolderName = modelPath.filename().replace_extension("").replace_extension("").string();
			modelPath._Remove_filename_and_separator();
			
			//Create MaterialAsset
			//MaterialPath/FBX'sFileName/mesh's.meta
			const SPathInfo* matRootInfo = g_Engine->Get<PathManager>()->FindPath(MATERIAL_PATH);
			fs::path materialRootPath = matRootInfo->aPath;
			materialRootPath /= MaterialTextureFolderName;
			materialRootPath += "\\";			
				
			std::string materialPathInfo = materialRootPath.string() + m_EntityNodes[i]->MeshMaterial->GetName().C_Str();			
			Material* materialAsset = (Material*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(materialPathInfo), Material::StaticClass());

			//Diffuse
			aiColor3D color(0.f, 0.f, 0.f);
			if (aiReturn_SUCCESS == m_EntityNodes[i]->MeshMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
			{			
				SVector3 diffuseColor = SVector3(color.r, color.g, color.b);
				materialAsset->SetDiffuseColor(diffuseColor);

			}
			//Specular
			aiColor3D specular(0.f, 0.f, 0.f);
			if (aiReturn_SUCCESS == m_EntityNodes[i]->MeshMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular))
			{
				SVector3 specularColor = SVector3(specular.r, specular.g, specular.b);
				materialAsset->SetSpecularColor(specularColor);				
			}
			//Alpha
			float opacity = 0.0f;
			if (aiReturn_SUCCESS == m_EntityNodes[i]->MeshMaterial->Get(AI_MATKEY_OPACITY, opacity))
			{
				materialAsset->SetOpacity(opacity);
			}

			float shininess = 0.0f;
			if (aiReturn_SUCCESS == m_EntityNodes[i]->MeshMaterial->Get(AI_MATKEY_SHININESS, shininess))
			{
				// Phong Shininess -> Beckmann BRDF Roughness conversion
				//
				// https://simonstechblog.blogspot.com/2011/12/microfacet-brdf.html
				// https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
				//
				materialAsset->SetRoughness(::sqrtf(2.0f / (2.0f + shininess)));
			}



			
			const SPathInfo* texRoot = g_Engine->Get<PathManager>()->FindPath(TEXTURE_PATH);		
			fs::path textureRootPath = texRoot->aPath;	

			textureRootPath /= MaterialTextureFolderName;
			textureRootPath += "\\";			


			//Create Texture Asset.			
			//FBXFileName//DIFFUSE(Material_Property)//Texture.tga
			if(m_EntityNodes[i]->MeshMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) == AI_SUCCESS)
			{												
				fs::path texPath = texture_path.C_Str();
				TextureAsset* diffuseTexture = nullptr;
				const aiTexture* tex = pAiScene->GetEmbeddedTexture(texPath.string().c_str());
				if (tex)
				{
					texPath = tex->mFilename.C_Str();			

					std::string sEmbadedPath = textureRootPath.string();
					size_t texSize = std::max<size_t>(tex->mWidth, 1) * std::max<size_t>(tex->mHeight, 1);
					FTextureID texID = rdr->CreateTextureFromMemory(tex->pcData, texSize, texPath.filename().string(), sEmbadedPath);
					rdr->SaveTextureToFile(texID, sEmbadedPath);
					diffuseTexture =  (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sEmbadedPath + texPath.filename().string() + ".meta"), TextureAsset::StaticClass());
				}
				//Not Embedded Texture ->
				//Just load Texture From File..								

				materialAsset->SetDiffuseMap(diffuseTexture);
			}
			//Load Specular
			if (m_EntityNodes[i]->MeshMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texture_path) == AI_SUCCESS)
			{
				fs::path texPath = texture_path.C_Str();
				TextureAsset* specularTexture = nullptr;
				const aiTexture* tex = pAiScene->GetEmbeddedTexture(texPath.string().c_str());
				if(tex)
				{
					texPath = tex->mFilename.C_Str();

					std::string sEmbadedPath = textureRootPath.string();
					size_t texSize = std::max<size_t>(tex->mWidth, 1) * std::max<size_t>(tex->mHeight, 1);
					FTextureID texID = rdr->CreateTextureFromMemory(tex->pcData, texSize, texPath.filename().string(), sEmbadedPath);
					rdr->SaveTextureToFile(texID, sEmbadedPath);
					specularTexture = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sEmbadedPath + texPath.filename().string() + ".meta"), TextureAsset::StaticClass());					
				}


				materialAsset->SetSpecularMap(specularTexture);
			}
			//load Normals
			if (m_EntityNodes[i]->MeshMaterial->GetTexture(aiTextureType_NORMALS, 0, &texture_path) == AI_SUCCESS)
			{
				fs::path texPath = texture_path.C_Str();
				TextureAsset* normalTextrue = nullptr;
				const aiTexture* tex = pAiScene->GetEmbeddedTexture(texPath.string().c_str());
				if (tex)
				{
					texPath = tex->mFilename.C_Str();			

					std::string sEmbadedPath = textureRootPath.string();
					size_t texSize = std::max<size_t>(tex->mWidth, 1) * std::max<size_t>(tex->mHeight, 1);
					FTextureID texID = rdr->CreateTextureFromMemory(tex->pcData, texSize, texPath.filename().string(), sEmbadedPath);
					rdr->SaveTextureToFile(texID, sEmbadedPath);
					normalTextrue = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sEmbadedPath + texPath.filename().string() + ".meta"), TextureAsset::StaticClass());
				}
				/*	else
					{
						std::string FIleName = texPath.filename().string();
						normalTextrue = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(textureRootPath.string() + FIleName + ".meta"), TextureAsset::StaticClass());

					}*/


				materialAsset->SetNormalMap(normalTextrue);
			}

			if (m_EntityNodes[i]->MeshMaterial->GetTexture(aiTextureType_HEIGHT, 0, &texture_path) == AI_SUCCESS)
			{
				fs::path texPath = texture_path.C_Str();
				TextureAsset* heightMap = nullptr;
				const aiTexture* tex = pAiScene->GetEmbeddedTexture(texPath.string().c_str());
				if (tex)
				{
					texPath = tex->mFilename.C_Str();

					std::string sEmbadedPath = textureRootPath.string();
					size_t texSize = std::max<size_t>(tex->mWidth, 1) * std::max<size_t>(tex->mHeight, 1);
					FTextureID texID = rdr->CreateTextureFromMemory(tex->pcData, texSize, texPath.filename().string(), sEmbadedPath);
					rdr->SaveTextureToFile(texID, sEmbadedPath);
					heightMap = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sEmbadedPath + texPath.filename().string() + ".meta"), TextureAsset::StaticClass());
				}

				materialAsset->SetHeightMap(heightMap);
			}

			if (m_EntityNodes[i]->MeshMaterial->GetTexture(aiTextureType_METALNESS, 0, &texture_path) == AI_SUCCESS)
			{
				fs::path texPath = texture_path.C_Str();
				TextureAsset* metalicTexture = nullptr;
				const aiTexture* tex = pAiScene->GetEmbeddedTexture(texPath.string().c_str());
				if (tex)
				{
					texPath = tex->mFilename.C_Str();

					std::string sEmbadedPath = textureRootPath.string();
					size_t texSize = std::max<size_t>(tex->mWidth, 1) * std::max<size_t>(tex->mHeight, 1);
					FTextureID texID = rdr->CreateTextureFromMemory(tex->pcData, texSize, texPath.filename().string(), sEmbadedPath);
					rdr->SaveTextureToFile(texID, sEmbadedPath);
					metalicTexture = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sEmbadedPath + texPath.filename().string() + ".meta"), TextureAsset::StaticClass());
				}

				materialAsset->SetMetallicMap(metalicTexture);
			}


			if (m_EntityNodes[i]->MeshMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texture_path) == AI_SUCCESS)
			{
				fs::path texPath = texture_path.C_Str();
				TextureAsset* emissiveTexture = nullptr;
				const aiTexture* tex = pAiScene->GetEmbeddedTexture(texPath.string().c_str());
				if (tex)
				{
					texPath = tex->mFilename.C_Str();

					std::string sEmbadedPath = textureRootPath.string();
					size_t texSize = std::max<size_t>(tex->mWidth, 1) * std::max<size_t>(tex->mHeight, 1);
					FTextureID texID = rdr->CreateTextureFromMemory(tex->pcData, texSize, texPath.filename().string(), sEmbadedPath);
					rdr->SaveTextureToFile(texID, sEmbadedPath);
					emissiveTexture = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sEmbadedPath + texPath.filename().string() + ".meta"), TextureAsset::StaticClass());
				}

				materialAsset->SetEmissiveMap(emissiveTexture);
			}

			if (m_EntityNodes[i]->MeshMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texture_path) == AI_SUCCESS)
			{
				fs::path texPath = texture_path.C_Str();
				TextureAsset* roughnessMap = nullptr;
				const aiTexture* tex = pAiScene->GetEmbeddedTexture(texPath.string().c_str());
				if (tex)
				{
					texPath = tex->mFilename.C_Str();

					std::string sEmbadedPath = textureRootPath.string();
					size_t texSize = std::max<size_t>(tex->mWidth, 1) * std::max<size_t>(tex->mHeight, 1);
					FTextureID texID = rdr->CreateTextureFromMemory(tex->pcData, texSize, texPath.filename().string(), sEmbadedPath);
					rdr->SaveTextureToFile(texID, sEmbadedPath);
					roughnessMap = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sEmbadedPath + texPath.filename().string() + ".meta"), TextureAsset::StaticClass());
				}

				materialAsset->SetRoughnessMap(roughnessMap);
			}
			if(meshID != INDEX_NONE)
				model->m_MaterialLookupPerMesh.insert(std::make_pair(meshID, materialAsset));
		}		
	}

	ParseAnimations(pAiScene,convertedSkeleton);
}

void AssimpLoader::ParseNode(const aiNode* assimp_node, SAssimpNode* parent_node, SAssimpNode* new_Node, const aiScene* pAiScene)
{
	if (parent_node) // parent node is already set
	{
		new_Node->Name = assimp_node->mName.C_Str();
		new_Node->ParentTransform = STransform(parent_node->relRotQuat, parent_node->relPos, parent_node->relScale3D);
		new_Node->Root = false;
		new_Node->Parent = parent_node;
	} 
	else
	{
		new_Node->Name = assimp_node->mName.C_Str();
		new_Node->Root = true;		
	}
	set_entity_transform(assimp_node, new_Node);	
	ParseNodeMeshes(assimp_node, new_Node, pAiScene);

	for (uint32_t i = 0; i < assimp_node->mNumChildren; i++)
	{
		SAssimpNode* child = new SAssimpNode;
		m_EntityNodes.push_back(child);		
		ParseNode(assimp_node->mChildren[i], new_Node, child, pAiScene);
	}
}

void AssimpLoader::ParseNodeMeshes(const aiNode* assimp_node, SAssimpNode* new_entity,const aiScene* pAiScene)
{
	for (uint32_t i = 0; i < assimp_node->mNumMeshes; i++)
	{
		SAssimpNode* entity = new_entity; // set the current entity
		const auto assimp_mesh = pAiScene->mMeshes[assimp_node->mMeshes[i]]; // get mesh
		std::string _name = assimp_node->mName.C_Str(); // get name

		// if this node has many meshes, then assign a new entity for each one of them
		if (assimp_node->mNumMeshes > 1)
		{
			const bool is_active = false;
			entity = new SAssimpNode;
			m_EntityNodes.push_back(entity);
			entity->ParentTransform = STransform(new_entity->relRotQuat, new_entity->relPos, new_entity->relScale3D); // set parent
			_name += "_" + to_string(i + 1); // set name
		}

		// Set entity name
		entity->Name = _name;

		// Process mesh
		LoadMesh(assimp_mesh, entity, pAiScene);
	}
}

void AssimpLoader::ParseAnimations(const aiScene* pAiScene, Skeleton* ConvertedSkeleton)
{
	uint32 animSize = pAiScene->mNumAnimations;
	for(uint32 i = 0;i<animSize;i++)
	{
		const aiAnimation* assimp_animation = pAiScene->mAnimations[i];
		//Create Animation Sequence
		const SPathInfo* animPath = g_Engine->Get<PathManager>()->FindPath(ANIMATION_PATH);
		fs::path animRootPath = animPath->aPath;
		std::string name = m_FullPath.filename().replace_extension("").string();
		animRootPath /= name;

		AnimationSequence* animation = (AnimationSequence*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(animRootPath.string()),AnimationSequence::StaticClass());
			

		// Basic properties		
		//assimp_animation->mName.C_Str()
		animation->SetName(m_FullPath.filename().replace_extension("").string());
		animation->SetDuration(assimp_animation->mDuration);
		animation->SetFramePerSec(assimp_animation->mTicksPerSecond != 0.0f ? assimp_animation->mTicksPerSecond : 25.0f);		
		

		// Animation channels
		for (uint32_t j = 0; j < static_cast<uint32_t>(assimp_animation->mNumChannels); j++)
		{
			const aiNodeAnim* assimp_node_anim = assimp_animation->mChannels[j];
			SAnimationNode animation_node;
			animation->m_Channels.resize(assimp_animation->mNumChannels);

			animation_node.Name = assimp_node_anim->mNodeName.C_Str();

			// Position keys
			for (uint32_t k = 0; k < static_cast<uint32_t>(assimp_node_anim->mNumPositionKeys); k++)
			{
				const double time = assimp_node_anim->mPositionKeys[k].mTime;
				const SVector3 value = convert_vector3(assimp_node_anim->mPositionKeys[k].mValue);

				animation_node.PositionFrames.emplace_back(SKeyVector{ time, value });
			}

			// Rotation keys
			for (uint32_t k = 0; k < static_cast<uint32_t>(assimp_node_anim->mNumRotationKeys); k++)
			{
				const auto time = assimp_node_anim->mPositionKeys[k].mTime;
				const SVector4 value = convert_quaternion(assimp_node_anim->mRotationKeys[k].mValue);

				animation_node.RotationFrames.emplace_back(SKeyQuaternion{ time, value });
			}

			// Scaling keys
			for (uint32_t k = 0; k < static_cast<uint32_t>(assimp_node_anim->mNumScalingKeys); k++)
			{
				const auto time = assimp_node_anim->mPositionKeys[k].mTime;
				const SVector3 value = convert_vector3(assimp_node_anim->mScalingKeys[k].mValue);

				animation_node.ScaleFrames.emplace_back(SKeyVector{ time, value });
			}
			animation->m_Channels[j] = (animation_node);
		}				
		
		animation->MakeFrameTrans(ConvertedSkeleton,this);
	}
}

void AssimpLoader::LoadMesh(aiMesh* assimp_mesh, SAssimpNode* entity_parent, const aiScene* pAiScene)
{
	const uint32_t vertex_count = assimp_mesh->mNumVertices;
	const uint32_t index_count = assimp_mesh->mNumFaces * 3;	
			
	entity_parent->Vertices.resize(vertex_count);
	{
		for (uint32_t i = 0; i < vertex_count; i++)
		{
			SVertex& vertex = entity_parent->Vertices[i];

			// Position
			const aiVector3D& pos = assimp_mesh->mVertices[i];
			vertex.Position[0] = pos.x;
			vertex.Position[1] = pos.y;
			vertex.Position[2] = pos.z;		
	
			// Normal
			if (assimp_mesh->mNormals)
			{
				const aiVector3D& normal = assimp_mesh->mNormals[i];
				vertex.Normal[0] = normal.x;
				vertex.Normal[1] = normal.y;
				vertex.Normal[2] = normal.z;
				
			}

			// Tangent
			if (assimp_mesh->mTangents)
			{
				const aiVector3D& tangent = assimp_mesh->mTangents[i];
				vertex.Tangent[0] = tangent.x;
				vertex.Tangent[1] = tangent.y;
				vertex.Tangent[2] = tangent.z;
			}

			//Axis Change~~
			//Matrix mat = Matrix::StaticRotation(90.0f, 0, 0);
			//vertex.Position = vertex.Position.TransformCoord(mat);
			//vertex.Normal = vertex.Normal.TransformCoord(mat);
			//vertex.Tangent = vertex.Tangent.TransformCoord(mat);
			Matrix matReflect = Matrix(1.0, 0.0, 0.0, 0.0,
									   0.0, 0.0, 1.0, 0.0,
									   0.0, -1.0, 0.0, 0.0,
									   0.0, 0.0, 0.0, 1.0);
			Matrix matReflect2 = Matrix(1.0, 0.0, 0.0, 0.0,
										0.0, 0.0, -1.0, 0.0,
										0.0, 1.0, 0.0, 0.0,
										0.0, 0.0, 0.0, 1.0);
			Matrix transP = Matrix::StaticTranslation(vertex.Position);			
			Matrix transN = Matrix::StaticTranslation(vertex.Normal);
			Matrix transT = Matrix::StaticTranslation(vertex.Tangent);
			
			vertex.Position = (matReflect * transP * matReflect2).GetTranslation();
			vertex.Normal = (matReflect * transN * matReflect2).GetTranslation();
			vertex.Tangent = (matReflect * transT * matReflect2).GetTranslation();


			// Texture coordinates
			const uint32_t uv_channel = 0;
			if (assimp_mesh->HasTextureCoords(uv_channel))
			{
				const auto& tex_coords = assimp_mesh->mTextureCoords[uv_channel][i];
				vertex.UV[0] = tex_coords.x;
				vertex.UV[1] = tex_coords.y;
			}
		}
	}

	// Indices
	entity_parent->Indices.resize(index_count);	
	{
		// Get indices by iterating through each face of the mesh.
		for (uint32_t face_index = 0; face_index < assimp_mesh->mNumFaces; face_index++)
		{
			// if (aiPrimitiveType_LINE | aiPrimitiveType_POINT) && aiProcess_Triangulate) then (face.mNumIndices == 3)
			const aiFace& face = assimp_mesh->mFaces[face_index];		
			const uint32 indices_index = (face_index * 3);
			entity_parent->Indices[indices_index + 0] = face.mIndices[0];
			entity_parent->Indices[indices_index + 1] = face.mIndices[1];
			entity_parent->Indices[indices_index + 2] = face.mIndices[2];
		}
	}

	 
	// Compute AABB (before doing move operation on vertices)
	//const BoundingBox aabb = BoundingBox(vertices.data(), static_cast<uint32_t>(vertices.size()));
	

	//// Bones
	entity_parent->Bones.resize(vertex_count);
	LoadBones(assimp_mesh, entity_parent, pAiScene);

	if(pAiScene->HasMaterials())
	{
		aiMaterial* assimp_material = pAiScene->mMaterials[assimp_mesh->mMaterialIndex];
		entity_parent->MeshMaterial = assimp_material;
	}


}

void AssimpLoader::LoadBones(const aiMesh* assimp_mesh, SAssimpNode* entity_parent, const aiScene* pAiScene)
{
	// Maximum number of bones per mesh
	// Must not be higher than same const in skinning shader
	constexpr uint8_t MAX_BONES = 64;
	// Maximum number of bones per vertex
	constexpr uint8_t MAX_BONES_PER_VERTEX = 4;

	for (uint32 i = 0; i < assimp_mesh->mNumBones; i++)
	{
		uint32 BoneIndex = 0;
		std::string BoneName = assimp_mesh->mBones[i]->mName.C_Str();				
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
		{
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			SBoneInfo bi;
			m_BoneInfo.push_back(bi);			
			m_BoneInfo[BoneIndex].matOffset = Convert_Matrix(assimp_mesh->mBones[i]->mOffsetMatrix);						
			//m_BoneInfo[BoneIndex].matBone = Convert_Matrix(assimp_mesh->mBones[i]->mOffsetMatrix);
			m_BoneInfo[BoneIndex].Name = assimp_mesh->mBones[i]->mName.C_Str();						
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = m_BoneMapping[BoneName];
		}
		for (uint32 j = 0; j < assimp_mesh->mBones[i]->mNumWeights; j++)
		{
			uint32 VertexID =/* m_Entries[MeshIndex].BaseVertex +*/ assimp_mesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = assimp_mesh->mBones[i]->mWeights[j].mWeight;
			entity_parent->Bones[VertexID].Index = (float)BoneIndex;
			entity_parent->Bones[VertexID].Weights = Weight;		
			entity_parent->Bones[VertexID].vecWeigths.push_back(Weight);
			entity_parent->Bones[VertexID].vecIndex.push_back(BoneIndex);
		}
	}

}

void AssimpLoader::BoneTransform(float TimeInSeconds, std::vector<Matrix>& OutMatrix)
{
	Matrix Identity;

	float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[0]->mDuration);
	
	ReadNodeHierarchy(AnimationTime, m_pScene->mRootNode, Identity);

	OutMatrix.resize(m_NumBones);

	for (uint32 i = 0; i < m_NumBones; i++)
	{
		OutMatrix[i] = m_BoneInfo[i].matBone;
	}
}



void AssimpLoader::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	Matrix NodeTransformation = Convert_Matrix(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim)
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		Matrix ScalingM;
		Matrix::StaticScaling(convert_vector3(Scaling));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		Matrix RotationM = Matrix::StaticRotationQuaternion(convert_quaternion(RotationQ));			

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		Matrix TranslationM = Matrix::StaticTranslation(convert_vector3(Translation));		


		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM* ScalingM;
	}

	Matrix GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
	{
		uint32 BoneIndex = m_BoneMapping[NodeName];
		m_BoneInfo[BoneIndex].matBone = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].matOffset;
		
	}

	for (uint32 i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


const aiNodeAnim* AssimpLoader::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{	
	for (int32 i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
	
		if (NodeName == pNodeAnim->mNodeName.C_Str())
		{
			return pNodeAnim;
		}
	}
	return NULL;
}

void AssimpLoader::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}
	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void AssimpLoader::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}
	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void AssimpLoader::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}
	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

unsigned int AssimpLoader::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);
	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}

unsigned int AssimpLoader::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);
	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}

unsigned int AssimpLoader::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}

//std::vector<FTextureID> AssimpLoader::LoadMaterialTextures(class aiMaterial* pMaterial, aiTextureType texType, const std::string& texName, Renderer* pRenderer, const std::string& modelDirectory)
//{
//	std::vector<FTextureID> textures;
//	for (uint32 i = 0; i < pMaterial->GetTextureCount(texType); i++)
//	{
//		aiString str;
//		pMaterial->GetTexture(type, i, &str);
//		std::string path = str.C_Str();
//		textures.push_back(pRenderer->CreateTextureFromFile(path, modelDirectory, true));
//	}
//	return textures;
//}
//


