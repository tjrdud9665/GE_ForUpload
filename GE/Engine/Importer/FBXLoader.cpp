#include "EnginePCH.h"
#include "FBXLoader.h"

#include "../Assets/Material/Material.h"
#include "../Assets/Mesh/Skeleton.h"
#include "../Assets/Mesh/SkeletalMeshAsset.h"
#include "../Assets/Mesh/Mesh.h"
#include "../Assets/Animation/AnimationSequence.h"
#include "FileSystem/FileSystem.h"
#include "Renderer/Renderer.h"
#include "AssetManager.h"
#include "Path/PathManager.h"
#include "../Renderer/GeometryGenerator.h"


Matrix ConvertFBXMatrix(FbxAMatrix& matrix)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat[i][j] = (float)matrix.mData[i].mData[j];
		}
	}

	return mat;

}


FBXLoader::FBXLoader()
	:m_Manager(nullptr)
	,m_Scene(nullptr)	
	, m_ReqAnimOnly(false)
{

}

FBXLoader::~FBXLoader()
{
	size_t	Size = m_vecMaterial.size();

	for (size_t i = 0; i < Size; ++i)
	{
		size_t	Size1 = m_vecMaterial[i].size();

		for (size_t j = 0; j < Size1; ++j)
		{
			SAFE_DELETE(m_vecMaterial[i][j]);
		}
	}

	Size = m_vecMeshContainer.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecMeshContainer[i]);
	}

	Size = m_vecBone.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecBone[i]);
	}

	for (int i = 0; i < m_NameArray.Size(); ++i)
	{
		m_NameArray[i]->Clear();
		SAFE_DELETE(m_NameArray[i]);
	}

	Size = m_vecClip.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecClip[i]);
	}

	if (m_Scene)
	{
		m_Scene->Clear();
		m_Scene->Destroy();
	}

	if (m_Manager)
		m_Manager->Destroy();
}

bool FBXLoader::LoadFBX(const char* FullPath, MeshAsset*& OutMeshAsset)
{
	if (IsConverted(FullPath))
	{
		return true;
	}
	
	m_Manager = FbxManager::Create();

	FbxIOSettings* IOS = FbxIOSettings::Create(m_Manager, IOSROOT);

	m_Manager->SetIOSettings(IOS);

	m_Scene = FbxScene::Create(m_Manager, "");

	FbxImporter* Importer = FbxImporter::Create(m_Manager, "");

	Importer->Initialize(FullPath, -1, m_Manager->GetIOSettings());

	//FBM 폴더 생성
	Importer->Import(m_Scene);

	if (m_Scene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::Max)
		m_Scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

	bool HasAnimation = false;

	m_Scene->FillAnimStackNameArray(m_NameArray);

	if (m_NameArray.GetCount() > 0)
	{
		// 본 정보를 읽어온다.
		LoadBone(m_Scene->GetRootNode());
		if(!m_vecBone.empty())
		{
			LoadAnimationClip();
			HasAnimation = true;
			// 클립이 가지고 있는 프레임을 본 개수만큼 resize 해준다.
			// 원래 Animation Clip 에서 하던것이다.
			for (size_t i = 0; i < m_vecClip.size(); ++i)
			{
				m_vecClip[i]->vecBoneKeyFrame.resize(m_vecBone.size());
			}
		}
	
	}

	if(!HasAnimation && m_ReqAnimOnly)
	{
		Importer->Destroy();
		return true;
	}

	Triangulate(m_Scene->GetRootNode());

	LoadMesh(m_Scene->GetRootNode(), !HasAnimation);


	//Create Mesh Or SkeleltalMesh Asset
	Converting(OutMeshAsset);



	Importer->Destroy();



	return true;
}


