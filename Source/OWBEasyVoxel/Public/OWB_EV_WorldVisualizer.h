#pragma once
#include "CoreMinimal.h"
#include "MarchingCubes.h"
#include "OWB_DencityDataBuilder.h"
#include "OpenWorldBakery.h"
#include "utility/OpenWorldBakeryHeightmapDebugMapping.h"
#include "OWB_EV_WorldVisualizer.generated.h"


UCLASS()
class OWBEASYVOXEL_API AWorldVisializer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldVisializer();
	//~AWorldVisializer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration", meta = (ClampMin = "16"))
	int MapResolution = 128;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration", meta = (ClampMin = "1"))
	int CutChunks = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	float CoverNoiseScale = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	float CoverNoiseStrength = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	TArray<FOWBMapDisplacement> MapDisplacements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	TArray<FOWBRandHillSetup> RandomHills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	int TerrainSeed = 2;

	UPROPERTY(BlueprintReadOnly, Category = "LandscapeGeneration")
	UOpenWorldBakery* Bakery;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FCriticalSection MeshGenLock;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	float ChunkSize = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	UMaterial* ColoredMeshMaterialTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	UMaterialInstance* WaterMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "LandscapeGeneration")
	FDebugTextureParams DebugTextureParams;
private:
//	FVoxelSettings Voxel;
	UPROPERTY()
	UOWBDensityDataBuilder* GroundDensityBuilder = nullptr;
	UPROPERTY()
	UOWBDensityDataBuilder* WaterDensityBuilder = nullptr;
};

