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
		if (ensureMsgf(OWB->ChunksLayaut.XChunks > 0, TEXT("Open world bakery haven't executed CookChunks() yet, unsafe chunks setup - can not check validity")))
			ensureMsgf(OWB->ChunksLayaut.XChunks >= ChunkX && OWB->ChunksLayaut.YChunks >= ChunkY, TEXT("You set wrong chunk %i:%i. Only %i:%i available"), ChunkX, ChunkY, OWB->ChunksLayaut.XChunks, OWB->ChunksLayaut.YChunks);
	if (ensureMsgf(ChunkX >= 0 && ChunkY >= 0, TEXT("Invalid chunks adress"))) {
		ChunkX_ = ChunkX;
		ChunkY_ = ChunkY;
	}
}
void UOWBDensityDataBuilder::SetLayer(EOWBMeshBlockTypes MapLayer) {
	Layer = MapLayer;
}

FDensityPoint UOWBDensityDataBuilder::BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings)
{
	FDensityPoint ThisPointData(0, FLinearColor(0, 0, 0, 0));
	if (ensureMsgf( OWB != nullptr,TEXT("Open world bakery not set, call BindToOpenWOrldBakery first"))
		&& ensureMsgf(OWB->MapWidth > 0, TEXT("Open world bakery has no world cooked (have you called CalcHillsLayaut() already?)"))) {
		int Z = VoxelCoordinates.Z;
		int X = VoxelCoordinates.X; // +ChunkSlot.X * Settings.ChunkRadius.X;
		int Y = VoxelCoordinates.Y; // +ChunkSlot.Y * Settings.ChunkRadius.Y;

		if (ChunkX_ >= 0) {
			FOWBMeshBlocks_set& ChunkDescrs = OWB->Chunks[ChunkX_ + ChunkY_ * OWB->ChunksLayaut.XChunks];
			if (ChunkDescrs.ChunkContents.Contains(Layer)) {
				X += ChunkDescrs.ChunkContents[Layer].MinPoint.X;
				Y += ChunkDescrs.ChunkContents[Layer].MinPoint.Y;
				Z += ChunkDescrs.ChunkContents[Layer].MinPoint.Z;
			} else {
				return ThisPointData;
			}
		}

		//if (ChunkSlot.X == 0)
		//	UE_LOG(LogTemp, Log, TEXT("GetCellIn %i:%i:%i from chunk %i:%i:%i. Final coords %i:%i:%i"),
		//		VoxelCoordinates.X, VoxelCoordinates.Y, VoxelCoordinates.Z,
		//		ChunkSlot.X, ChunkSlot.Y, ChunkSlot.Z,X,Y,Z);

		//if (Z <= 1 || X <= 1 || Y <= 1 || X >= OWB->MapWidth - 1 || Y >= OWB->MapHeight - 1) {
		//	return ThisPointData;
		//}

		X--;
		Y--;
		Z--;

		OpenWorldBakery::FSquareMeter& Ground = OWB->Ground(X, Y);

		float ThisCellHeight = Ground.GroundElevation / OWB->CellWidth;
		#if !UE_BUILD_SHIPPING
		if (isnan(ThisCellHeight)) {
			ThisCellHeight = 200000;
		}
		#endif
//		float BtmLVL = OWB->CellWidth * Z;
		ThisPointData.Value = ThisCellHeight - Z;

	}
	return ThisPointData;
}