void FBXLoader::Converting(MeshAsset*& OutMeshAsset)
{
	Renderer* rdr = g_Engine->Get<Renderer>();

	bool createMeshAsset = true;
	Skeleton* convertedSkeleton = nullptr;
	XClass* createMeshType = MeshAsset::StaticClass();
	if (m_vecBone.size() > 0)
	{
		createMeshType = SkeletalMeshAsset::StaticClass();
	} else
	{
		createMeshType = MeshAsset::StaticClass();
	}

	size_t ContainerSIze = m_vecMeshContainer.size();

	if(ContainerSIze == 0)
	{
		assert(false);
	}
	std::string modelRootName = m_vecMeshContainer[0]->ModelName;
	if(modelRootName.empty())
	{
		modelRootName = m_FullPath.filename().string();		
	}
	if (!m_ReqAnimOnly)
	{
		AssetData* found = g_Engine->Get<AssetManager>()->FindAssetFromName(modelRootName, MeshAsset::StaticClass());

		if (found)
		{
			createMeshAsset = false;
			OutMeshAsset = (MeshAsset*)found;
		} else if (createMeshAsset)
		{			
			fs::path meshPath = m_FullPath;
			meshPath._Remove_filename_and_separator();
			meshPath /= modelRootName;
			OutMeshAsset = (MeshAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(meshPath.string()), createMeshType);
			OutMeshAsset->SetFromFile(m_FullPath.filename().string());
		}		

		if (createMeshAsset)
		{		
			for (size_t i = 0; i < ContainerSIze; i++)
			{
				int32 meshID = INDEX_NONE;
				bool CollisionMesh = false;
				std::string meshPartName = m_vecMeshContainer[i]->ModelName;
				if (meshPartName.empty())
				{
					std::stringstream ss;
					ss << modelRootName << "_" << i;
					meshPartName = ss.str();
				}
				
				if(meshPartName.find("UCX") == 0)
				{
					CollisionMesh = true;
				}
				if(CollisionMesh)
				{					

					for (size_t c = 0; c < m_vecMeshContainer[i]->vecIndex.size(); c++)
					{
						OutMeshAsset->AddCollisionMeshData(m_vecMeshContainer[i]->vecPos, m_vecMeshContainer[i]->vecIndex[c]);
					}

				}
				else
				{
					Mesh convertedMesh = Mesh(meshPartName);

					std::vector<SVertex> vertices;
					size_t posSize = m_vecMeshContainer[i]->vecPos.size();
					size_t nrmSize = m_vecMeshContainer[i]->vecNormal.size();
					size_t uvSize = m_vecMeshContainer[i]->vecUV.size();
					size_t tanSize = m_vecMeshContainer[i]->vecTangent.size();

					size_t blendIdxSize = m_vecMeshContainer[i]->vecBlendIndex.size();
					size_t blendWeightSize = m_vecMeshContainer[i]->vecBlendWeight.size();

					vertices.resize(posSize);
					for (size_t j = 0; j < posSize; j++)
					{

						vertices[j].Position = m_vecMeshContainer[i]->vecPos[j];
						vertices[j].Normal = m_vecMeshContainer[i]->vecNormal[j];
						vertices[j].Tangent = m_vecMeshContainer[i]->vecTangent[j];
						vertices[j].UV = m_vecMeshContainer[i]->vecUV[j];

						if (m_vecBone.size() != 0)
						{
							vertices[j].BlendWeight = m_vecMeshContainer[i]->vecBlendWeight[j];
							vertices[j].BlendIndex = m_vecMeshContainer[i]->vecBlendIndex[j];
						}

					}

					for (size_t c = 0; c < m_vecMeshContainer[i]->vecIndex.size(); c++)
					{
						//if (c >= 1)
						//{

						//	FBufferID createdVertexID = convertedMesh.m_IABuffer[c - 1].first;
						//	convertedMesh.Initialize(createdVertexID, m_vecMeshContainer[i]->vecIndex[c]);

						//} else
						{
							convertedMesh.Initialize<SVertex>(vertices, m_vecMeshContainer[i]->vecIndex[c]);
							convertedMesh.m_BBMax = m_vecMeshContainer[i]->bbMax;
							convertedMesh.m_BBMin = m_vecMeshContainer[i]->bbMin;
						}
					}



					meshID = OutMeshAsset->AddMesh(convertedMesh);


					//Create Material.
					size_t slotidx = i;
					if (m_vecMaterial.size() <= i)
						slotidx = m_vecMaterial.size() - 1;

					size_t mtrlSize = m_vecMaterial[slotidx].size();
					//보통 서브셋 사이즈만큼

					for (size_t j = 0; j < mtrlSize; j++)
					{
						fs::path modelPath = OutMeshAsset->GetInfo().GetSPath();

						std::string MaterialTextureFolderName = modelPath.filename().replace_extension("").replace_extension("").string();
						modelPath._Remove_filename_and_separator();

						//Create MaterialAsset
						//MaterialPath/FBX'sFileName/mesh's.meta
						const SPathInfo* matRootInfo = g_Engine->Get<PathManager>()->FindPath(MATERIAL_PATH);
						fs::path materialRootPath = matRootInfo->aPath;
						//materialRootPath /= MaterialTextureFolderName;
						materialRootPath += "\\";
						FbxMaterial*& mat = m_vecMaterial[slotidx][j];

						std::string materialPathInfo = materialRootPath.string() + mat->MaterialName;
						Material* materialAsset = (Material*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(materialPathInfo), Material::StaticClass());

						materialAsset->SetDiffuseColor(mat->BaseColor);
						materialAsset->SetSpecularColor(mat->SpecularColor);
						materialAsset->SetEmissiveColor(mat->EmissiveColor);

						const SPathInfo* resRootInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
						fs::path resRootPath = resRootInfo->aPath;

						std::string sDiffusePath = resRootPath.append(mat->DiffuseTexture).string();
						std::string sNormalTexPath = resRootPath.append(mat->BumpTexture).string();
						std::string sSpecPath = resRootPath.append(mat->SpecularTexture).string();


						if (!mat->DiffuseTexture.empty() && FileSystem::FileExists(sDiffusePath))
						{

							TextureAsset* diffuseMap = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(sDiffusePath), TextureAsset::StaticClass());
							materialAsset->SetDiffuseMap(diffuseMap);
						}

						if (!mat->BumpTexture.empty() && FileSystem::FileExists(sNormalTexPath))
						{
							TextureAsset* normalMap = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(mat->BumpTexture), TextureAsset::StaticClass());
							materialAsset->SetNormalMap(normalMap);
						}

						if (!mat->SpecularTexture.empty() && FileSystem::FileExists(sSpecPath))
						{
							TextureAsset* specularMap = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(mat->SpecularTexture), TextureAsset::StaticClass());
							materialAsset->SetSpecularMap(specularMap);
						}


						/*
							std::string	DiffuseTexture;
							std::string	BumpTexture;
							std::string	SpecularTexture;
						*/
						OutMeshAsset->m_MaterialLookupPerMesh.insert(std::make_pair((int32)j, materialAsset));
					}
				}

			}
		}	


		if (createMeshType->IsA(SkeletalMeshAsset::StaticClass()))
		{
			fs::path modelPath = OutMeshAsset->GetInfo().GetSPath();
			std::string SkeletonFolderName = modelPath.filename().replace_extension("").replace_extension("").string();
			modelPath._Remove_filename_and_separator();

			const SPathInfo* skelRootInfo = g_Engine->Get<PathManager>()->FindPath(SKELETON_PATH);
			fs::path skelRootPath = skelRootInfo->aPath;
			skelRootPath /= SkeletonFolderName;
			//Create Skeleton 
			Skeleton* skeleton = (Skeleton*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(skelRootPath.string()), Skeleton::StaticClass());
			if (skeleton)
			{
				if (!skeleton->m_BoneInfos.size())
				{
					size_t BoneCnt = m_vecBone.size();
					skeleton->m_BoneInfos.resize(BoneCnt);

					for (size_t b = 0; b < BoneCnt; b++)
					{
						skeleton->m_BoneInfos[b].Name = m_vecBone[b]->Name;
						skeleton->m_BoneInfos[b].ParentIdx = m_vecBone[b]->ParentIndex;

						skeleton->m_BoneInfos[b].matBone = ConvertFBXMatrix(m_vecBone[b]->matBone);
						skeleton->m_BoneInfos[b].matOffset = ConvertFBXMatrix(m_vecBone[b]->matOffset);
					}

					skeleton->SetModelMeshName(modelRootName);
					convertedSkeleton = skeleton;
				} else
				{
					//AlreadyCraeted
				}

				SkeletalMeshAsset* skeletalModel = (SkeletalMeshAsset*)OutMeshAsset;
				convertedSkeleton = skeletalModel->m_Skeleton = skeleton;

			}

		}
	}

	//std::string modelRootName = m_vecMeshContainer[0]->ModelName;
	//AssetData* found = g_Engine->Get<AssetManager>()->FindAssetFromName(modelRootName, MeshAsset::StaticClass());

	
	size_t ClipSize =m_vecClip.size();
	for(size_t i=0;i<ClipSize;i++)
	{
		
		if (m_ReqAnimOnly)
		{
			std::string modelRootName = m_vecMeshContainer[0]->ModelName;
			const SPathInfo* skelRootInfo = g_Engine->Get<PathManager>()->FindPath(SKELETON_PATH);
			fs::path skelRootPath = skelRootInfo->aPath;
			skelRootPath /= modelRootName;
			//Create Skeleton 
			Skeleton* skeleton = (Skeleton*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(skelRootPath.string()), Skeleton::StaticClass());
			if (skeleton)
			{
				convertedSkeleton = skeleton;
			}

		}




		//Create am Sequence
		const SPathInfo* animPath = g_Engine->Get<PathManager>()->FindPath(ANIMATION_PATH);
		fs::path animRootPath = animPath->aPath;
		std::string name = m_FullPath.filename().replace_extension("").string();		
		std::stringstream ss;
		ss << name;
		animRootPath /= ss.str();
		AnimationSequence* animation = (AnimationSequence*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(animRootPath.string()), AnimationSequence::StaticClass());
		
		std::vector<SBoneKeyFrame*> boneKeyFrame;

		animation->SetFromFile(m_FullPath.filename().string());
		
		switch (m_vecClip[i]->TimeMode)
		{
		case FbxTime::eFrames24:
			animation->m_FramePerSec = 24;
			break;
		case FbxTime::eFrames30:
			animation->m_FramePerSec = 30;
			break;
		case FbxTime::eFrames60:
			animation->m_FramePerSec = 60;
			break;
		}

		animation->m_StartFrame = (int32)m_vecClip[i]->StartTime.GetFrameCount(m_vecClip[i]->TimeMode);
		animation->m_EndFrame = (int32)m_vecClip[i]->EndTime.GetFrameCount(m_vecClip[i]->TimeMode);
		animation->m_FrameLength = (int32)(animation->m_EndFrame - animation->m_StartFrame );

		animation->m_FrameTime = animation->m_PlayTime / animation->m_FramePerSec;
		animation->m_Skeleton = convertedSkeleton;
		
		//std::vector<SAnimFrameTrans>	vecFrameTrans;
		animation->m_FrameTrans.resize(m_vecClip[i]->vecBoneKeyFrame.size()* animation->m_FrameLength);

		for (size_t j = 0; j < m_vecClip[i]->vecBoneKeyFrame.size(); ++j)
		{
			SBoneKeyFrame* pBoneKeyFrame = new SBoneKeyFrame;

			pBoneKeyFrame->BoneIndex = m_vecClip[i]->vecBoneKeyFrame[j].BoneIndex;

			animation->m_vecKeyFrame.push_back(pBoneKeyFrame);

			// 아래부터 키프레임 정보를 저장한다.
			pBoneKeyFrame->vecKeyFrame.reserve(m_vecClip[i]->vecBoneKeyFrame[j].vecKeyFrame.size());

			for (size_t q = 0; q < m_vecClip[i]->vecBoneKeyFrame[j].vecKeyFrame.size(); ++q)
			{
				SKeyFrame* pKeyFrame = new SKeyFrame;

				pKeyFrame->Time = q * animation->m_FrameTime;

				// 현재 본의 키 프레임에 해당하는 행렬 정보를 얻어온다.
				FbxAMatrix		mat = m_vecClip[i]->vecBoneKeyFrame[j].vecKeyFrame[q].matTransform;

				FbxVector4		Pos, Scale;
				FbxQuaternion	Rot;

				// 행렬로부터 위치, 크기, 회전 정보를 얻어온다.
				Pos = mat.GetT();
				Scale = mat.GetS();
				Rot = mat.GetQ();

				pKeyFrame->Scale = SVector3((float)Scale.mData[0], (float)Scale.mData[1],
										   (float)Scale.mData[2]);
				pKeyFrame->Pos = SVector3((float)Pos.mData[0], (float)Pos.mData[1],
										 (float)Pos.mData[2]);
				pKeyFrame->Rot = SVector4((float)Rot.mData[0], (float)Rot.mData[1],
										 (float)Rot.mData[2], (float)Rot.mData[3]);

				pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

				if (q < animation->m_FrameLength)
				{
					
					SAnimFrameTrans	FrameTr = {};
					FrameTr.Scale = SVector4(pKeyFrame->Scale.x, pKeyFrame->Scale.y,
											pKeyFrame->Scale.z, 1.f);
					FrameTr.Translate = SVector4(pKeyFrame->Pos.x, pKeyFrame->Pos.y,
												pKeyFrame->Pos.z, 1.f);
					FrameTr.Rot = pKeyFrame->Rot;

					animation->m_FrameTrans[j * animation->m_FrameLength + q] = FrameTr;
				}
			}			
		}		
	}


}

