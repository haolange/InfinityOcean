#pragma once

#include "OceanRender.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProceduralMeshComponent.h"
#include "OceanUtillity.generated.h"

UCLASS()
class OCEANRUNTIME_API UOceanUtillity : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "OceanUtillity")
	static void GeneratePlaneMesh(bool CenterPivot, bool FlipEdge, int NumQuad, float GridSize, FVector OffsetPosition, TArray<int32>& IndexArray, TArray<FVector2D>& UVArray, TArray<FVector>& VertexArray);
};
