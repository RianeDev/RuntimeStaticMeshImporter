// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Misc/Paths.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/FileHelper.h"
#include "RuntimeStaticMeshImporterBFL.generated.h"


USTRUCT(BlueprintType) struct FSTMIMeshData
{
	GENERATED_BODY()

public:

	FSTMIMeshData()
	{
	
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<FVector> Vertices;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<int32> Triangles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<FVector> Normals;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<FVector2D> UV0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<FLinearColor> VertexColors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<FProcMeshTangent> Tangents;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		FTransform RelativeTransform;
};

USTRUCT(BlueprintType) struct FSTMIObjectData
{
	GENERATED_BODY()

public:

	FSTMIObjectData()
	{

	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		bool bGotMeshData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		int32 NumMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<FSTMIMeshData> MeshData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<FString> TexturePaths;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		TArray<UTexture2D*> Textures;
};

USTRUCT(BlueprintType) struct FSTMIFilePathData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		FString FileName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		FString FilePath;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		FString SubfolderName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Runtime Static Mesh Importer|Data")
		FString FileSubfolder;
};

UCLASS()
class RUNTIMESTATICMESHIMPORTER_API URuntimeStaticMeshImporterBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//Get all user content directories. If DirectPath is checked it will search the full path, if not it will search from a specified folder in your project's directory. Also gets any fbx data paths to easily connect with LoadFbxFile function.
	UFUNCTION(BlueprintCallable, Category = "Runtime Static Mesh Importer")
		static void GetUgcDirectoriesAndFbx(FString UGCDirectory, bool bDirectPath, bool bMakeDirectoryIfNotFound, TArray<FString>& Directories, TArray<FSTMIFilePathData>& FbxPathStructs, bool& bFoundUGCDirectory);

	// Loads fbx data for populating procedural mesh component sections. Will check for DirectPath first, if empty will check for FBX Path Struct. The FbxData array contains ALL meshes found in the fbx file.
	UFUNCTION(BlueprintCallable, Category = "Runtime Static Mesh importer")
		static void LoadFbxFile(FString DirectPath, FSTMIFilePathData FbxPathStruct, TArray<FSTMIObjectData>& FbxData);

	//Automatically called by LoadFbxFile, but this can be used by itself as well to find the textures for a specific FBX Data struct.
	UFUNCTION(BlueprintCallable, Category = "Runtime Static Mesh importer")
		static void GetFbxTextures(FSTMIFilePathData FbxPathStruct, TArray<FString>& TexturePaths, TArray<UTexture2D*>& loadedtextures);

	//Creates a transient ProceduralMeshComponent. This is a temporary object that isn't saved by the game, so it's important to either populate a static mesh using the data (PopulateStaticMeshFromPMC) or set your own PMC with it! If you just want to use it to setup a static mesh, then it will simply be garbage collected and you needn't worry about removing it manually.
	UFUNCTION(BlueprintCallable, Category = "Runtime Static Mesh importer")
		static void CreatePMCFromData(FSTMIObjectData FbxData, UProceduralMeshComponent*& OutMesh);
	//Populates a specified static mesh component using a procedural mesh component. 
	UFUNCTION(BlueprintCallable, Category = "Runtime Static Mesh importer")
		static void PopulateStaticMeshFromPMC(UProceduralMeshComponent* ProceduralMesh, UStaticMeshComponent* StaticMeshComponentToPopulate);
private:

};
