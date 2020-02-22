#pragma once
#include "CoreMinimal.h"
#include "MarchingCubes.h"
#include "DensityDataBuilder.h"
#include "OpenWorldBakery.h"
#include "OWB_DencityDataBuilder.generated.h"

UCLASS(BlueprintType)
class OWBEASYVOXEL_API UOWBDensityDataBuilder : public UObject, public IDensityDataBuilder
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "OpenWorldBakery")
	void BindToOpenWOrldBakery(UOpenWorldBakery* OpenWorldBakery);

	UFUNCTION(BlueprintCallable, Category = "OpenWorldBakery")
	void SetChunk(int ChunkX, int ChunkY);

	UFUNCTION(BlueprintCallable, Category = "OpenWorldBakery")
	void SetLayer(EOWBMeshBlockTypes MapLayer);

	UOWBDensityDataBuilder();
	~UOWBDensityDataBuilder();

	virtual FDensityPoint BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings) override;
private:
	float CellWidth = 0;
	UPROPERTY()
	UOpenWorldBakery* OWB = nullptr;
	int ChunkX_ = -1;
	int ChunkY_ = -1;
	EOWBMeshBlockTypes Layer = EOWBMeshBlockTypes::Ground;
};