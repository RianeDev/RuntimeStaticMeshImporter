// Fill out your copyright notice in the Description page of Project Settings.
#include "RuntimeStaticMeshImporterBFL.h"
#include "RuntimeStaticMeshImporter.h"
#include "UObject/UObjectGlobals.h"
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <ProceduralMeshConversion.h>
#include "StaticMeshDescription.h"
#include "MeshDescription.h"
#include <Kismet/KismetMathLibrary.h>

void URuntimeStaticMeshImporterBFL::GetUgcDirectoriesAndFbx(FString UGCDirectory, bool bDirectPath, bool bMakeDirectoryIfNotFound, TArray<FString>& Directories, TArray<FSTMIFilePathData>& FbxPathStructs, bool& bFoundUGCDirectory)
{
	bFoundUGCDirectory = false;
	FString mainpath;

	if (bDirectPath)
	{
		mainpath = UGCDirectory;
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> Searching directory : %s"), *mainpath);
	}
	else
	{
		FString basepath = FPaths::ProjectDir();
		mainpath = basepath + "/" + UGCDirectory;
	}

	bFoundUGCDirectory = FPaths::DirectoryExists(mainpath);

	if (!bFoundUGCDirectory)
	{
		if(bMakeDirectoryIfNotFound)
		{
			UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> Creating directory : %s"), *mainpath);
			IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
			if(!FileManager.CreateDirectory(*mainpath))
			{
				UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> Failed to create directory : %s"), *mainpath);
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> Failed to find directory : %s"), *mainpath);
			return;
		}

	}

	FFileManagerGeneric::Get().FindFilesRecursive(Directories, *mainpath, TEXT("*"), false, true, true);

	if (Directories.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> Directory has no subfolders : %s"), *mainpath);
		return;
	}

	FString currentdir;

	for (int8 i = 0; i < Directories.Num(); i++) //filter through each subdirectory
	{
		if (Directories[i] != "")
		{
			currentdir = Directories[i];
			TArray<FString> fbxpaths;

			FFileManagerGeneric::Get().FindFilesRecursive(fbxpaths, *currentdir, TEXT("*.fbx"), true, false, true);

			if (!fbxpaths.IsEmpty())

			{

				FSTMIFilePathData newstruct;

				for (int8 j = 0; j < fbxpaths.Num(); j++)// filter through the fbx files in the subdirectory
				{
					if (fbxpaths[j] != "")
					{
						FString fbxpartialpath;
						FString fbxname;
						FString fbxextension;
						FString dirpartial;
						FString dirname;
						FString dirext;
						FPaths::Split(currentdir, dirpartial, dirname, dirext);
						FPaths::Split(fbxpaths[j], fbxpartialpath, fbxname, fbxextension);

						newstruct.FileName = fbxname;
						newstruct.FilePath = fbxpaths[j];
						newstruct.SubfolderName = dirname;
						newstruct.FileSubfolder = currentdir;

						FbxPathStructs.Add(newstruct);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> Subdirectory has no files : %s"), *currentdir);
			}
		}
	}

	return;
};

void GetMeshData(const aiScene* scene, aiNode* node, FSTMIObjectData& result)
{

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		std::string TestString = node->mName.C_Str();
		FString Fs = FString(TestString.c_str());
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter ->Getting Mesh Data : %s"), *Fs);
		int meshidx = *node->mMeshes;
		aiMesh* mesh = scene->mMeshes[meshidx];
		FSTMIMeshData& mi = result.MeshData[meshidx];

		//transform.
		aiMatrix4x4 tempTrans = node->mTransformation;
		FMatrix tempMatrix;
		tempMatrix.M[0][0] = tempTrans.a1; tempMatrix.M[0][1] = tempTrans.b1; tempMatrix.M[0][2] = tempTrans.c1; tempMatrix.M[0][3] = tempTrans.d1;
		tempMatrix.M[1][0] = tempTrans.a2; tempMatrix.M[1][1] = tempTrans.b2; tempMatrix.M[1][2] = tempTrans.c2; tempMatrix.M[1][3] = tempTrans.d2;
		tempMatrix.M[2][0] = tempTrans.a3; tempMatrix.M[2][1] = tempTrans.b3; tempMatrix.M[2][2] = tempTrans.c3; tempMatrix.M[2][3] = tempTrans.d3;
		tempMatrix.M[3][0] = tempTrans.a4; tempMatrix.M[3][1] = tempTrans.b4; tempMatrix.M[3][2] = tempTrans.c4; tempMatrix.M[3][3] = tempTrans.d4;
		mi.RelativeTransform = FTransform(tempMatrix);

		//vert
		for (uint32 j = 0; j < mesh->mNumVertices; ++j)
		{
			FVector vertex = FVector(
				mesh->mVertices[j].x,
				mesh->mVertices[j].y,
				mesh->mVertices[j].z);

			vertex = mi.RelativeTransform.TransformFVector4(vertex);
			mi.Vertices.Push(vertex);

			//Normal
			if (mesh->HasNormals())
			{
				FVector normal = FVector(
					mesh->mNormals[j].x,
					mesh->mNormals[j].y,
					mesh->mNormals[j].z);

				//normal = mi.RelativeTransform.TransformFVector4(normal);
				mi.Normals.Push(normal);
			}
			else
			{
				mi.Normals.Push(FVector::ZeroVector);
			}

			//UV Coordinates - inconsistent coordinates
			if (mesh->HasTextureCoords(0))
			{
				FVector2D uv = FVector2D(mesh->mTextureCoords[0][j].x, -mesh->mTextureCoords[0][j].y);
				mi.UV0.Add(uv);
			}

			//Tangent
			if (mesh->HasTangentsAndBitangents())
			{
				FProcMeshTangent meshTangent = FProcMeshTangent(
					mesh->mTangents[j].x,
					mesh->mTangents[j].y,
					mesh->mTangents[j].z
				);
				mi.Tangents.Push(meshTangent);
			}

			//Vertex color
			if (mesh->HasVertexColors(0))
			{
				FLinearColor color = FLinearColor(
					mesh->mColors[0][j].r,
					mesh->mColors[0][j].g,
					mesh->mColors[0][j].b,
					mesh->mColors[0][j].a
				);
				mi.VertexColors.Push(color);
			}

		}
	}
}


void FindMesh(const aiScene* scene, aiNode* node, FSTMIObjectData& returndata)
{
	GetMeshData(scene, node, returndata);

	for (uint32 m = 0; m < node->mNumChildren; ++m)
	{
		FindMesh(scene, node->mChildren[m], returndata);
	}
}

void URuntimeStaticMeshImporterBFL::LoadFbxFile(FString DirectPath, FSTMIFilePathData FbxPathStruct, TArray<FSTMIObjectData>& FbxData)
{
	FSTMIObjectData dresult;
	dresult.bGotMeshData = false;
	dresult.MeshData.Empty();
	dresult.NumMeshes = 0;
	dresult.TexturePaths.Empty();
	dresult.Textures.Empty();
	std::string file;

	if (FbxPathStruct.FilePath == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> LoadFbxFiles -> no filepath!"));
		return;
	}

	if (DirectPath != "")
	{
		file = TCHAR_TO_UTF8(*DirectPath);
	}
	else
	{
		file = TCHAR_TO_UTF8(*FbxPathStruct.FilePath);
	}

	Assimp::Importer FBXImporter;

	const aiScene* MeshScenePtr = FBXImporter.ReadFile(file, aiProcess_Triangulate | aiProcess_MakeLeftHanded | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes);

	if (MeshScenePtr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> LoadFbxFiles -> Failed to read mesh data!"));
		return;
	}

	if (MeshScenePtr->HasMeshes())
	{
		dresult.MeshData.SetNum(MeshScenePtr->mNumMeshes, false);

		FindMesh(MeshScenePtr, MeshScenePtr->mRootNode, dresult);

		for (uint32 i = 0; i < MeshScenePtr->mNumMeshes; ++i)
		{
			//loop through triangles
			for (uint32 l = 0; l < MeshScenePtr->mMeshes[i]->mNumFaces; ++l)
			{
				for (uint32 m = 0; m < MeshScenePtr->mMeshes[i]->mFaces[l].mNumIndices; ++m)
				{
					dresult.MeshData[i].Triangles.Push(MeshScenePtr->mMeshes[i]->mFaces[l].mIndices[m]);
				}
			}
		}

		TArray<FString> texturepaths;
		TArray<UTexture2D*> fbxtextures;
		GetFbxTextures(FbxPathStruct, texturepaths, fbxtextures);

		if (!fbxtextures.IsEmpty())
		{
			dresult.Textures.Append(fbxtextures);
			dresult.TexturePaths.Append(texturepaths);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> LoadFbxFile -> fbtextures array is empty!"));
		}

		dresult.bGotMeshData = true;
		FbxData.Add(dresult);
	}
	return;
}

void URuntimeStaticMeshImporterBFL::GetFbxTextures(FSTMIFilePathData FbxPathStruct, TArray<FString>& TexturePaths, TArray<UTexture2D*>& loadedtextures)
{
	FString basepath = FbxPathStruct.FileSubfolder;
	FString TextToSearch = FbxPathStruct.FileName;
	TArray<FString> TexturesToCheck;

	FFileManagerGeneric::Get().FindFilesRecursive(TexturesToCheck, *basepath, TEXT("*.png"), true, false, true);

	for (int8 i = 0; i < TexturesToCheck.Num(); i++)
	{
		if (TexturesToCheck[i].Contains(TextToSearch))
		{
			TexturePaths.Add(TexturesToCheck[i]);
			UTexture2D* newobject = nullptr;
			newobject = UKismetRenderingLibrary::ImportFileAsTexture2D(GEngine->GetWorld(), TexturesToCheck[i]);

			if (newobject->IsValidLowLevel())
			{ 
				loadedtextures.Add(newobject);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> GetFbxTextures -> newobject is invalid!"));
			}
		}
	}
	return;
}

void URuntimeStaticMeshImporterBFL::CreatePMCFromData(FSTMIObjectData FbxData, UProceduralMeshComponent*& OutMesh)
{
	if (FbxData.bGotMeshData)
	{
		if (!FbxData.MeshData.IsEmpty())
		{
			int32 RandomInt{};
			RandomInt = UKismetMathLibrary::RandomIntegerInRange(0, 999);
			FString intstring = FString::FromInt(RandomInt);
			FString procname = FString("RSMI_ProcMesh") + intstring;

			UProceduralMeshComponent* procmesh = NewObject<UProceduralMeshComponent>(GetTransientPackage(), FName(procname), EObjectFlags::RF_Transient);
			//UProceduralMeshComponent* procmesh = NewObject<UProceduralMeshComponent>(ObjectToAttachTo, TEXT("RSMI_ProcMesh"));

			FString nummeshdata = FString::FromInt(FbxData.MeshData.Num());

			for (int8 i = 0; i < FbxData.MeshData.Num(); i++)
			{
				FString currentindex = FString::FromInt(i);
				procmesh->CreateMeshSection_LinearColor(
					i,
					FbxData.MeshData[i].Vertices,
					FbxData.MeshData[i].Triangles,
					FbxData.MeshData[i].Normals,
					FbxData.MeshData[i].UV0,
					FbxData.MeshData[i].VertexColors,
					FbxData.MeshData[i].Tangents,
					true
				);

			}
			OutMesh = procmesh;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> CreateProceduralMeshFromData -> FbxData.MeshData is empty!"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> CreateProceduralMeshFromData -> FbxData is invalid!"));
	}
	return;
}

void URuntimeStaticMeshImporterBFL::PopulateStaticMeshFromPMC(UProceduralMeshComponent* ProceduralMesh, UStaticMeshComponent* StaticMeshComponentToPopulate)
{
	if (IsValid(ProceduralMesh) && IsValid(StaticMeshComponentToPopulate))
	{
		UProceduralMeshComponent* procmesh = ProceduralMesh;

		FName procmeshname = FName(procmesh->GetName() + "_Static");

		UStaticMesh* NewStaticMesh = NewObject<UStaticMesh>(GetTransientPackage(), procmeshname, EObjectFlags::RF_Transient);
		NewStaticMesh->bAllowCPUAccess = true;
		NewStaticMesh->NeverStream = true;
		NewStaticMesh->InitResources();
		NewStaticMesh->SetLightingGuid();

		FMeshDescription PMC_Description = BuildMeshDescription(procmesh);
		UStaticMeshDescription* SM_Description = NewStaticMesh->CreateStaticMeshDescription();
		SM_Description->SetMeshDescription(PMC_Description);
		NewStaticMesh->BuildFromStaticMeshDescriptions({ SM_Description }, false);

		// Collision
		NewStaticMesh->CalculateExtendedBounds();
		NewStaticMesh->SetBodySetup(procmesh->ProcMeshBodySetup);
		

#if WITH_EDITOR

		NewStaticMesh->PostEditChange();

#endif
		NewStaticMesh->MarkPackageDirty();

		if (IsValid(NewStaticMesh))
		{
			StaticMeshComponentToPopulate->SetStaticMesh(NewStaticMesh);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> CreateStaticMeshFromData -> New static mesh invalid!"));
		}
	}
	else if(!IsValid(ProceduralMesh))
	{
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> CreateStaticMeshFromData -> Need valid PMC reference!"));
	}
	else if (!IsValid(StaticMeshComponentToPopulate))
	{
		UE_LOG(LogTemp, Warning, TEXT("RuntimeStaticMeshImporter -> CreateStaticMeshFromData -> Need valid static mesh reference!"));
	}
	return;
}
