#include "OWB_EV_WorldVisualizer.h"
#include "Engine/World.h"

UOWB_EV_WorldVisializer::UOWB_EV_WorldVisializer() {
	PrimaryComponentTick.bCanEverTick = true;
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
				AOWB_EV_Chunk* NewChunk = GetWorld()->SpawnActor<AOWB_EV_Chunk>();
				NewChunk->WorldVisualizer = this;
				NewChunk->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
				FVector NewLocation = FVector::ZeroVector;
				NewLocation.X += ChunkSize * x * OpenWorldBakery->ChunksLayaut.ChunkWidth;
				NewLocation.Y += ChunkSize * y * OpenWorldBakery->ChunksLayaut.ChunkHeight;
				NewChunk->SetActorRelativeLocation(NewLocation, false, nullptr, {});

				NewChunk->BindToOpenWOrldBakery(OpenWorldBakery, x, y);
				NewChunk->State = EOWBEVChunkStates::OWBEV_Pending;
				ChunksVisualizers.Add(NewChunk);
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

