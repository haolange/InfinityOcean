#include "WindSource.h"

AWindSource::AWindSource()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWindSource::BeginPlay()
{
	Super::BeginPlay();
}

void AWindSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

