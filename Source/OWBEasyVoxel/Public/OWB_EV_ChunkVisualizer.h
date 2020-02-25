// Open World Baker
// Copyright 2019 Dmitry Grivov, grib@gribuser.ru, all rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "OpenWorldBakery.h"
#include "OWB_DencityDataBuilder.h"
#include "EasyVoxelsMCLibrary.h"
#include "VoxelLib/VoxelDataConverter.h"

//#include "MarchingCubes.h"
//#include "Materials/MaterialInstanceDynamic.h"
//#include "HeightMapTerrain/UnrealRandomHeightMap.h"
//#include "RuntimeMeshComponent.h"
//#include "RuntimeMeshActor.h"
//#include "APIs/EasyVoxelMC_DencityDataBuilder.h"

#include "OWB_EV_ChunkVisualizer.generated.h"
class UOWB_EV_WorldVisializer;

UENUM(BlueprintType, Category = "Open World Bakery|Enums")
enum EOWBEVChunkStates { OWBEV_Idle, OWBEV_Pending, OWBEV_Working };

UCLASS()
class OWBEASYVOXEL_API AOWB_EV_Chunk : public AActor //ARuntimeMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOWB_EV_Chunk();
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	UProceduralMeshComponent* ProceduralMesh;

	UFUNCTION(BlueprintCallable, Category = "OpenWorldBakery")
	void BindToOpenWOrldBakery(UOpenWorldBakery* OpenWorldBakery, int ChunkX, int ChunkY);


	UFUNCTION(BlueprintCallable, Category = "OpenWorldBakery")
	void InitTerrainBuild();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	TEnumAsByte<EOWBEVChunkStates> State = EOWBEVChunkStates::OWBEV_Idle;

	virtual void Tick(float DeltaTime) override;
	UOWB_EV_WorldVisializer* WorldVisualizer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	EOWBMeshBlockTypes LayerToDraw;

//	FOWBMeshBlocks_set* MyChunkDescr;

private:
	UPROPERTY()
	UOWBDensityDataBuilder* DensityBuilder = nullptr;

	UPROPERTY()
	UOpenWorldBakery* OWB = nullptr;

	int ChunkX_ = -1;
	int ChunkY_ = -1;

	void EndTerrainBuild(const FMeshData& AMeshData);

	TSharedPtr<FVoxelDataConverter, ESPMode::ThreadSafe> WorkerMesh = nullptr;
	TSharedPtr<FMarchingCubes, ESPMode::ThreadSafe> WorkerCubes = nullptr;

	FVoxelSettings MCSettings;
	int CurLayer = 0;
	
	void DoBuildTerrainLayer();
	void PlaceOcean();
	void DrawFInished();



	//
//	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
//	//URuntimeMeshComponent* StaticProvider = nullptr;
//
//	FIntVector Chunk;
//	FVoxelSettings MCSettings;
//	

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

//	void InitTerrainBuild();
//	void RefreshDebugBitmap();
//private:
//	void EndTerrainBuild(const FMeshData& AMeshData);
};
