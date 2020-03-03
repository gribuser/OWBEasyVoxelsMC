#pragma once
#include "CoreMinimal.h"
#include "MarchingCubes.h"
#include "VoxelDataConverter.h"
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

	virtual FDensityPoint BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings) override;
	void DoGetFDensityPoint(const FIntVector& VoxelCoordinates, FDensityPoint& DensityPoint);
private:
	UPROPERTY()
	UOpenWorldBakery* OWB = nullptr;
	int ChunkX_ = -1;
	int ChunkY_ = -1;
	EOWBMeshBlockTypes Layer = EOWBMeshBlockTypes::Ground;
};


class OWBEASYVOXEL_API FOWB_MarchingCubes : public FMarchingCubes {
public:
	FOWB_MarchingCubes(FVoxelSettings MCSettings, UOWBDensityDataBuilder* DataBuilder);
	virtual void BuildDensityPoint(const FIntVector& VoxelCoordinates, FDensityPoint& DensityPoint) const override;
private:
	UOWBDensityDataBuilder* MyDDBuider;
};

class OWBEASYVOXEL_API FOWB_VoxelDataConverter : public FVoxelDataConverter {
public:
	FOWB_VoxelDataConverter(const TArray<FVector>& InCoordinates, const TArray<int32>& InTriangles, const TMap<FIntVector, FDensityPoint>& InDensityData, const FVoxelSettings& InSettings, UOWBDensityDataBuilder* DataBuilder);
	virtual void BuildDensityPoint(const FIntVector& VoxelCoordinates, FDensityPoint& DensityPoint) const override;
private:
	UOWBDensityDataBuilder* MyDDBuider;
};