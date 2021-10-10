#pragma once
#include "OceanMesh.h"
#include "CoreMinimal.h"
#include "OceanProfile.h"
#include "ObjectMacros.h"
#include "OceanRenderer.h"
#include "GameFramework/Actor.h"
#include "OceanActor.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), hideCategories = (Replication, Collision, Input, Actor, LOD, Cooking))
class OCEANRUNTIME_API AOceanActor : public AActor
{
	GENERATED_BODY()

public:
	float LodAlphaBlackPointFade;
	float LodAlphaBlackWhitePointFade;
	float ViewerAltitudeLevelAlpha;

	UPROPERTY()
	bool bInfiniteOcean = true;

	UPROPERTY()
	float MoveFollowDistance = 1.0f;

	UPROPERTY(EditAnywhere, Category = "MeshData")
	int NumLOD = 6;

	UPROPERTY(EditAnywhere, Category = "MeshData")
	int NumQuad = 32;

	UPROPERTY(EditAnywhere, Category = "MeshData")
	float MaxScaleHeight = 2048.0f;

	UPROPERTY(EditAnywhere, Category = "MeshData")
	UMaterialInterface* OceanMaterial;

	UPROPERTY()
	FOceanMeshBuilder OceanMeshBuilder;

	UPROPERTY()
	TArray<UOceanMesh*> OceanMeshComponents;

	UPROPERTY()
	TArray<USceneComponent*> OceanMeshLODGroups;

	UPROPERTY()
	FVector PreSetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "OceanComponent")
	UOceanRenderer* OceanRenderer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveData")
	UOceanProfile* Profile_A;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveData")
	UTextureRenderTarget2D* HeightRT_A;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveData")
	UTextureRenderTarget2D* NormalRT_A;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveData")
	UOceanProfile* Profile_B;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveData")
	UTextureRenderTarget2D* HeightRT_B;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveData")
	UTextureRenderTarget2D* NormalRT_B;

public:
	AOceanActor();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;

private:
	UFUNCTION(Category = "InfinityOcean")
	void UpdateOceanMaterial();

	USceneComponent* CreateLOD(int LodIndex, bool bBiggestLOD, const FOceanMeshBuilder& OceanMeshBulder);
};
