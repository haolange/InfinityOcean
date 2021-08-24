#include "OceanRender.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UOceanRender::UOceanRender()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UOceanRender::~UOceanRender()
{
	OceanTexture_A = nullptr;
	OceanTexture_B = nullptr;
}

PRAGMA_DISABLE_OPTIMIZATION
bool UOceanRender::CanDraw()
{
	return Profile_A && Profile_B && HeightRT_A && NormalRT_A && HeightRT_B && NormalRT_B;
}

void UOceanRender::InitTexture()
{
	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(InitOceanTexture)
	(
		[FeatureLevel, this](FRHICommandListImmediate& RHICmdList)
		{
			uint8 HeightFormat = HeightRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture()->GetFormat();
			uint8 NormalFormat = NormalRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture()->GetFormat();

			OceanTexture_A->Init(FeatureLevel, HeightRT_A->SizeX, HeightFormat, NormalFormat, Profile_A->Parameters);
			OceanTexture_B->Init(FeatureLevel, HeightRT_B->SizeX, HeightFormat, NormalFormat, Profile_B->Parameters);
		}
	);
}

void UOceanRender::DrawTexture(float SimulationTime)
{
	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(DrawOceanTexture)
	(
		[FeatureLevel, SimulationTime, this](FRHICommandListImmediate& RHICmdList)
		{
			FRHITexture* OutHeight_A = HeightRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			FRHITexture* OutNormal_A = NormalRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			FRHITexture* OutHeight_B = HeightRT_B->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			FRHITexture* OutNormal_B = NormalRT_B->TextureReference.TextureReferenceRHI->GetReferencedTexture();

			OceanTexture_A->Draw(FeatureLevel, HeightRT_A->SizeX, SimulationTime, Profile_A->Parameters, OutHeight_A, OutNormal_A, RHICmdList);
			OceanTexture_B->Draw(FeatureLevel, HeightRT_B->SizeX, SimulationTime, Profile_B->Parameters, OutHeight_B, OutNormal_B, RHICmdList);
		}
	);
}

#if WITH_EDITOR
void UOceanRender::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UOceanRender::BeginPlay()
{
	Super::BeginPlay();

	OceanTexture_A = NewObject<UOceanTexture>();
	OceanTexture_B = NewObject<UOceanTexture>();

	if (CanDraw()) 
	{
		InitTexture();
	}
}

void UOceanRender::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (CanDraw()) 
	{
		DrawTexture(GetWorld()->GetTimeSeconds());
	}
}
PRAGMA_ENABLE_OPTIMIZATION