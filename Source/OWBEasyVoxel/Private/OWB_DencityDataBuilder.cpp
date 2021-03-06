// Open World Bakery free additions
// Copyright 2020 Dmitry Grivov, g@primitive.game, all rights reserved

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
		if (ensureMsgf(OWB->ChunksLayout.XChunks > 0, TEXT("Open world bakery haven't executed CookChunks() yet, unsafe chunks setup - can not check validity")))
			ensureMsgf(OWB->ChunksLayout.XChunks >= ChunkX && OWB->ChunksLayout.YChunks >= ChunkY, TEXT("You set wrong chunk %i:%i. Only %i:%i available"), ChunkX, ChunkY, OWB->ChunksLayout.XChunks, OWB->ChunksLayout.YChunks);
	if (ensureMsgf(ChunkX >= 0 && ChunkY >= 0, TEXT("Invalid chunks adress"))) {
		ChunkX_ = ChunkX;
		ChunkY_ = ChunkY;
	}
}
void UOWBDensityDataBuilder::SetLayer(EOWBMeshBlockTypes MapLayer) {
	Layer = MapLayer;
}

FDensityPoint* UOWBDensityDataBuilder::DoGetFDensityPoint(const FIntVector& VoxelCoordinates)
{
	FDensityPoint* DensityPoint = new FDensityPoint(0, FLinearColor(0, 0, 0, 0));
	if (ensureMsgf(OWB != nullptr, TEXT("Open world bakery not set, call BindToOpenWOrldBakery first"))
		&& ensureMsgf(OWB->MapWidth > 0, TEXT("Open world bakery has no world cooked (have you called CalcHillsLayaut() already?)"))) {
		int Z = VoxelCoordinates.Z;
		int X = VoxelCoordinates.X; // +ChunkSlot.X * Settings.ChunkRadius.X;
		int Y = VoxelCoordinates.Y; // +ChunkSlot.Y * Settings.ChunkRadius.Y;

		if (ChunkX_ >= 0) {
			const FOWBMeshBlocks_set& ChunkDescrs = OWB->Chunks[ChunkX_ + ChunkY_ * OWB->ChunksLayout.XChunks];
			if (ChunkDescrs.ChunkContents.Contains(Layer)) {
				X += ChunkDescrs.ChunkContents[Layer].MinPoint.X;
				Y += ChunkDescrs.ChunkContents[Layer].MinPoint.Y;
				Z += ChunkDescrs.ChunkContents[Layer].MinPoint.Z;
			}
			else {
				return DensityPoint;
			}
		}

		//if (ChunkSlot.X == 0)
		//	UE_LOG(LogTemp, Log, TEXT("GetCellIn %i:%i:%i from chunk %i:%i:%i. Final coords %i:%i:%i"),
		//		VoxelCoordinates.X, VoxelCoordinates.Y, VoxelCoordinates.Z,
		//		ChunkSlot.X, ChunkSlot.Y, ChunkSlot.Z,X,Y,Z);

		if (X < 2 || Y < 2 || X >= OWB->MapWidth - 2 || Y >= OWB->MapHeight - 2) {
			return DensityPoint;
		}

		X -= 2;
		Y -= 2;
		Z -= 2;
#if !UE_BUILD_SHIPPING
		if (X >= OWB->DebugTrapFrom.X && X <= OWB->DebugTrapTo.X && Y >= OWB->DebugTrapFrom.Y && Y <= OWB->DebugTrapTo.Y) {
			UE_LOG(LogTemp, Log, TEXT("Debug trap %i:%i"), X, Y);
		}
#endif
		const FOWBSquareMeter& CookedGround = OWB->BakedHeightMap[X + Y * OWB->MapWidth];

		OWBVoxFloat ThisCellHeight = CookedGround.HeightByType(Layer) / OWB->CellWidth;
		if (Layer == EOWBMeshBlockTypes::Ground && ThisCellHeight <= OWB->OceanDeep) {
			ThisCellHeight -= 0.001; // hack to supress blinking
		}
#if !UE_BUILD_SHIPPING
		if (isnan(ThisCellHeight)) {
			ThisCellHeight = 200000;
		}
#endif
		//		float BtmLVL = OWB->CellWidth * Z;
		DensityPoint->Value = ThisCellHeight - Z;

		if (Layer == EOWBMeshBlockTypes::FreshWater) {
			// Water needs some extra info
			FVector2D NormalAsColor = CookedGround.Stream;
			//NormalAsColor.X = 0.1 * FMath::RoundToFloat(NormalAsColor.X * 10);
			float Deep = 0;
			if (CookedGround.GroundSurface < CookedGround.WaterSurface)
				Deep = FMath::Clamp((float)sqrt((CookedGround.WaterSurface - CookedGround.GroundSurface) / OWB->CellWidth) / 5, 0.0f, 1.0f);
			else
				NormalAsColor /= 2;
			NormalAsColor.Y *= -1;
			NormalAsColor = (NormalAsColor + FVector2D(1.0, 1.0)) / 2;

			DensityPoint->Color.R = NormalAsColor.X;
			DensityPoint->Color.G = NormalAsColor.Y;
			DensityPoint->Color.B = Deep;
			DensityPoint->Color.A = 1.0;
		}
		else if (Layer == EOWBMeshBlockTypes::Ground) {
			DensityPoint->Color = OWB->TerrainVoxelColor(CookedGround);
		}
	}
	return DensityPoint;
}

void UOWBDensityDataBuilder::BuildDensityPoint_Implementation(const FIntVector& VoxelCoordinates, const FIntVector& ChunkSlot, const FVoxelSettings& Settings, FDensityPoint& DensityPoint)
{
	FDensityPoint* Tmp = DoGetFDensityPoint(VoxelCoordinates);
	DensityPoint = *Tmp;
	delete Tmp;
}


FOWB_MarchingCubes::FOWB_MarchingCubes(TSharedRef<FEasyVoxelsMCWorker, ESPMode::ThreadSafe> InWorker, FVoxelSettings MCSettings, UOWBDensityDataBuilder* DataBuilder) :
	FMarchingCubes(MCSettings, { 0,0,0 }, InWorker), MyDDBuider(DataBuilder){}

FDensityPoint* FOWB_MarchingCubes::GetDensityPoint(const FIntVector& VoxelCoordinates)
{
	return MyDDBuider->DoGetFDensityPoint(VoxelCoordinates);
}

FOWB_VoxelDataConverter::FOWB_VoxelDataConverter(const FVoxelSettings& InSettings, TSharedRef<FEasyVoxelsMCWorker, ESPMode::ThreadSafe> InWorker, UOWBDensityDataBuilder* DataBuilder) :
	FVoxelDataConverter(InSettings, {0,0,0}, InWorker),
	MyDDBuider(DataBuilder){}

FDensityPoint* FOWB_VoxelDataConverter::GetDensityPoint(const FIntVector& VoxelCoordinates)
{
	return MyDDBuider->DoGetFDensityPoint(VoxelCoordinates);
}
