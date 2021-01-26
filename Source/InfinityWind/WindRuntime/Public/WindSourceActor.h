#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindSourceActor.generated.h"

UCLASS()
class WINDRUNTIME_API AWindSourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWindSourceActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
