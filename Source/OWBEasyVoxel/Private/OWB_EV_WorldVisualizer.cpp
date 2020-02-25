#include "OWB_EV_WorldVisualizer.h"
#include "Engine/World.h"

UOWB_EV_WorldVisializer::UOWB_EV_WorldVisializer() {
	PrimaryComponentTick.bCanEverTick = true;
	LayersToDraw.Add(Ground);
//	LayersToDraw.Add(Lake);
}

void UOWB_EV_WorldVisializer::BeginPlay()
{
	Super::BeginPlay();
	OpenWorldBakery = NewObject<UOpenWorldBakery>();
	//GroundDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("GroundDensityBuilder"), RF_Standalone);
	//WaterDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("WaterDensityBuilder"), RF_Standalone);

}

void UOWB_EV_WorldVisializer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	int NumWorking = 0;
	int NumPending = 0;
	for (AOWB_EV_Chunk* Chunk : ChunksVisualizers) {
		switch (Chunk->State){
			case EOWBEVChunkStates::OWBEV_Pending: NumPending++; break;
			case EOWBEVChunkStates::OWBEV_Working: NumWorking++; break;
		}
	}

	int ThreadsToStart = FMath::Clamp(OpenWorldBakery->ThreadsToUse - NumWorking, 0, NumPending);
	if (ThreadsToStart > 0) {
		for (AOWB_EV_Chunk* Chunk : ChunksVisualizers) {
			if (Chunk->State == EOWBEVChunkStates::OWBEV_Pending) {
				Chunk->InitTerrainBuild();
				ThreadsToStart--;
				NumPending--;
			}
			if (ThreadsToStart == 0)
				break;
		}
		//if (NumPending <= 0)
		//	PrimaryComponentTick.bCanEverTick = false;
	}
}

void UOWB_EV_WorldVisializer::CreateVisualization() {
	if (ensureMsgf(IsValid(OpenWorldBakery), TEXT("OpenWorldBakery object broken"))
		&& ensureMsgf(OpenWorldBakery->bChunksReady, TEXT("Chunks were not setup properly, call SetupChunks() first"))) {
		if (ChunksVisualizers.Num() > 0) {
			RemoveVisualization();
		}
//		FTransform MyTransform = GetComponentTransform();
//		FVector MyLocation = GetComponentLocation();
		for (int x = 0; x < OpenWorldBakery->ChunksLayaut.XChunks; x++) {
			for (int y = 0; y < OpenWorldBakery->ChunksLayaut.YChunks; y++) {
				FOWBMeshBlocks_set& CurChunksDescr = OpenWorldBakery->Chunks[y * OpenWorldBakery->ChunksLayaut.XChunks + y];
				for (EOWBMeshBlockTypes& Layer : LayersToDraw) {
					if (CurChunksDescr.Blocks.Contains(Layer)) {
						FOWBMeshChunk& LayerChunk = CurChunksDescr.Blocks[Layer];
						AOWB_EV_Chunk* NewChunk = GetWorld()->SpawnActor<AOWB_EV_Chunk>();
						NewChunk->WorldVisualizer = this;
						NewChunk->LayerToDraw = Layer;
						NewChunk->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

						FVector MeshLocation = { VoxelSize, VoxelSize, VoxelSize };
						MeshLocation.X *= LayerChunk.MinPoint.X - x * OpenWorldBakery->ChunksLayaut.ChunkWidth - 1 + (LayerChunk.MaxPoint.X - LayerChunk.MinPoint.X) / 2;
						MeshLocation.Y *= LayerChunk.MinPoint.Y - y * OpenWorldBakery->ChunksLayaut.ChunkHeight - 1 + (LayerChunk.MaxPoint.Y - LayerChunk.MinPoint.Y) / 2;
						MeshLocation.Z *= LayerChunk.MinHeight / OpenWorldBakery->CellWidth;

						NewChunk->SetActorRelativeLocation(MeshLocation, false, nullptr, {});
						NewChunk->BindToOpenWOrldBakery(OpenWorldBakery, x, y);
						NewChunk->State = EOWBEVChunkStates::OWBEV_Pending;
						NewChunk->ChunkDescr = &CurChunksDescr;
						ChunksVisualizers.Add(NewChunk);
					}
				}
				if (CurChunksDescr.Blocks.Contains(EOWBMeshBlockTypes::Ocean)) {
					PlaceOcean(x, y);
				}
			}
		}
//		PrimaryComponentTick.bCanEverTick = true;
	}
}

void UOWB_EV_WorldVisializer::RemoveVisualization() {
	// ...
	for (AOWB_EV_Chunk* ChunkInWorld : ChunksVisualizers) {
		ChunkInWorld->ConditionalBeginDestroy();
	}
	ChunksVisualizers.Empty();
}

const float SeaPlaneSize = 100;
void UOWB_EV_WorldVisializer::PlaceOcean(int X, int Y) {
	float Scale = OpenWorldBakery->ChunksLayaut.ChunkWidth * VoxelSize / SeaPlaneSize;
	FActorSpawnParameters SpamParams;

	FTransform MyTransform;
	MyTransform.SetScale3D({ Scale,Scale,10.0 });
	if (ensureMsgf(OceanPlaneBP != nullptr, TEXT("Ocean plane template not defined, see world generator props"))) {
		AActor* APlaneActor = GetWorld()->SpawnActor<AActor>(OceanPlaneBP, MyTransform);
		APlaneActor->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		APlaneActor->SetActorRelativeLocation({ SeaPlaneSize / 2 * Scale + OpenWorldBakery->ChunksLayaut.ChunkWidth * VoxelSize * X, SeaPlaneSize / 2 * Scale + OpenWorldBakery->ChunksLayaut.ChunkHeight * VoxelSize * Y, 0 });
	}
}
