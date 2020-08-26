// Open World Bakery free additions
// Copyright 2020 Dmitry Grivov, g@primitive.game, all rights reserved

#pragma once
#include "CoreMinimal.h"
#include "MarchingCubes.h"
#include "OWB_EV_ChunkVisualizer.h"
#include "OpenWorldBakery.h"
#include "Enchanters/RandomHeightMap.h"
#include "TexturedBakery.h"
#include "Components/SceneComponent.h"
#include "OWB_EV_WorldVisualizer.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OWBEASYVOXEL_API UOWB_EV_WorldVisializer : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UOWB_EV_WorldVisializer();
	//~UOWB_EV_WorldVisializer();

	/// Physical size of the voxel in the world, generally should be somewhere in 50-150 range
	/// for the walkable terrain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bakery Landscape", meta = (ClampMin = "0.1"))
	float VoxelSize = 50;

	/// You can set things up here, you can interact with it.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bakery Landscape")
	UOpenWorldBakeryTextured* OpenWorldBakery;

	/// Not used internally, just a handy variable to store res
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helper variables", meta = (ClampMin = "16"))
	int MapResolution = 256;
	
	/// Not used internally, just a handy variable to store seed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helper variables")
	int TerrainSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bakery Landscape")
	TMap<EOWBMeshBlockTypes, UMaterialInterface*> TypedMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Bakery Landscape")
	TSubclassOf<AActor> OceanPlaneBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bakery Landscape")
	TSubclassOf<AActor> OceandeepPlaneBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bakery Landscape")
	TSubclassOf<AActor> DebugChunkVisualBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bakery Landscape")
	FVector LandscapeShift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bakery Landscape")
	FDebugTextureParams DebugTextureParams;

	FCriticalSection MeshGeneratorLock;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bakery Landscape")
	TArray<FIntPoint> DebugVoxels = { {-1,-1} };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bakery Landscape")
	UMaterialInterface* DebugMaterialTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bakery Landscape")
	FIntPoint DebugTrapFrom = { -1,-1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bakery Landscape")
	FIntPoint DebugTrapTo = { -1,-1 };

	UFUNCTION(BlueprintCallable, Category = "Bakery Landscape")
	void RemoveVisualization();

	UFUNCTION(BlueprintCallable, Category = "Bakery Landscape")
	void CreateVisualization();

private:
//	FVoxelSettings Voxel;
	UPROPERTY()
	TArray<AOWB_EV_Chunk*> ChunksVisualizers;


	UPROPERTY()
	TArray<AActor*> ChunksAdditionalActors;

	TArray<EOWBMeshBlockTypes> LayersToDraw = {
		EOWBMeshBlockTypes::Ground,
		EOWBMeshBlockTypes::FreshWater
	};

	void PlaceOcean(int X, int Y, bool Water);
	void DrawChunkBox(const FOWBMeshBlocks_set_contents& LayerChunk);
	bool DebugBitmapForThis(int x, int y);

};

