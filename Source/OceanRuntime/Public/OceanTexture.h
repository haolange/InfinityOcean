#pragma once

#include "CoreMinimal.h"
#include "RHIResources.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Containers/DynamicRHIResourceArray.h"

#include "OceanTexture.generated.h"

USTRUCT(BlueprintType)
struct FOceanParameterStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	float Period;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	float TimeScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	float Choppyness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	float Fraction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	float Aplitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	float WindDependency;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	float WindSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfinityOcean")
	FVector2D WindDir;

public:
	FOceanParameterStruct()
	{
		Period = 8;
		TimeScale = 0.05f;
		Choppyness = 1;
		Fraction = 0;
		Aplitude = 1;
		WindDependency = 0.9f;
		WindSpeed = 1;
		WindDir = FVector2D(-0.8f, 0.6f);
	}
};

struct FOceanGPUBuffer
{
public:
	FStructuredBufferRHIRef Buffer;

public:
	FOceanGPUBuffer() 
	{

	}

	void Initialize(uint32 BytesPerElement, uint32 NumElements) 
	{
		FRHIResourceCreateInfo CreateInfo;
		Buffer = RHICreateStructuredBuffer(BytesPerElement, BytesPerElement * NumElements, BUF_ShaderResource | BUF_UnorderedAccess, CreateInfo);
	}

	template <typename T>
	void Initialize(uint32 BytesPerElement, uint32 NumElements, TResourceArray<T>& BufferData) 
	{
		FRHIResourceCreateInfo CreateInfo(&BufferData);
		Buffer = RHICreateStructuredBuffer(BytesPerElement, BytesPerElement * NumElements, BUF_ShaderResource | BUF_UnorderedAccess , CreateInfo);
	}

	void Release() 
	{
		Buffer.SafeRelease();
	}
};

UCLASS(ClassGroup = InfinityOcean, DisplayName = "InfinityOceanContext")
class OCEANRUNTIME_API UOceanTexture : public UObject
{
	GENERATED_BODY()

private:
	const float TwoPi = 6.28318530718f;
	const float Gravity = 9.810f;
	const float SqrtHalf = 0.707106781186f;
	const float Euler = 2.71828182846f;

	TUniformBufferRef<FOceanUniform> OceanUniformRef;

	FOceanGPUBuffer GaussBuffer;
	FShaderResourceViewRHIRef GaussBuffer_SRV;

	FOceanGPUBuffer HZeroBuffer;
	FShaderResourceViewRHIRef HZeroBuffer_SRV;
	FUnorderedAccessViewRHIRef HZeroBuffer_UAV;

	FOceanGPUBuffer OmegaBuffer;
	FShaderResourceViewRHIRef OmegaBuffer_SRV;

	FOceanGPUBuffer HTBuffer;
	FShaderResourceViewRHIRef HTBuffer_SRV;
	FUnorderedAccessViewRHIRef HTBuffer_UAV;

	FOceanGPUBuffer DTBuffer;
	FShaderResourceViewRHIRef DTBuffer_SRV;
	FUnorderedAccessViewRHIRef DTBuffer_UAV;

	FTexture2DRHIRef Height_RT;
	FUnorderedAccessViewRHIRef Height_UAV;

	FTexture2DRHIRef Normal_RT;
	FUnorderedAccessViewRHIRef Normal_UAV;

public:
	UOceanTexture();

	~UOceanTexture();

	void Init(ERHIFeatureLevel::Type FeatureLevel, int32 Resolution, uint8 HeightFormat, uint8 NormalFormat, const FOceanParameterStruct& OceanParameters);

	void Draw(ERHIFeatureLevel::Type FeatureLevel, int32 Resolution, float SimulationTime, const FOceanParameterStruct& OceanParameters, FRHITexture* DscHeightTexture, FRHITexture* DscNormalTexture, FRHICommandListImmediate& CmdList);

private:
	float GaussValue();

	void InitGaussArray(int32 ArraySizeXY, TResourceArray<FVector2D>& GaussArrayRef);

	void InitOmegaArray(int32 ArraySizeXY, float Period, TResourceArray<float>& OmegaArrayRef);
};