bool FBXLoader::IsConverted(const char* FullPath)
{
	m_FullPath = FullPath;

	fs::path assetMetaPath = FullPath;
	//SK_Character_Samurai_Warrior_01
	AssetData* converted = g_Engine->Get<AssetManager>()->FindAssetFromFileName(assetMetaPath.filename().string());
				
	if (converted)
	{	
		//Is StaticMesh?
		if(converted->GetClass()->IsA(MeshAsset::StaticClass()))
		{
			return true;
		}
		//converted 가 에니메이션인경우 에니메이션의 스켈레톤과 메쉬를 convert했는지 여부에따라 추가적인 작업의 여부를 결정
		AnimationSequence* convertedAnim = (AnimationSequence*)g_Engine->Get<AssetManager>()->FindAssetFromFileNameAndAssetType(assetMetaPath.filename().string(),AnimationSequence::StaticClass());

		if(convertedAnim)
		{
			Skeleton* skel = convertedAnim->GetSkeleton();
			if(!skel)
			{
				return false;
			}						
			AssetData* meshAsset = g_Engine->Get<AssetManager>()->FindAssetFromName(skel->GetModelMeshName(),SkeletalMeshAsset::StaticClass());
			if(!meshAsset)
			{
				return false;
			}
			m_ReqAnimOnly = false;
		}
		else
		{
			//해당 fbx가 Animation을 가지고있는지
			m_ReqAnimOnly = true;
			return false;
			
		}

		return true;
	}	

	return false;
}

