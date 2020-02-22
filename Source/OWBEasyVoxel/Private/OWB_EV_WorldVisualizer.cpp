#include "OWB_EV_WorldVisualizer.h"

AWorldVisializer::AWorldVisializer() {

}

void AWorldVisializer::BeginPlay()
{
	Super::BeginPlay();
	GroundDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("GroundDensityBuilder"), RF_Standalone);
	WaterDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("WaterDensityBuilder"), RF_Standalone);

}
