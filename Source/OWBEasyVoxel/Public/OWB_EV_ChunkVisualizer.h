// Open World Baker
// Copyright 2019 Dmitry Grivov, grib@gribuser.ru, all rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
//#include "MarchingCubes.h"
//#include "Materials/MaterialInstanceDynamic.h"
//#include "EasyVoxelsMCLibrary.h"
//#include "VoxelLib/VoxelDataConverter.h"
//#include "HeightMapTerrain/UnrealRandomHeightMap.h"
//#include "RuntimeMeshComponent.h"
//#include "RuntimeMeshActor.h"
//#include "APIs/EasyVoxelMC_DencityDataBuilder.h"
#include "OWB_EV_ChunkVisualizer.generated.h"


UCLASS()
class OWBEASYVOXEL_API AOWB_EV_Chunk : public AActor //ARuntimeMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOWB_EV_Chunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	UProceduralMeshComponent* NewPMC;
//
//	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
//	//URuntimeMeshComponent* StaticProvider = nullptr;
//
//	FIntVector Chunk;
//	FVoxelSettings MCSettings;
//	
//	UPROPERTY()
//	UHeightmapDensityDataBuilder* DensityBuilder = nullptr;
//
//	UPROPERTY()
//	URandomHeightMap* RHM = nullptr;
//	bool CookBitmaps = true;
//	
//	TSharedPtr<FVoxelDataConverter, ESPMode::ThreadSafe> WorkerMesh = nullptr;
//	TSharedPtr<FMarchingCubes, ESPMode::ThreadSafe> WorkerCubes = nullptr;
//	FCriticalSection* SharedMeshGenLock;
//	FDebugTextureParams DebugTextureParams;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
//	UMaterialInstanceDynamic* DynamicMaterial = nullptr;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
//	UMaterialInstance* StaticMaterial = nullptr;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
//	bool CleanupMesh = false;
//
//	ATerrainVoxel* Next;
//	int CutChunks;
//
//	virtual void Tick(float DeltaTime) override;
//	void InitTerrainBuild();
//	void RefreshDebugBitmap();
//private:
//	void EndTerrainBuild(const FMeshData& AMeshData);
};
