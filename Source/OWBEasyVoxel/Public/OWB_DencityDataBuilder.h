// Open World Bakery free additions
// Copyright 2020 Dmitry Grivov, g@primitive.game, all rights reserved

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

	virtual void BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings, FDensityPoint& DensityPoint) override;
	FDensityPoint* DoGetFDensityPoint(const FIntVector& VoxelCoordinates);
private:
	UPROPERTY()
	UOpenWorldBakery* OWB = nullptr;
	int ChunkX_ = -1;
	int ChunkY_ = -1;
	EOWBMeshBlockTypes Layer = EOWBMeshBlockTypes::Ground;
};


class OWBEASYVOXEL_API FOWB_MarchingCubes : public FMarchingCubes {
public:
	FOWB_MarchingCubes(TSharedRef<FEasyVoxelsMCWorker, ESPMode::ThreadSafe> InWorker, FVoxelSettings MCSettings, UOWBDensityDataBuilder* DataBuilder);
	virtual FDensityPoint* GetDensityPoint(const FIntVector& VoxelCoordinates) override;
private:
	UOWBDensityDataBuilder* MyDDBuider;
};

class OWBEASYVOXEL_API FOWB_VoxelDataConverter : public FVoxelDataConverter {
public:
	FOWB_VoxelDataConverter(const FVoxelSettings& InSettings, TSharedRef<FEasyVoxelsMCWorker, ESPMode::ThreadSafe> InWorker, UOWBDensityDataBuilder* DataBuilder);
	virtual FDensityPoint* GetDensityPoint(const FIntVector& VoxelCoordinates) override; 
private:
	UOWBDensityDataBuilder* MyDDBuider;
};