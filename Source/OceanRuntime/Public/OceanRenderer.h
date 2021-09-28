#pragma once

#include "CoreMinimal.h"
#include "OceanProfile.h"
#include "OceanTexture.h"
#include "OceanRenderer.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), hideCategories = (Tags, Activation, Cooking, AssetUserData, Collision))
class OCEANRUNTIME_API UOceanRenderer : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UOceanProfile* Profile_A;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UTextureRenderTarget2D* HeightRT_A;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UTextureRenderTarget2D* NormalRT_A;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UOceanProfile* Profile_B;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UTextureRenderTarget2D* HeightRT_B;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OceanInput")
	UTextureRenderTarget2D* NormalRT_B;


	UOceanRenderer();

	~UOceanRenderer();

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

	bool CanDraw();

	void InitTexture();

	void DrawTexture(float SystemTime);
};
