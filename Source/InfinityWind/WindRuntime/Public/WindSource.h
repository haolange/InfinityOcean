#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindSource.generated.h"

UCLASS()
class WINDRUNTIME_API AWindSource : public AActor
{
	GENERATED_BODY()
	
public:	
	AWindSource();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