void FBXLoader::Triangulate(FbxNode* Node)
{
	// FbxNodeAttribute : FbxNode의 속성을 나타낸다.
	FbxNodeAttribute* Attr = Node->GetNodeAttribute();

	if (Attr && (Attr->GetAttributeType() == FbxNodeAttribute::eMesh ||
				 Attr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
				 Attr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter	Converter(m_Manager);

		Converter.Triangulate(Attr, true);
	}

	// 재질 정보를 읽는다.
	int32	MtrlCount = Node->GetMaterialCount();

	if (MtrlCount > 0)
	{
		std::vector<FbxMaterial*>	vecMtrl;

		m_vecMaterial.push_back(vecMtrl);

		for (int32 i = 0; i < MtrlCount; ++i)
		{
			FbxSurfaceMaterial* Mtrl = Node->GetMaterial(i);
			
			if (!Mtrl)
				continue;
			
			LoadMaterial(Mtrl, Mtrl->GetName());
		}
	}

	int32	ChildCount = Node->GetChildCount();

	for (int32 i = 0; i < ChildCount; ++i)
	{
		Triangulate(Node->GetChild(i));
	}
}

void FBXLoader::LoadMaterial(FbxSurfaceMaterial* Mtrl,const char* MtrlName)
{
	// 재질 정보를 저장할 구조체를 생성한다.
	FbxMaterial* MtrlInfo = new FbxMaterial;

	MtrlInfo->MaterialName = MtrlName;

	// 가장 마지막 컨테이너에 재질 정보를 추가한다.
	m_vecMaterial[m_vecMaterial.size() - 1].push_back(MtrlInfo);

	// Diffuse 정보를 읽어온다.
	MtrlInfo->BaseColor = GetMaterialColor(Mtrl, FbxSurfaceMaterial::sDiffuse,FbxSurfaceMaterial::sDiffuseFactor);

	MtrlInfo->AmbientColor = GetMaterialColor(Mtrl, FbxSurfaceMaterial::sAmbient,FbxSurfaceMaterial::sAmbientFactor);

	MtrlInfo->SpecularColor = GetMaterialColor(Mtrl, FbxSurfaceMaterial::sSpecular,FbxSurfaceMaterial::sSpecularFactor);

	MtrlInfo->EmissiveColor = GetMaterialColor(Mtrl, FbxSurfaceMaterial::sEmissive,FbxSurfaceMaterial::sEmissiveFactor);

	MtrlInfo->SpecularPower = (float)GetMaterialFactor(Mtrl, FbxSurfaceMaterial::sSpecularFactor);
	MtrlInfo->Shininess = (float)GetMaterialFactor(Mtrl, FbxSurfaceMaterial::sShininess);
	MtrlInfo->TransparencyFactor = (float)GetMaterialFactor(Mtrl, FbxSurfaceMaterial::sTransparencyFactor);
	

	MtrlInfo->DiffuseTexture = GetMaterialTexture(Mtrl, FbxSurfaceMaterial::sDiffuse);
	MtrlInfo->BumpTexture = GetMaterialTexture(Mtrl, FbxSurfaceMaterial::sNormalMap);

	if (MtrlInfo->BumpTexture.empty())
		MtrlInfo->BumpTexture = GetMaterialTexture(Mtrl, FbxSurfaceMaterial::sBump);

	MtrlInfo->SpecularTexture = GetMaterialTexture(Mtrl, FbxSurfaceMaterial::sSpecular);
}

SVector4 FBXLoader::GetMaterialColor(FbxSurfaceMaterial* Mtrl, const char* PropertyName, const char* PropertyFactorName)
{
	FbxDouble3	Result(0, 0, 0);
	double		Factor = 0;

	// 인자로 들어온 재질에서 해당 이름을 가진 프로퍼티가 있는지를 판단한다음
	// 얻어온다. 마찬가지로 해당 이름의 프로퍼티 팩터가 있는지를 판단한다음
	// 얻어온다.
	FbxProperty	Property = Mtrl->FindProperty(PropertyName);
	FbxProperty	PropertyFactor = Mtrl->FindProperty(PropertyFactorName);

	// 유효한지 체크한다.
	if (Property.IsValid() && PropertyFactor.IsValid())
	{
		Result = Property.Get<FbxDouble3>();
		Factor = PropertyFactor.Get<FbxDouble>();

		// Factor가 1이 아닐 경우 일종의 비율값으로 사용하여
		// 얻어온 vResult 에 모두 곱해준다.
		if (Factor != 1)
		{
			Result[0] *= Factor;
			Result[1] *= Factor;
			Result[2] *= Factor;
		}
	}

	return SVector4((float)Result[0], (float)Result[1], (float)Result[2], (float)Factor);
}

double FBXLoader::GetMaterialFactor(FbxSurfaceMaterial* Mtrl, const char* PropertyName)
{
	FbxProperty	Property = Mtrl->FindProperty(PropertyName);
	double	Factor = 0.0;

	if (Property.IsValid())
		Factor = Property.Get<FbxDouble>();

	return Factor;
}

std::string FBXLoader::GetMaterialTexture(FbxSurfaceMaterial* Mtrl, const char* PropertyName)
{
	FbxProperty	Property = Mtrl->FindProperty(PropertyName);

	PathManager* pathMgr = g_Engine->Get<PathManager>();	
	fs::path resRootPath = pathMgr->FindPath(RESOURCE_ROOT_PATH)->aPath;
	std::string	str = "";

	if (Property.IsValid())
	{
		// FbxFileTexture 이 타입이 몇개가 있는지를 얻어오는 함수이다.
		int	TexCount = Property.GetSrcObjectCount<FbxFileTexture>();

		if (TexCount > 0)
		{
			FbxFileTexture* FileTex = Property.GetSrcObject<FbxFileTexture>(0);

			if (FileTex)
			{
						
				std::string absPath = FileTex->GetFileName();

				size_t pos = absPath.find("Resources");

				if (pos != std::string::npos)
					absPath = absPath.substr(pos+10);
			

				str = absPath;


			}
		}
	}

	return str;
}

void FBXLoader::LoadMesh(FbxNode* Node, bool Static)
{
	FbxNodeAttribute* Attr = Node->GetNodeAttribute();

	if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* Mesh = Node->GetMesh();
		
		if (Mesh)
			LoadMesh(Mesh, Static, Attr->GetName());		
	}

	int32	ChildCount = Node->GetChildCount();

	for (int32 i = 0; i < ChildCount; ++i)
	{
		LoadMesh(Node->GetChild(i), Static);
	}

}

