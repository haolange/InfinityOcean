#include "WindSourceActor.h"

AWindSourceActor::AWindSourceActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWindSourceActor::BeginPlay()
{
	Super::BeginPlay();
}

void AWindSourceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

