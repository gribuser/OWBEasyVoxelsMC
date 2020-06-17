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
enum class EOWBEVChunkStates: uint8 { OWBEV_Idle, OWBEV_Pending, OWBEV_Working };

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
	void BindToOpenWOrldBakery(UOpenWorldBakeryDebugger* OpenWorldBakery, int ChunkX, int ChunkY);


	UFUNCTION(BlueprintCallable, Category = "OpenWorldBakery")
	void InitTerrainBuild();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	EOWBEVChunkStates State = EOWBEVChunkStates::OWBEV_Idle;

	virtual void Tick(float DeltaTime) override;
	UOWB_EV_WorldVisializer* WorldVisualizer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	EOWBMeshBlockTypes LayerToDraw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	UMaterialInstanceDynamic* DebugMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	UMaterialInstanceDynamic* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	int ChunkX_ = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenWorldBakery")
	int ChunkY_ = -1;

	const FOWBMeshBlocks_set* ChunkDescr;
	bool FullScaleDebugTexture = false;

private:
	UPROPERTY()
	UOWBDensityDataBuilder* DensityBuilder = nullptr;

	UPROPERTY()
	UOpenWorldBakeryDebugger* OWB = nullptr;

	void EndTerrainBuild(const FMeshData& AMeshData);

	TSharedPtr<FVoxelDataConverter, ESPMode::ThreadSafe> WorkerMesh = nullptr;
	TSharedPtr<FMarchingCubes, ESPMode::ThreadSafe> WorkerCubes = nullptr;

	FVoxelSettings MCSettings;
	int CurLayer = 0;
};
