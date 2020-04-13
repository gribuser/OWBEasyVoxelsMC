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

void UOWBDensityDataBuilder::DoGetFDensityPoint(const FIntVector& VoxelCoordinates, FDensityPoint& DensityPoint)
{
	DensityPoint = FDensityPoint(0, FLinearColor(0, 0, 0, 0));
	if (ensureMsgf(OWB != nullptr, TEXT("Open world bakery not set, call BindToOpenWOrldBakery first"))
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
			}
			else {
				return;
			}
		}

		//if (ChunkSlot.X == 0)
		//	UE_LOG(LogTemp, Log, TEXT("GetCellIn %i:%i:%i from chunk %i:%i:%i. Final coords %i:%i:%i"),
		//		VoxelCoordinates.X, VoxelCoordinates.Y, VoxelCoordinates.Z,
		//		ChunkSlot.X, ChunkSlot.Y, ChunkSlot.Z,X,Y,Z);

		if (X <= 0 || Y <= 0 || X >= OWB->MapWidth - 1 || Y >= OWB->MapHeight - 1) {
			return;
		}

		X--;
		Y--;
		Z--;
#if !UE_BUILD_SHIPPING
		if (X >= OWB->DebugTrapFrom.X && X <= OWB->DebugTrapTo.X && Y >= OWB->DebugTrapFrom.Y && Y <= OWB->DebugTrapTo.Y) {
			UE_LOG(LogTemp, Log, TEXT("Debug trap %i:%i"), X, Y);
		}
#endif
		const FOWBSquareMeter& CookedGround = OWB->CookedHeightMap[X + Y * OWB->MapWidth];

		OWBVoxFloat ThisCellHeight = CookedGround.HeightByType(Layer) / OWB->CellWidth;
		if (Layer == EOWBMeshBlockTypes::Ground && ThisCellHeight <= OpenWorldBakery::OceanDeep) {
			ThisCellHeight -= 0.001; // hack to supress blinking
		}
#if !UE_BUILD_SHIPPING
		if (isnan(ThisCellHeight)) {
			ThisCellHeight = 200000;
		}
#endif
		//		float BtmLVL = OWB->CellWidth * Z;
		DensityPoint.Value = ThisCellHeight - Z;

		if (Layer == EOWBMeshBlockTypes::FreshWater) {
			// Water needs some extra info
			FVector2D NormalAsColor = CookedGround.Stream;
			//NormalAsColor.X = 0.1 * FMath::RoundToFloat(NormalAsColor.X * 10);
			NormalAsColor.Y *= -1;
			NormalAsColor = (NormalAsColor + FVector2D(1.0, 1.0)) / 2;

			float Deep = FMath::Clamp((float)sqrt((ThisCellHeight * OWB->CellWidth - CookedGround.GroundSurface) / OWB->CellWidth) / 10, 0.0f, 1.0f);

			DensityPoint.Color.R = NormalAsColor.X;
			DensityPoint.Color.G = NormalAsColor.Y;
			DensityPoint.Color.B = Deep;
			DensityPoint.Color.A = 1.0;
		}
	}
}

FDensityPoint UOWBDensityDataBuilder::BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings)
{
	FDensityPoint ThisPointData;
	DoGetFDensityPoint(VoxelCoordinates, ThisPointData);
	return ThisPointData;
}


FOWB_MarchingCubes::FOWB_MarchingCubes(FVoxelSettings MCSettings, UOWBDensityDataBuilder* DataBuilder) :
	FMarchingCubes({}, nullptr, MCSettings, { 0,0,0 }), MyDDBuider(DataBuilder){}

void FOWB_MarchingCubes::BuildDensityPoint(const FIntVector& VoxelCoordinates, FDensityPoint& DensityPoint) const
{
	MyDDBuider->DoGetFDensityPoint(VoxelCoordinates, DensityPoint);
}

FOWB_VoxelDataConverter::FOWB_VoxelDataConverter(const TArray<FVector>& InCoordinates, const TArray<int32>& InTriangles, const TMap<FIntVector, FDensityPoint>& InDensityData, const FVoxelSettings& InSettings, UOWBDensityDataBuilder* DataBuilder) :
	FVoxelDataConverter(InCoordinates, InTriangles, InDensityData, nullptr, InSettings, {0,0,0}, true, false),
	MyDDBuider(DataBuilder){}

void FOWB_VoxelDataConverter::BuildDensityPoint(const FIntVector& VoxelCoordinates, FDensityPoint& DensityPoint) const
{
	MyDDBuider->DoGetFDensityPoint(VoxelCoordinates, DensityPoint);
}
