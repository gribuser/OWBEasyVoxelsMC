// Open World Bakery free additions
// Copyright 2020 Dmitry Grivov, g@primitive.game, all rights reserved

#include "OWB_EV_ChunkVisualizer.h"
#include "OWB_EV_WorldVisualizer.h"
#include "Materials/MaterialInstanceDynamic.h"


AOWB_EV_Chunk::AOWB_EV_Chunk()
{
	PrimaryActorTick.bCanEverTick = true;
	//	SetMobility(EComponentMobility::Movable);
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Empty root"));
	SetRootComponent(ProceduralMesh);
	ProceduralMesh->bUseAsyncCooking = false;
	ProceduralMesh->bUseComplexAsSimpleCollision = true;

	DensityBuilder = NewObject<UOWBDensityDataBuilder>();

	MCSettings.ISOLevel = 0.0000001;
	MCSettings.bInverted = false;
	MCSettings.bUseSharedPoints = true;
	MCSettings.bForceManifold = false;
}


void AOWB_EV_Chunk::BeginPlay()
{
	Super::BeginPlay();
}

void AOWB_EV_Chunk::BindToOpenWOrldBakery(UOpenWorldBakeryTextured* OpenWorldBakery, int ChunkX, int ChunkY) {
	OWB = OpenWorldBakery;
	if (ensureMsgf(OWB != nullptr, TEXT("Open world bakery not set, unsafe chunks setup - can not check - can not check validity")))
		if (ensureMsgf(OWB->ChunksLayaut.XChunks > 0, TEXT("Open world bakery haven't executed CookChunks() yet, unsafe chunks setup - can not check validity")))
			ensureMsgf(OWB->ChunksLayaut.XChunks >= ChunkX && OWB->ChunksLayaut.YChunks >= ChunkY, TEXT("You set wrong chunk %i:%i. Only %i:%i available"), ChunkX, ChunkY, OWB->ChunksLayaut.XChunks, OWB->ChunksLayaut.YChunks);
	if (ensureMsgf(ChunkX >= 0 && ChunkY >= 0, TEXT("Invalid chunks adress"))) {
		ChunkX_ = ChunkX;
		ChunkY_ = ChunkY;
	}
	DensityBuilder->BindToOpenWOrldBakery(OWB);
	DensityBuilder->SetChunk(ChunkX, ChunkY);

	MCSettings.Resolution = WorldVisualizer->VoxelSize;
}

void AOWB_EV_Chunk::InitTerrainBuild()
{
	if (!ensureMsgf(ChunkX_ >= 0, TEXT("Call BindToOpenWOrldBakery() first"))) {
		State = EOWBEVChunkStates::OWBEV_Idle;
		return;
	}
	State = EOWBEVChunkStates::OWBEV_Working;

	DensityBuilder->SetLayer(LayerToDraw);
	const FOWBMeshChunk& LayerChunk = ChunkDescr->ChunkContents[LayerToDraw];

	MCSettings.Units = LayerChunk.MaxPoint - LayerChunk.MinPoint;

	if (MCSettings.Units.X <= 0 || MCSettings.Units.Y <= 0 || MCSettings.Units.Z <= 0) {
		State = EOWBEVChunkStates::OWBEV_Idle;
		return;
	}

	MCSettings.Units = MCSettings.Units + FIntVector(4, 4, 4);
	
	WorkerCubes = MakeShareable(new FOWB_MarchingCubes(MCWorker, MCSettings, DensityBuilder));

	TFunction<void()> BodyFunction = [this]
	{
		WorkerCubes->GenerateVoxelData();
	};

	TFunction<void()> OnCompleteFunction = [this]
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				WorkerMesh = MakeShareable(new FOWB_VoxelDataConverter(
					MCWorker,
					WorkerCubes->Coordinates,
					WorkerCubes->Triangles,
					{},
					MCSettings, //const bool UseGradientNormals, const bool UseFlatShading
					DensityBuilder
				));
				//WorkerCubes.Reset();
				//WorkerCubes = nullptr;
				TFunction<void()> BodyFunction2 = [this]
				{
					WorkerMesh->ConvertToMeshData();
				};

				TFunction<void()> OnCompleteFunction2 = [this]
				{
					AsyncTask(ENamedThreads::GameThread, [this]()
						{
							EndTerrainBuild(WorkerMesh->MeshData);
							//WorkerMesh.Reset();
							//WorkerMesh = nullptr;
						});
				};
				//WorkerMesh->ConvertToMeshData();
				//EndTerrainBuild(WorkerMesh->MeshData);
				MCWorker->StartWork(BodyFunction2, OnCompleteFunction2, nullptr);
			});
	};
	//WorkerCubes->GenerateVoxelData();
	//OnCompleteFunction();
	MCWorker->StartWork(BodyFunction, OnCompleteFunction, nullptr);
}


