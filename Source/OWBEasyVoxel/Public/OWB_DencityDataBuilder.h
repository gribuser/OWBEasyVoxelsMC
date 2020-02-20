#pragma once
#include "CoreMinimal.h"
#include "MarchingCubes.h"
#include "DensityDataBuilder.h"
#include "OWB_DencityDataBuilder.generated.h"

UCLASS()
class UMyDensityDataBuilder : public UObject, public IDensityDataBuilder
{
	GENERATED_BODY()
public:
	TArray<float>* HeightMap;
	int Width = 0;
	int Height = 0;
	void InitWithHeighMap(TArray<float>* AHeightMap, int AWidth, int AHeight, float ACellWidth);
	UMyDensityDataBuilder();
	~UMyDensityDataBuilder();
//	FDensityPoint BuildDensityPoint(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings);
	virtual FDensityPoint BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings) override;
private:
	float CellWidth = 0;
};

//class TERRAINGENERATION01_API FMyMarchingCubes : public FMarchingCubes {
//	FMyMarchingCubes(const TMap<FIntVector, FDensityPoint>& InDensityData, UObject* InDensityInterfaceObject, const FVoxelSettings& InSettings, const FIntVector& InChunkSlot);
//};
