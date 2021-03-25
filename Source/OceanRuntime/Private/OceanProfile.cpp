#include "OceanProfile.h"

UOceanProfile::UOceanProfile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Period = 100;
	TimeScale = 1;
	Fraction = 0;
	Aplitude = 0.75f;
	Choppyness = 1;
	WindSpeed = 1;
	WindDependency = 0.95f;
	WindDir = FVector2D(0.85f, 0.75f);
}

#if WITH_EDITOR
void UOceanProfile::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateParameter();
}
#endif

void UOceanProfile::UpdateParameter()
{
	Parameters.Period = Period;
	Parameters.TimeScale = TimeScale;
	Parameters.Choppyness = Choppyness;
	Parameters.Fraction = Fraction;
	Parameters.Aplitude = Aplitude;
	Parameters.WindDependency = WindDependency;
	Parameters.WindSpeed = WindSpeed;
	Parameters.WindDir = WindDir;
}

