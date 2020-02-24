// Open World Baker
// Copyright 2019 Dmitry Grivov, grib@gribuser.ru, all rights reserved.


#include "OWB_EV_ChunkVisualizer.h"
//#include "Components/StaticMeshComponent.h"
//#include "VoxelLib/VoxelDataConverter.h"
//#include "RuntimeMeshActor.h"
//#include "RuntimeMeshCore.h"
//#include "Providers/RuntimeMeshProviderStatic.h"
//#include "Providers/RuntimeMeshProviderStaticMesh.h"

AOWB_EV_Chunk::AOWB_EV_Chunk()
{
	PrimaryActorTick.bCanEverTick = true;
//	SetMobility(EComponentMobility::Movable);
	NewPMC = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	NewPMC->bUseAsyncCooking = false;
	NewPMC->bUseComplexAsSimpleCollision = true;
	SetRootComponent(NewPMC);
}


void AOWB_EV_Chunk::BeginPlay()
{
	Super::BeginPlay();
}

//// Called every frame
//void ATerrainVoxel::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//	if (CleanupMesh) {
//		NewPMC->ClearAllMeshSections();
//		CleanupMesh = false;
//	}
//}
//
//void ATerrainVoxel::InitTerrainBuild()
//{
//	CleanupMesh = false;
//	WorkerCubes = MakeShareable(new FMarchingCubes({}, DensityBuilder, MCSettings, Chunk));
//
//	TFunction<void()> BodyFunction = [this]
//	{
//		WorkerCubes->GenerateVoxelData();
//	};
//
//	TFunction<void()> OnCompleteFunction = [this]
//	{
//		AsyncTask(ENamedThreads::GameThread, [this]()
//			{
//				WorkerMesh = MakeShareable(new FVoxelDataConverter(
//					WorkerCubes->Coordinates,
//					WorkerCubes->Triangles,
//					{},
//					DensityBuilder,
//					MCSettings,
//					WorkerCubes->ChunkSlot,
//					false, false //const bool UseGradientNormals, const bool UseFlatShading
//				));
//				//WorkerCubes.Reset();
//				//WorkerCubes = nullptr;
//				TFunction<void()> BodyFunction2 = [this]
//				{
//					WorkerMesh->ConvertToMeshData();
//				};
//
//				TFunction<void()> OnCompleteFunction2 = [this]
//				{
//					AsyncTask(ENamedThreads::GameThread, [this]()
//						{
//							EndTerrainBuild(WorkerMesh->MeshData);
//							//WorkerMesh.Reset();
//							//WorkerMesh = nullptr;
//						});
//				};
//				//WorkerMesh->ConvertToMeshData();
//				//EndTerrainBuild(WorkerMesh->MeshData);
//				WorkerMesh->StartWork(BodyFunction2, OnCompleteFunction2, nullptr);
//			});
//	};
//
//	WorkerCubes->StartWork(BodyFunction, OnCompleteFunction, nullptr);
//}
//
//void ATerrainVoxel::EndTerrainBuild(const FMeshData& AMeshData)
//{
//	UE_LOG(LogTemp, Log, TEXT("Drawing %i:%i:%i"), Chunk.X, Chunk.Y, Chunk.Z);
//	// empty one
//	if (AMeshData.Vertices.Num() == 0) {
//		UE_LOG(LogTemp, Log, TEXT("Empty %i:%i:%i"), Chunk.X, Chunk.Y, Chunk.Z);
//		if (Next != nullptr)
//			Next->InitTerrainBuild();
//		return;
//	}
//
////	// ============================================
////
////	URuntimeMeshProviderStatic* StaticProvider = NewObject<URuntimeMeshProviderStatic>(this);
////	GetRuntimeMeshComponent()->GetOrCreateRuntimeMesh()->Initialize(StaticProvider);
////
////	//	StaticProvider->SetupMaterialSlot(0, FName("Cube Base"), UMaterial::GetDefaultMaterial(MD_Surface));
////	StaticProvider->SetupMaterialSlot(0, "Whatever", (DynamicMaterial != nullptr)?DynamicMaterial: StaticMaterial);
////	// Setup section
////	FRuntimeMeshSectionProperties Properties;
////	Properties.bCastsShadow = true;
////	Properties.bIsVisible = true;
////	Properties.MaterialSlot = 0;
////	StaticProvider->CreateSection(0, 0, Properties);
////
////	// Setup the mesh data
////	FRuntimeMeshRenderableMeshData MeshData(false, false, 1, false);
////	//FRuntimeMeshCollisionData CollisionData;
////	//FRuntimeMeshCollisionVertexStream& CollisionVertices = CollisionData.Vertices;
////	//FRuntimeMeshCollisionTriangleStream& CollisionTriangles = CollisionData.Triangles;
////	FRotator MyRotation = GetActorRotation();
////	FVector MyLocation = GetActorLocation();
////
////	for (size_t i = 0; i < AMeshData.Vertices.Num(); i++) {
////		MeshData.Positions.Add(MyRotation.RotateVector(AMeshData.Vertices[i]) + MyLocation);
////		MeshData.Tangents.Add(MyRotation.RotateVector(AMeshData.Normals[i]), MyRotation.RotateVector(AMeshData.Tangents[i].TangentX));
////		MeshData.Colors.Add(AMeshData.Colors[i].ToRGBE());
////		MeshData.TexCoords.Add(AMeshData.UV0[i]);
//////		CollisionVertices.Add(AMeshData.Vertices[i]);
////	}
////	for (size_t i = 0; i < AMeshData.Triangles.Num() / 3; i++) {
////		MeshData.Triangles.AddTriangle(AMeshData.Triangles[i * 3], AMeshData.Triangles[i * 3 + 1], AMeshData.Triangles[i * 3 + 2]);
//////		CollisionTriangles.Add(AMeshData.Triangles[i * 3], AMeshData.Triangles[i * 3 + 1], AMeshData.Triangles[i * 3 + 2]);
////	}
////	StaticProvider->SetRenderableSectionAffectsCollision(0, true);
////	FRuntimeMeshCollisionSettings Settings;
////	Settings.bUseAsyncCooking = true;
////	Settings.bUseComplexAsSimple = true;
////	StaticProvider->SetCollisionSettings(Settings);
////
////	StaticProvider->UpdateSection(0, 0, MeshData);
//
//
////
////	// Add a single box element for the simple collision
//////	Settings.Boxes.Emplace(MyLocation + MCSettings.Resolution * (MCSettings.ChunkRadius.X/2), MyRotation, MCSettings.Resolution * (MCSettings.ChunkRadius.X / 2), MCSettings.Resolution * (MCSettings.ChunkRadius.X / 2), MCSettings.Resolution * (MCSettings.ChunkRadius.X / 2));
////
////	// Add the mesh
////	FRuntimeMeshCollisionSourceSectionInfo CollusionSRC(0, MeshData.Triangles.Num(), StaticProvider, 0, ERuntimeMeshCollisionFaceSourceType::Renderable);
////
////	CollisionData.CollisionSources.Add(CollusionSRC);
////	StaticProvider->SetCollisionSettings(Settings);
////	StaticProvider->SetCollisionMesh(CollisionData);
//
//
//	//FVector BoxRadius(500, 500, 500);
//	//// Now setup collision
//
//	//// Add a single box element for the simple collision
//	//Settings.Boxes.Emplace(BoxRadius.X * 2, BoxRadius.Y * 2, BoxRadius.Z * 2);
//
//
//	//// Setup Collision 
//	//FRuntimeMeshCollisionData CollisionData;
//	//FRuntimeMeshCollisionVertexStream& CollisionVertices = CollisionData.Vertices;
//	//FRuntimeMeshCollisionTriangleStream& CollisionTriangles = CollisionData.Triangles;
//
//	//// Generate verts
//	//CollisionVertices.Add(FVector(-BoxRadius.X * 5, BoxRadius.Y, BoxRadius.Z));
//	//CollisionVertices.Add(FVector(BoxRadius.X * 5, BoxRadius.Y, BoxRadius.Z));
//	//CollisionVertices.Add(FVector(BoxRadius.X, -BoxRadius.Y, BoxRadius.Z));
//	//CollisionVertices.Add(FVector(-BoxRadius.X, -BoxRadius.Y, BoxRadius.Z));
//
//	//CollisionVertices.Add(FVector(-BoxRadius.X, BoxRadius.Y, -BoxRadius.Z));
//	//CollisionVertices.Add(FVector(BoxRadius.X, BoxRadius.Y, -BoxRadius.Z));
//	//CollisionVertices.Add(FVector(BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z));
//	//CollisionVertices.Add(FVector(-BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z));
//
//	//// Pos Z
//	//CollisionTriangles.Add(0, 1, 3);
//	//CollisionTriangles.Add(1, 2, 3);
//	//// Neg X
//	//CollisionTriangles.Add(4, 0, 7);
//	//CollisionTriangles.Add(0, 3, 7);
//	//// Pos Y
//	//CollisionTriangles.Add(5, 1, 4);
//	//CollisionTriangles.Add(1, 0, 4);
//	//// Pos X
//	//CollisionTriangles.Add(6, 2, 5);
//	//CollisionTriangles.Add(2, 1, 5);
//	//// Neg Y
//	//CollisionTriangles.Add(7, 3, 6);
//	//CollisionTriangles.Add(3, 2, 6);
//	//// Neg Z
//	//CollisionTriangles.Add(7, 6, 4);
//	//CollisionTriangles.Add(6, 5, 4);
//
//
//	//// add the collision mesh for complex collision
//	//StaticProvider->SetCollisionMesh(CollisionData);
//	//// ============================================
//
//	SharedMeshGenLock->Lock();
//	NewPMC->CreateMeshSection_LinearColor(0, AMeshData.Vertices, AMeshData.Triangles, AMeshData.Normals, AMeshData.UV0, AMeshData.Colors, AMeshData.Tangents, true);
//	SharedMeshGenLock->Unlock();
//
//	NewPMC->CastShadow = 0;
//	if (DynamicMaterial != nullptr) {
//		NewPMC->SetMaterial(0, DynamicMaterial);
//		if (CookBitmaps)
//			RefreshDebugBitmap();
//	} else {
//		NewPMC->SetMaterial(0, StaticMaterial);
//	}
//
//	if (Next != nullptr)
//		Next->InitTerrainBuild();
//}
//
//void ATerrainVoxel::RefreshDebugBitmap()
//{
//	//// debug begin
//	//FVector MapShiftPos1 = { 0,0,0 };
//	//DynamicMaterial->SetVectorParameterValue(TEXT("WorldPos"), MapShiftPos1);
//	//float MapRescale1 = (float)1 / MCSettings.Resolution / (RHM->MapWidth / CutChunks + 2);
//	//DynamicMaterial->SetScalarParameterValue(TEXT("TextureScale"), MapRescale1);
//	//DynamicMaterial->SetScalarParameterValue(TEXT("GridScale"), (float)1 / MCSettings.Resolution / 10);
//	//return;
//	//// debug end
//
//	if (DynamicMaterial != nullptr && CookBitmaps) {
//		if (ensureMsgf(RHM->State == Ready, TEXT("Producing chunk over unready RHM"))) {
//			//		UE_LOG(LogTemp, Log, TEXT("Painting bitmap %i:%i"), Chunk.X, Chunk.Y);
//			FVector MapShiftPos = { 0,0,0 };
//			MapShiftPos.X -= MCSettings.Units.X * MCSettings.Resolution / 2 - MCSettings.Resolution * 0.5;// +MCSettings.Resolution * 1.5;
//			MapShiftPos.Y -= MCSettings.Units.Y * MCSettings.Resolution / 2 - MCSettings.Resolution * 0.5;
//			DebugTextureParams.Caller = this;
//			UCanvasRenderTarget2D* DebugTexture = RHM->CreateDebugTexture(
//				Chunk.X * MCSettings.ChunkRadius.X - 1,
//				Chunk.Y * MCSettings.ChunkRadius.Y - 1,
//				(Chunk.X + 1) * MCSettings.ChunkRadius.X + 1,
//				(Chunk.Y + 1) * MCSettings.ChunkRadius.Y + 1,
//				DebugTextureParams
//			);
//			DynamicMaterial->SetVectorParameterValue(TEXT("WorldPos"), MapShiftPos);
//			float MapRescale = (float)1 / MCSettings.Resolution / (RHM->MapWidth / CutChunks + 2);
//			DynamicMaterial->SetScalarParameterValue(TEXT("TextureScale"), MapRescale);
//			DynamicMaterial->SetScalarParameterValue(TEXT("GridScale"), (float)1 / MCSettings.Resolution / 10);
//			DynamicMaterial->SetTextureParameterValue(
//				TEXT("ColorMap"),
//				DebugTexture
//			);
//		}
//	}
//}
//