void AOWB_EV_Chunk::EndTerrainBuild(const FMeshData& AMeshData){
	WorldVisualizer->MeshGeneratorLock.Lock();
	ProceduralMesh->CreateMeshSection_LinearColor(0, AMeshData.Vertices, AMeshData.Triangles, AMeshData.Normals, AMeshData.UV0, AMeshData.Colors, AMeshData.Tangents, true);
	WorldVisualizer->MeshGeneratorLock.Unlock();

	const FOWBMeshChunk& LayerChunk = ChunkDescr->ChunkContents[LayerToDraw];

	UMaterialInstanceDynamic* ApplyedMaterial = nullptr;

	if (DebugMaterial != nullptr) {
		ApplyedMaterial = DebugMaterial;

		int BlockWidth = LayerChunk.MaxPoint.X - LayerChunk.MinPoint.X + 4;
		int BLockHeight = LayerChunk.MaxPoint.Y - LayerChunk.MinPoint.Y + 4;

		float TextureScaleX = 1.0f / BlockWidth / MCSettings.Resolution;
		float TextureScaleY = 1.0f / BLockHeight / MCSettings.Resolution;

		DebugMaterial->SetVectorParameterValue(TEXT("TextureScale"), { TextureScaleX ,TextureScaleY, 1 });

		ApplyedMaterial->SetVectorParameterValue(
			TEXT("MapShift"),
			{ (float)(0.5 - 1.5 / BlockWidth), (float)(0.5 - 1.5 / BLockHeight), 1 });
//			{ (float)0.5, (float)0.5, 0 });
		FDebugTextureParams ParamsForThis = WorldVisualizer->DebugTextureParams;
		if (!FullScaleDebugTexture) {
			ParamsForThis.CellSize = 3;
			ParamsForThis.bDrawDownlinks = false;
			ParamsForThis.bDrawUplinks = false;
			ParamsForThis.bDrawSlopeVector = false;
		}
		UCanvasRenderTarget2D* DebugTexture = OWB->CreateDebugTexture(
			LayerChunk.MinPoint.X - 2,
			LayerChunk.MinPoint.Y - 2,
			LayerChunk.MaxPoint.X + 2,
			LayerChunk.MaxPoint.Y + 2,
			ParamsForThis
		);

		DebugMaterial->SetTextureParameterValue(
			TEXT("ColorMap"),
			DebugTexture
		);
	}
	else if (Material != nullptr) {
		ApplyedMaterial = Material;
	}
	if (ApplyedMaterial != nullptr) {
		ProceduralMesh->SetMaterial(0, ApplyedMaterial);
		ApplyedMaterial->SetScalarParameterValue(TEXT("GridScale"), 0.1f / MCSettings.Resolution);
		float CenterX = (float)(LayerChunk.MinPoint.X + LayerChunk.MaxPoint.X) / 2 - 0.5;
		float ShiftX = CenterX / 10;

		float CenterY = (float)(LayerChunk.MinPoint.Y + LayerChunk.MaxPoint.Y) / 2 - 0.5;
		float ShiftY = CenterY / 10;
		ApplyedMaterial->SetVectorParameterValue(TEXT("GridShift"), { ShiftX, ShiftY, 0});
		ApplyedMaterial->SetScalarParameterValue(
			TEXT("GlowLevel"),
			WorldVisualizer->DebugTextureParams.DebugTerrainFill == EOWBDebugFillType::Water ? 0.5 : 0
		);
	}

	State = EOWBEVChunkStates::OWBEV_Idle;
}

void AOWB_EV_Chunk::Tick(float DeltaTime) {
}