void FBXLoader::LoadMesh(FbxMesh* Mesh, bool Static,const char* InName)
{
	// MeshContainer를 만들고 컨테이너별 정점과 컨테이너&서브셋 별
	// 인덱스 정보를 얻어와서 메쉬를 구성해야 한다.
	FbxMeshContainer* Container = new FbxMeshContainer;
	Mesh->ComputeBBox();

	FbxDouble3 bMin = Mesh->BBoxMin.EvaluateValue();
	FbxDouble3 bMax = Mesh->BBoxMax.EvaluateValue();

	Container->bbMin = SVector3((float)bMin.mData[0], (float)bMin.mData[1], (float)bMin.mData[2]);
	Container->bbMax = SVector3((float)bMax.mData[0], (float)bMax.mData[1], (float)bMax.mData[2]);

	

	m_vecMeshContainer.push_back(Container);
	Container->ModelName = InName;

	// ControlPoint 는 위치정보를 담고 있는 배열이다.
	// 이 배열의 개수는 곧 정점의 개수가 된다.
	int32	VtxCount = Mesh->GetControlPointsCount();

	int32 test = Mesh->GetPolygonVertexCount();

	FbxArray<FbxVector2> uvs;
	if(test > VtxCount)
	{
		Mesh->SplitPoints();
		VtxCount = Mesh->GetControlPointsCount();

		FbxStringList uvSet;
		Mesh->GetUVSetNames(OUT uvSet);
		int32 Count = uvSet.GetCount();
		if (Count != 0)
		{			
			Mesh->GetPolygonVertexUVs(uvSet[0], OUT uvs);
		}


	}

	
	
	// 내부적으로 FbxVector4타입의 배열로 저장하고 있기 때문에 배열의 
	// 시작주소를 얻어온다.
	FbxVector4* VtxPos = Mesh->GetControlPoints();

	// 컨테이너가 가지고 있는 정점 정보들을 정점수만큼 resize 해준다.
	Container->vecPos.resize(VtxCount);
	Container->vecNormal.resize(VtxCount);
	Container->vecUV.resize(VtxCount);
	Container->vecTangent.resize(VtxCount);
	Container->vecBinormal.resize(VtxCount);

	for (int32 i = 0; i < VtxCount; ++i)
	{
		// y와 z축이 바뀌어 있기 때문에 변경해준다.
		Container->vecPos[i].x = (float)VtxPos[i].mData[0];
		Container->vecPos[i].y = (float)VtxPos[i].mData[2];
		Container->vecPos[i].z = (float)VtxPos[i].mData[1];
	}



	// 폴리곤 수를 얻어온다.
	int32	PolygonCount = Mesh->GetPolygonCount();	

	UINT	VtxID = 0;

	// 재질 수를 얻어온다.(재질 수 == 서브셋 수)
	int32	MtrlCount = Mesh->GetNode()->GetMaterialCount();

	MtrlCount = MtrlCount == 0 ? 1 : MtrlCount;

	// 재질 수는 곧 서브셋 수이기 때문에 재질 수만큼 resize 한다.
	Container->vecIndex.resize(MtrlCount);

	// 재질 정보를 얻어온다.
	FbxGeometryElementMaterial* Material = Mesh->GetElementMaterial();
	int32 iCount = Mesh->GetElementMaterialCount();
	// 삼각형 수만큼 반복한다.
	for (int32 i = 0; i < PolygonCount; ++i)
	{
		// 이 폴리곤을 구성하는 정점의 수를 얻어온다.
		// 삼각형일 경우 3개를 얻어온다.
		int32	PolygonSize = Mesh->GetPolygonSize(i);

		int32	Idx[3] = {};

		for (int32 j = 0; j < PolygonSize; ++j)
		{
			// 현재 삼각형을 구성하고 있는 버텍스정보 내에서의 인덱스를
			// 구한다.
			int32	ControlIndex = Mesh->GetPolygonVertex(i, j);

			Idx[j] = ControlIndex;

			LoadNormal(Mesh, Container, VtxID, ControlIndex);

			LoadUV(Mesh, Container, Mesh->GetTextureUVIndex(i, j),ControlIndex, VtxID,uvs);

			LoadTangent(Mesh, Container, VtxID, ControlIndex);

			LoadBinormal(Mesh, Container, VtxID, ControlIndex);

			++VtxID;
		}

		// 삼각형이 소속된 서브셋 인덱스를 얻어온다.
		int	MtrlID = 0;

		if (Material)
			MtrlID = Material->GetIndexArray().GetAt(i);

		Container->vecIndex[MtrlID].push_back(Idx[0]);
		Container->vecIndex[MtrlID].push_back(Idx[2]);
		Container->vecIndex[MtrlID].push_back(Idx[1]);
	}

	

	if (!Static)
		LoadAnimation(Mesh, Container);
}

