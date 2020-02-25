#pragma once
#include "CoreMinimal.h"
#include "MarchingCubes.h"
#include "OWB_EV_ChunkVisualizer.h"
#include "OpenWorldBakery.h"
#include "utility/OpenWorldBakeryHeightmapDebugMapping.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration", meta = (ClampMin = "16"))
	int MapResolution = 128;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration", meta = (ClampMin = "0.1"))
	float VoxelSize = 5;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	UOpenWorldBakery* OpenWorldBakery;

	UPROPERTY(EditDefaultsOnly, Category = "LandscapeGeneration")
	TSubclassOf<AActor> OceanPlaneBP;

	FCriticalSection MeshGeneratorLock;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FCriticalSection MeshGenLock;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	float ChunkSize = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	UMaterial* ColoredMeshMaterialTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeGeneration")
	UMaterialInstance* WaterMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "LandscapeGeneration")
	FDebugTextureParams DebugTextureParams;
	
	UFUNCTION(BlueprintCallable, Category = "LandscapeGeneration")
	void RemoveVisualization();

	UFUNCTION(BlueprintCallable, Category = "LandscapeGeneration")
	void CreateVisualization();

private:
//	FVoxelSettings Voxel;
	UPROPERTY()
	TArray<AOWB_EV_Chunk*> ChunksVisualizers;
	TArray<EOWBMeshBlockTypes> LayersToDraw;

};

