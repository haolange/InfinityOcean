#pragma once

#include "CoreMinimal.h"
#include "OceanProfile.h"
#include "OceanTexture.h"
#include "OceanRender.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), hideCategories = (Tags, Activation, Cooking, AssetUserData, Collision))
class OCEANRUNTIME_API UOceanRender : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UOceanProfile* OceanProfile_A;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UTextureRenderTarget2D* DisplacementRT_A;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UOceanProfile* OceanProfile_B;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UTextureRenderTarget2D* DisplacementRT_B;


	UOceanRender();

	~UOceanRender();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:

	UPROPERTY()
	UOceanTexture* OceanTexture_A;

	UPROPERTY()
	UOceanTexture* OceanTexture_B;

	bool ShouldDrawTexture();

	void InitTexture();

	void DrawTexture(float SystemTime);
};