void FBXLoader::LoadNormal(FbxMesh* Mesh, FbxMeshContainer* Container, int32 VtxID, int32 ControlIndex)
{
	FbxGeometryElementNormal* Normal = Mesh->GetElementNormal();

	int32	NormalIndex = VtxID;

	// MappingMode와 ReferenceMode에 따라서 인덱스로 사용할 정보가 달라진다.
	if (Normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (Normal->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			NormalIndex = Normal->GetIndexArray().GetAt(VtxID);
			break;
		}
	}

	else if (Normal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (Normal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			NormalIndex = ControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			NormalIndex = Normal->GetIndexArray().GetAt(ControlIndex);
			break;
		}
	}

	FbxVector4	vNormal = Normal->GetDirectArray().GetAt(NormalIndex);

	Container->vecNormal[ControlIndex].x = (float)vNormal.mData[0];
	Container->vecNormal[ControlIndex].y = (float)vNormal.mData[2];
	Container->vecNormal[ControlIndex].z = (float)vNormal.mData[1];
}

void FBXLoader::LoadUV(FbxMesh* Mesh, FbxMeshContainer* pContainer, int32 UVID, int32 ControlIndex, int32 VtxID, const FbxArray<FbxVector2>& uvs)
{
	int32 Count = Mesh->GetElementUVCount();
	FbxGeometryElementUV* UV = Mesh->GetElementUV(0);

	


	/*FbxStringList uvSet;
	Mesh->GetUVSetNames(OUT uvSet);
	FbxArray<FbxVector2> uvs;	
	Mesh->GetPolygonVertexUVs(uvSet[Count - 1], uvs);*/


	if (!UV)
		return;

	int32	UVIndex = UVID;

	if (UV->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (UV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			UVIndex = ControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			UVIndex = UV->GetIndexArray().GetAt(ControlIndex);
			break;
		}
	}
	else if (UV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (UV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:			
			break;
		case FbxGeometryElement::eIndexToDirect:			
		{
			
			UVIndex = UV->GetIndexArray().GetAt(VtxID);
			break;
		}
		}
	}
	
	

	FbxVector2	vUV = UV->GetDirectArray().GetAt(UVIndex);
	//if (uvs.Size() != 0)
	//	vUV = uvs.GetAt(VtxID);
		//UV->GetDirectArray().GetAt(VtxID);

	pContainer->vecUV[ControlIndex].x = (float)vUV.mData[0] - (int32)vUV.mData[0];
	pContainer->vecUV[ControlIndex].y = 1.f - (float)(vUV.mData[1] - (int32)vUV.mData[1]);
}

void FBXLoader::LoadTangent(FbxMesh* Mesh, FbxMeshContainer* Container, int32 VtxID, int32 ControlIndex)
{
	// 메쉬로부터 ElementTangent 정보를 얻어온다.
	FbxGeometryElementTangent* Tangent = Mesh->GetElementTangent();
	
	if (!Tangent)
	{		
		Mesh->GenerateTangentsDataForAllUVSets();
		Tangent = Mesh->GetElementTangent();
		if(!Tangent)
			return;
	}
	
	Container->Bump = true;

	int32	TangentIndex = VtxID;

	// MappingMode와 ReferenceMode에 따라서 인덱스로 사용할 정보가 달라진다.
	if (Tangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (Tangent->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			TangentIndex = Tangent->GetIndexArray().GetAt(VtxID);
			break;
		}
	}

	else if (Tangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (Tangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			TangentIndex = ControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			TangentIndex = Tangent->GetIndexArray().GetAt(ControlIndex);
			break;
		}
	}

	FbxVector4	vTangent = Tangent->GetDirectArray().GetAt(TangentIndex);

	Container->vecTangent[ControlIndex].x = (float)vTangent.mData[0];
	Container->vecTangent[ControlIndex].y = (float)vTangent.mData[2];
	Container->vecTangent[ControlIndex].z = (float)vTangent.mData[1];
	
}


