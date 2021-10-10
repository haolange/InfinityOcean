#pragma once

#include "CoreMinimal.h"
#include "OceanTexture.h"
#include "Engine/StaticMesh.h"

#include "OceanProfile.generated.h"


UCLASS(Blueprintable)
class OCEANRUNTIME_API UOceanProfile : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	float Period;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	float TimeScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	float Fraction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	float Aplitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	float Choppyness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	float WindSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	float WindDependency;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveParameter")
	FVector2D WindDir;

	UPROPERTY()
	FOceanParameterStruct Parameters;

public:
	#if WITH_EDITOR
		virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif

	UFUNCTION(BlueprintCallable, Category = "WaveParameter")
	void UpdateParameter();
};
