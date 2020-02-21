
#include "OWB_DencityDataBuilder.h"
//#include "MarchingCubes.h"
//#include "MarchingCubesTables.h"
//#include "DensityDataBuilder.h"
//#include "EasyVoxelsMCLibrary.h"

void UOWBDensityDataBuilder::BindToOpenWOrldBakery(UOpenWorldBakery* OpenWorldBakery) {
	OWB = OpenWorldBakery;
}
void UOWBDensityDataBuilder::SetChunk(int ChunkX, int ChunkY) {
	if (ensureMsgf(OWB != nullptr, TEXT("Open world bakery not set, unsafe chunks setup - can not check - can not check validity")))
		if (ensureMsgf(OWB->ChunksSetup.XChunks > 0, TEXT("Open world bakery haven't executed CookChunks() yet, unsafe chunks setup - can not check validity")))
			ensureMsgf(OWB->ChunksSetup.XChunks >= ChunkX && OWB->ChunksSetup.YChunks >= ChunkY, TEXT("You set wrong chunk %i:%i. Only %i:%i available"), ChunkX, ChunkY, OWB->ChunksSetup.XChunks, OWB->ChunksSetup.YChunks);
	if (ensureMsgf(ChunkX >= 0 && ChunkY >= 0, TEXT("Invalid chunks adress"))) {
		ChunkX_ = ChunkX;
		ChunkY_ = ChunkY;
	}
}
UOWBDensityDataBuilder::UOWBDensityDataBuilder() {}

UOWBDensityDataBuilder::~UOWBDensityDataBuilder() {}

FDensityPoint UOWBDensityDataBuilder::BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings)
{
	FDensityPoint ThisPointData(0, FLinearColor(0, 0, 0, 0));
	if (ensureMsgf( OWB != nullptr,TEXT("Open world bakery not set, call BindToOpenWOrldBakery first"))
		&& ensureMsgf(OWB->MapWidth > 0, TEXT("Open world bakery has no world cooked (have you called CalcHillsLayaut() already?)"))) {
		int Z = VoxelCoordinates.Z;
		int X = VoxelCoordinates.X; // +ChunkSlot.X * Settings.ChunkRadius.X;
		int Y = VoxelCoordinates.Y; // +ChunkSlot.Y * Settings.ChunkRadius.Y;

		if (ChunkX_ >= 0) {
			X += ChunkX_ * OWB->ChunksSetup.ChunkWidth;
			Y += ChunkY_ * OWB->ChunksSetup.ChunkHeight;
		}

		//if (ChunkSlot.X == 0)
		//	UE_LOG(LogTemp, Log, TEXT("GetCellIn %i:%i:%i from chunk %i:%i:%i. Final coords %i:%i:%i"),
		//		VoxelCoordinates.X, VoxelCoordinates.Y, VoxelCoordinates.Z,
		//		ChunkSlot.X, ChunkSlot.Y, ChunkSlot.Z,X,Y,Z);

		if (Z <= 1 || X <= 1 || Y <= 1 || X >= OWB->MapWidth - 1 || Y >= OWB->MapHeight - 1) {
			return ThisPointData;
		}

		Z -= 3;
		X--;
		Y--;

		OpenWorldBakery::FSquareMeter& Ground = OWB->Ground(X, Y);

		float ThisCellHeight = Ground.GroundElevation * OWB->CellWidth;
		if (isnan(ThisCellHeight)) {
			ThisCellHeight = 200000;
		}

		float BtmLVL = Z * CellWidth;
		ThisPointData.Value = ThisCellHeight - BtmLVL;

	}
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