void FBXLoader::LoadBinormal(FbxMesh* Mesh, FbxMeshContainer* Container, int32 VtxID, int32 ControlIndex)
{
	// 메쉬로부터 ElementBinormal 정보를 얻어온다.
	FbxGeometryElementBinormal* Binormal = Mesh->GetElementBinormal();

	if (!Binormal)
		return;

	Container->Bump = true;

	int	BinormalIndex = VtxID;

	// MappingMode와 ReferenceMode에 따라서 인덱스로 사용할 정보가 달라진다.
	if (Binormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (Binormal->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			BinormalIndex = Binormal->GetIndexArray().GetAt(VtxID);
			break;
		}
	}

	else if (Binormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (Binormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			BinormalIndex = ControlIndex;
			break;
		case FbxGeometryElement::eIndexToDirect:
			BinormalIndex = Binormal->GetIndexArray().GetAt(ControlIndex);
			break;
		}
	}

	FbxVector4	vBinormal = Binormal->GetDirectArray().GetAt(BinormalIndex);

	Container->vecBinormal[ControlIndex].x = (float)vBinormal.mData[0];
	Container->vecBinormal[ControlIndex].y = (float)vBinormal.mData[2];
	Container->vecBinormal[ControlIndex].z = (float)vBinormal.mData[1];
}

void FBXLoader::LoadAnimationClip()
{
	int32	Count = m_NameArray.GetCount();

	FbxTime::EMode	TimeMode = m_Scene->GetGlobalSettings().GetTimeMode();

	for (int32 i = 0; i < Count; ++i)
	{
		// m_NameArr에 저장된 이름으로 Scene으로부터 FbxAnimStack 객체를 얻어온다.
		FbxAnimStack* AnimStack = m_Scene->FindMember<FbxAnimStack>(m_NameArray[i]->Buffer());

		if (!AnimStack)
			continue;

		FbxAnimationClip* Clip = new FbxAnimationClip;

		Clip->Name = AnimStack->GetName();

		FbxTakeInfo* Take = m_Scene->GetTakeInfo(Clip->Name.c_str());

		Clip->StartTime = Take->mLocalTimeSpan.GetStart();
		Clip->EndTime = Take->mLocalTimeSpan.GetStop();
		// GetFrameCount 함수를 호출하고  time모드를 넣어주면 시간을 프레임으로
		// 변환해준다. 몇프레임 짜리 애니메이션 인지를 구해준다.
		Clip->TimeLength = Clip->EndTime.GetFrameCount(TimeMode) -
			Clip->StartTime.GetFrameCount(TimeMode);
		Clip->TimeMode = TimeMode;

		m_vecClip.push_back(Clip);
	}
}

void FBXLoader::LoadBone(FbxNode* Node)
{
	int32 ChildCount = Node->GetChildCount();

	for (int32 i = 0; i < ChildCount; ++i)
	{
		LoadBoneRecursive(Node->GetChild(i), 0, 0, -1);
	}
}

void FBXLoader::LoadBoneRecursive(FbxNode* Node, int32 Depth, int32 Index, int32 Parent)
{
	FbxNodeAttribute* Attr = Node->GetNodeAttribute();

	if (Attr && Attr->GetAttributeType() ==
		FbxNodeAttribute::eSkeleton)
	{
		FbxBone* Bone = new FbxBone;

		Bone->Name = Node->GetName();
		Bone->Depth = Depth;
		Bone->ParentIndex = Parent;
		if (Parent == 0 && m_vecBone.size() == 0)
			Bone->ParentIndex = -1;
		
			
		m_vecBone.push_back(Bone);
	}

	int32	ChildCount = Node->GetChildCount();

	for (int32 i = 0; i < ChildCount; ++i)
	{
		LoadBoneRecursive(Node->GetChild(i), Depth + 1,
						  (int32)m_vecBone.size(), Index);
	}
}

void FBXLoader::LoadAnimation(FbxMesh* Mesh, FbxMeshContainer* Container)
{
	int32	SkinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);

	if (SkinCount <= 0)
		return;

	// 메쉬의 정점 수를 얻어온다.
	int32	CPCount = Mesh->GetControlPointsCount();

	// 정점의 가중치 정보와 본인덱스 정보는 정점 수만큼
	// 만들어져야 한다.
	Container->vecBlendWeight.resize(CPCount);
	Container->vecBlendIndex.resize(CPCount);

	Container->Animation = true;

	FbxAMatrix	matTransform = GetTransform(Mesh->GetNode());

	for (int32 i = 0; i < SkinCount; ++i)
	{
		FbxSkin* Skin = (FbxSkin*)Mesh->GetDeformer(i, FbxDeformer::eSkin);

		if (!Skin)
			continue;

		FbxSkin::EType	SkinningType = Skin->GetSkinningType();

		if (SkinningType == FbxSkin::eRigid ||
			SkinningType == FbxSkin::eLinear ||
			SkinningType == FbxSkin::eBlend)
		{
			// Cluster : 관절을 의미한다.
			int32	ClusterCount = Skin->GetClusterCount();

			for (int32 j = 0; j < ClusterCount; ++j)
			{
				FbxCluster* Cluster = Skin->GetCluster(j);

				if (!Cluster->GetLink())
					continue;

				std::string	BoneName = Cluster->GetLink()->GetName();
				

				int	BoneIndex = FindBoneFromName(BoneName);

				LoadWeightAndIndex(Cluster, BoneIndex, Container);

				LoadOffsetMatrix(Cluster, matTransform, BoneIndex, Container);

				m_vecBone[BoneIndex]->matBone = matTransform;

				LoadTimeTransform(Mesh->GetNode(), Cluster,
								  matTransform, BoneIndex);
			}
		}
	}

	ChangeWeightAndIndices(Container);
}

FbxAMatrix FBXLoader::GetTransform(FbxNode* Node)
{
	const FbxVector4	vT = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4	vR = Node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4	vS = Node->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}

