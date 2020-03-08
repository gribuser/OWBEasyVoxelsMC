#include "OWB_EV_WorldVisualizer.h"
#include "Materials/MaterialInstanceDynamic.h"

UOWB_EV_WorldVisializer::UOWB_EV_WorldVisializer() {
	PrimaryComponentTick.bCanEverTick = true;
	LayersToDraw.Add(Ground);
	LayersToDraw.Add(Lake);
	//LayersToDraw.Add(River);
}

void UOWB_EV_WorldVisializer::BeginPlay()
{
	Super::BeginPlay();
	OpenWorldBakery = NewObject<UOpenWorldBakeryDebugger>();
	OpenWorldBakery->DebugTrapFrom = DebugTrapFrom;
	OpenWorldBakery->DebugTrapTo = DebugTrapTo;
	//GroundDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("GroundDensityBuilder"), RF_Standalone);
	//WaterDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("WaterDensityBuilder"), RF_Standalone);

}

void UOWB_EV_WorldVisializer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	int NumWorking = 0;
	int NumPending = 0;
	for (AOWB_EV_Chunk* Chunk : ChunksVisualizers) {
		if (Chunk != nullptr)
			switch (Chunk->State){
				case EOWBEVChunkStates::OWBEV_Pending: NumPending++; break;
				case EOWBEVChunkStates::OWBEV_Working: NumWorking++; break;
			}
	}

	int ThreadsToStart = FMath::Clamp(OpenWorldBakery->ThreadsToUse - NumWorking, 0, NumPending);
	if (ThreadsToStart > 0) {
		for (AOWB_EV_Chunk* Chunk : ChunksVisualizers) {
			if (Chunk != nullptr && Chunk->State == EOWBEVChunkStates::OWBEV_Pending) {
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
						DrawChunkBox(LayerChunk);
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


						if (Layer == Ground && DebugMaterialTemplate != nullptr && DebugBitmapForThis(x,y))
							NewChunk->DebugMaterial = UMaterialInstanceDynamic::Create(DebugMaterialTemplate, this);
						
						if (TypedMaterials.Contains(Layer))
							NewChunk->Material = UMaterialInstanceDynamic::Create(TypedMaterials[Layer], this);

						NewChunk->ChunkDescr = &CurChunksDescr;
						ChunksVisualizers.Add(NewChunk);

						if (Layer == Ground && LayerChunk.MinPoint.Z == (int)(OpenWorldBakery::OceanDeep /OpenWorldBakery->CellWidth - 1)) {
							PlaceOcean(x, y, false);
						}
					}
				}
				if (CurChunksDescr.ChunkContents.Contains(EOWBMeshBlockTypes::Ocean))
					PlaceOcean(x, y, true);

				if (!CurChunksDescr.ChunkContents.Contains(EOWBMeshBlockTypes::Ground))
					PlaceOcean(x, y, false);
			}
		}
//		PrimaryComponentTick.bCanEverTick = true;
	}
}

bool UOWB_EV_WorldVisializer::DebugBitmapForThis(int x, int y) {
	if (DebugVoxels.Num() == 0)
		return true;
	for (FIntPoint& DVox : DebugVoxels) {
		if (DVox.X == x && DVox.Y == y)
			return true;
	}
	return false;
}

void UOWB_EV_WorldVisializer::DrawChunkBox(FOWBMeshBlocks_set_contents& LayerChunk) {
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
			ChunksAdditionalActors.Add(NewChunkBox);
		}
	}
}

void UOWB_EV_WorldVisializer::RemoveVisualization() {
	// ...
	for (AOWB_EV_Chunk* ChunkInWorld : ChunksVisualizers)
		if (ChunkInWorld != nullptr)
			ChunkInWorld->Destroy();
	ChunksVisualizers.Empty();

	for (AActor* Actor : ChunksAdditionalActors)
		if (Actor != nullptr)
			Actor->Destroy();
	ChunksAdditionalActors.Empty();
}

const float SeaPlaneSize = 100;
void UOWB_EV_WorldVisializer::PlaceOcean(int X, int Y, bool Water) {
	float Scale = OpenWorldBakery->ChunksLayaut.ChunkWidth * VoxelSize / SeaPlaneSize;
	FActorSpawnParameters SpamParams;

	FTransform MyTransform;
	MyTransform.SetScale3D({ Scale,Scale,10.0 });

	TSubclassOf<AActor> ActorToSpawn = Water ? OceanPlaneBP : OceandeepPlaneBP;

	if (ensureMsgf(ActorToSpawn != nullptr, TEXT("Ocean plane template not defined, see world generator props"))) {
		AActor* APlaneActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, MyTransform);
		APlaneActor->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		APlaneActor->SetActorRelativeLocation({
			SeaPlaneSize / 2 * Scale + OpenWorldBakery->ChunksLayaut.ChunkWidth * VoxelSize * X,
			SeaPlaneSize / 2 * Scale + OpenWorldBakery->ChunksLayaut.ChunkHeight * VoxelSize * Y,
			Water ? 0 : (float)(OpenWorldBakery::OceanDeep / OpenWorldBakery->CellWidth * VoxelSize) });
		ChunksAdditionalActors.Add(APlaneActor);
	}
}
