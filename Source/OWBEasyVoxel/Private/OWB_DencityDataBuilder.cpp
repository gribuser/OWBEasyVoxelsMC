#include "OWB_DencityDataBuilder.h"
#include "MarchingCubes.h"
#include "MarchingCubesTables.h"
#include "DensityDataBuilder.h"
#include "EasyVoxelsMCLibrary.h"
#include "HeightMapTerrain/HeightMapTerrain.h"

void UMyDensityDataBuilder::InitWithHeighMap(TArray<float>* AHeightMap, int AWidth, int AHeight, float ACellWidth)
{
	HeightMap = AHeightMap;
	Width = AWidth;
	Height = AHeight;
	CellWidth = ACellWidth;
}
UMyDensityDataBuilder::UMyDensityDataBuilder() {

}

UMyDensityDataBuilder::~UMyDensityDataBuilder()
{
	//delete[] HeightMap;
}

FDensityPoint UMyDensityDataBuilder::BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings)
{
	FDensityPoint ThisPointData(0, FLinearColor(0,0,0,0));
	int Z = VoxelCoordinates.Z;
	int X = VoxelCoordinates.X + ChunkSlot.X * Settings.ChunkRadius.X;
	int Y = VoxelCoordinates.Y + ChunkSlot.Y * Settings.ChunkRadius.Y;

	//if (ChunkSlot.X == 0)
	//	UE_LOG(LogTemp, Log, TEXT("GetCellIn %i:%i:%i from chunk %i:%i:%i. Final coords %i:%i:%i"),
	//		VoxelCoordinates.X, VoxelCoordinates.Y, VoxelCoordinates.Z,
	//		ChunkSlot.X, ChunkSlot.Y, ChunkSlot.Z,X,Y,Z);

	if (Z <= 1 || X <= 1 || Y <= 1 || X >= Width-1 || Y >= Height-1) {
		return ThisPointData;
	}

	Z -= 3;
	X--;
	Y--;

	int CellAddr = X + Y * Width;
	if (CellAddr >= HeightMap->Num()) {
		return ThisPointData;
	}

	float ThisCellHeight = (*HeightMap)[CellAddr] * CellWidth * Width;
	if (isnan(ThisCellHeight)) {
		ThisCellHeight = 200000;
	}

	float BtmLVL = Z * CellWidth;
	ThisPointData.Value = ThisCellHeight - BtmLVL;

	return ThisPointData;
}

//FMyMarchingCubes::FMyMarchingCubes(const TMap<FIntVector, FDensityPoint>& InDensityData, UObject* InDensityInterfaceObject, const FVoxelSettings& InSettings, const FIntVector& InChunkSlot)
//	: ModifiedDensityData(InDensityData)
//	, ChunkSlot(InChunkSlot)
//	, Settings(InSettings)
//	, DensityInterfaceObject(InDensityInterfaceObject)
//{
//	super::
//	ensureMsgf((Settings.Units.X >= 2), TEXT("X Unit Size must be higher or equal with two and a power of two."));
//	ensureMsgf((Settings.Units.Y >= 2), TEXT("Y Unit Size must be higher or equal with two and a power of two."));
//	ensureMsgf((Settings.Units.Z >= 2), TEXT("Z Unit Size must be higher or equal with two and a power of two."));
//	ensureMsgf(Settings.Resolution > 0.f, TEXT("Voxel Resolution must be higher than 0."));
//}