int32 FBXLoader::FindBoneFromName(const std::string& Name)
{
	for (size_t i = 0; i < m_vecBone.size(); ++i)
	{
		if (m_vecBone[i]->Name == Name)
			return (int32)i;
	}

	return -1;
}

void FBXLoader::LoadWeightAndIndex(FbxCluster* Cluster, int BoneIndex, FbxMeshContainer* Container)
{
	int32	ControlIndicesCount = Cluster->GetControlPointIndicesCount();

	for (int32 i = 0; i < ControlIndicesCount; ++i)
	{
		FbxWeight	Weight = {};

		Weight.Index = BoneIndex;
		Weight.Weight = Cluster->GetControlPointWeights()[i];

		int32	ClusterIndex = Cluster->GetControlPointIndices()[i];

		// map의 특징 : 키를 이용해 인덱스처럼 접근할 경우 해당 키가 없다면
		// 만들어준다.
		Container->mapWeights[ClusterIndex].push_back(Weight);
	}
}

void FBXLoader::LoadOffsetMatrix(FbxCluster* Cluster, const FbxAMatrix& matTransform, int BoneIndex, FbxMeshContainer* Container)
{
	FbxAMatrix	matCluster;
	FbxAMatrix	matClusterLink;

	Cluster->GetTransformMatrix(matCluster);
	Cluster->GetTransformLinkMatrix(matClusterLink);

	FbxVector4	v1 = { 1.0, 0.0, 0.0, 0.0 };
	FbxVector4	v2 = { 0.0, 0.0, 1.0, 0.0 };
	FbxVector4	v3 = { 0.0, 1.0, 0.0, 0.0 };
	FbxVector4	v4 = { 0.0, 0.0, 0.0, 1.0 };

	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	/*
	1 0 0 0   1 2 3 4   1 0 0 0
	0 0 1 0   5 6 7 8   0 0 1 0
	0 1 0 0 * 9 0 1 2 * 0 1 0 0
	0 0 0 1   3 4 5 6   0 0 0 1

	1 2 3 4   1 0 0 0
	9 0 1 2   0 0 1 0
	5 6 7 8 * 0 1 0 0
	3 4 5 6   0 0 0 1

	1 3 2 4
	9 1 0 2
	5 7 6 8
	3 5 4 6
	*/

	FbxAMatrix	matOffset;
	matOffset = matClusterLink.Inverse() * matCluster * matTransform;
	matOffset = matReflect * matOffset * matReflect;

	m_vecBone[BoneIndex]->matOffset = matOffset;
}

void FBXLoader::LoadTimeTransform(FbxNode* Node, FbxCluster* Cluster, const FbxAMatrix& matTransform, int BoneIndex)
{
	FbxVector4	v1 = { 1.0, 0.0, 0.0, 0.0 };
	FbxVector4	v2 = { 0.0, 0.0, 1.0, 0.0 };
	FbxVector4	v3 = { 0.0, 1.0, 0.0, 0.0 };
	FbxVector4	v4 = { 0.0, 0.0, 0.0, 1.0 };

	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;


	for (size_t i = 0; i < m_vecClip.size(); ++i)
	{
		FbxLongLong	Start = m_vecClip[i]->StartTime.GetFrameCount(m_vecClip[i]->TimeMode);
		FbxLongLong	End = m_vecClip[i]->EndTime.GetFrameCount(m_vecClip[i]->TimeMode);

		m_vecClip[i]->vecBoneKeyFrame[BoneIndex].BoneIndex = BoneIndex;

		// 전체 프레임 수만큼 반복한다.
		for (FbxLongLong j = Start; j <= End; ++j)
		{
			FbxTime	Time = {};

			// 현재 프레임에 해당하는 FbxTime을 만들어낸다.
			Time.SetFrame(j, m_vecClip[i]->TimeMode);

			// EvaluateGlobalTransform
			FbxAMatrix	matOffset = Node->EvaluateGlobalTransform(Time) * matTransform;
			FbxAMatrix	matCur = matOffset.Inverse() * Cluster->GetLink()->EvaluateGlobalTransform(Time);					


			matCur = matReflect * matCur * matReflect;

			FbxKeyFrame	KeyFrame = {};

			KeyFrame.Time = Time.GetSecondDouble();
			KeyFrame.matTransform = matCur;

			m_vecClip[i]->vecBoneKeyFrame[BoneIndex].vecKeyFrame.push_back(KeyFrame);
		}
	}
}

void FBXLoader::ChangeWeightAndIndices(FbxMeshContainer* Container)
{
	std::unordered_map<int, std::vector<FbxWeight>>::iterator	iter;
	std::unordered_map<int, std::vector<FbxWeight>>::iterator	iterEnd = Container->mapWeights.end();

	for (iter = Container->mapWeights.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second.size() > 4)
		{
			// 가중치 값에 따라 내림차순 정렬한다.
			sort(iter->second.begin(), iter->second.end(), [](const FbxWeight& lhs, const FbxWeight& rhs)
			{
				return lhs.Weight > rhs.Weight;
			});

			double	Sum = 0.0;

			for (int i = 0; i < 4; ++i)
			{
				Sum += iter->second[i].Weight;
			}

			double	Interpolate = 1.f - Sum;

			std::vector<FbxWeight>::iterator	iterErase = iter->second.begin() + 4;

			iter->second.erase(iterErase, iter->second.end());
			iter->second[0].Weight += Interpolate;
		}

		SVector4	vWeight;
		SVector4	vIndex;

		for (int32 i = 0; i < iter->second.size(); ++i)
		{
			vWeight[i] = (float)iter->second[i].Weight;
			vIndex[i] = (float)iter->second[i].Index;
		}

		Container->vecBlendWeight[iter->first] = vWeight;
		Container->vecBlendIndex[iter->first] = vIndex;
	}
}
