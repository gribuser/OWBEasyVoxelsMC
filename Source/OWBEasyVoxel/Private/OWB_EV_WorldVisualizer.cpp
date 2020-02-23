#include "OWB_EV_WorldVisualizer.h"

UOWB_EV_WorldVisializer::UOWB_EV_WorldVisializer() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UOWB_EV_WorldVisializer::BeginPlay()
{
	Super::BeginPlay();
	OpenWorldBakery = NewObject<UOpenWorldBakery>();
	GroundDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("GroundDensityBuilder"), RF_Standalone);
	WaterDensityBuilder = NewObject<UOWBDensityDataBuilder>(this, TEXT("WaterDensityBuilder"), RF_Standalone);

}

void UOWB_EV_WorldVisializer::RemoveVisualization()
{
}

void UOWB_EV_WorldVisializer::CreateVisualization()
{
}
