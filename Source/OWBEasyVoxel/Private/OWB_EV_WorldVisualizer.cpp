#include "OWB_EV_WorldVisualizer.h"
#include "Engine/World.h"

UOWB_EV_WorldVisializer::UOWB_EV_WorldVisializer() {
	PrimaryComponentTick.bCanEverTick = true;
	LayersToDraw.Add(Ground);
	//LayersToDraw.Add(Lake);
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
				FOWBMeshBlocks_set& CurChunksDescr = OpenWorldBakery->Chunks[y * OpenWorldBakery->ChunksLayaut.XChunks + x];
				for (EOWBMeshBlockTypes& Layer : LayersToDraw) {
					if (CurChunksDescr.ChunkContents.Contains(Layer)) {
						FOWBMeshBlocks_set_contents& LayerChunk = CurChunksDescr.ChunkContents[Layer];
						DrawChunk(LayerChunk);
						AOWB_EV_Chunk* NewChunk = GetWorld()->SpawnActor<AOWB_EV_Chunk>();
						NewChunk->WorldVisualizer = this;
						NewChunk->LayerToDraw = Layer;
						NewChunk->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

						FIntVector ChunkMetrics = LayerChunk.MaxPoint - LayerChunk.MinPoint;

						FVector MeshLocation = { VoxelSize, VoxelSize, VoxelSize };
						MeshLocation.X *= LayerChunk.MinPoint.X - 1 + 0.5 * ChunkMetrics.X;
						MeshLocation.Y *= LayerChunk.MinPoint.Y - 1 + 0.5 * ChunkMetrics.Y;
						MeshLocation.Z *= LayerChunk.MinPoint.Z - 1 + 0.5 * ChunkMetrics.Z;

						NewChunk->SetActorRelativeLocation(MeshLocation, false, nullptr, {});
						NewChunk->BindToOpenWOrldBakery(OpenWorldBakery, x, y);
						NewChunk->State = EOWBEVChunkStates::OWBEV_Pending;
						NewChunk->ChunkDescr = &CurChunksDescr;
						ChunksVisualizers.Add(NewChunk);
					}
				}
				if (CurChunksDescr.ChunkContents.Contains(EOWBMeshBlockTypes::Ocean)) {
					PlaceOcean(x, y);
				}
			}
		}
//		PrimaryComponentTick.bCanEverTick = true;
	}
}

void UOWB_EV_WorldVisializer::DrawChunk(FOWBMeshBlocks_set_contents& LayerChunk) {
	if (ChunkVisualBP != nullptr) {
		for (FOWBMeshChunk& Microchunk : LayerChunk.TypedBlocks) {
			FIntVector ChunkMetrics = Microchunk.MaxPoint - Microchunk.MinPoint;

			FVector ScaleVect(ChunkMetrics.X, ChunkMetrics.Y, ChunkMetrics.Z);
			ScaleVect *= VoxelSize / 100;

			FTransform MyTransform;
			MyTransform.SetScale3D(ScaleVect);
			AActor* NewChunkBox = GetWorld()->SpawnActor<AActor>(ChunkVisualBP, MyTransform);
			NewChunkBox->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

			FVector MeshLocation = { VoxelSize, VoxelSize, VoxelSize };
			MeshLocation.X *= Microchunk.MinPoint.X + ChunkMetrics.X / 2;
			MeshLocation.Y *= Microchunk.MinPoint.Y + ChunkMetrics.Y / 2;
			MeshLocation.Z *= Microchunk.MinPoint.Z + ChunkMetrics.Z / 2;

			NewChunkBox->SetActorRelativeLocation(MeshLocation, false, nullptr, {});
		}
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
