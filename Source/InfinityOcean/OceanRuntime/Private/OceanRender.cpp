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
bool UOceanRender::ShouldDrawTexture()
{
	return OceanProfile_A && OceanProfile_B && DisplacementRT_A && DisplacementRT_B;
}

void UOceanRender::InitTexture()
{
	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(InitOceanTexture)
	(
		[FeatureLevel, this](FRHICommandListImmediate& RHICmdList)
		{
			FRHITexture* OutRHI_A = DisplacementRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			FRHITexture* OutRHI_B = DisplacementRT_B->TextureReference.TextureReferenceRHI->GetReferencedTexture();

			OceanTexture_A->Init(FeatureLevel, DisplacementRT_A->SizeX, OceanProfile_A->Parameters);
			OceanTexture_B->Init(FeatureLevel, DisplacementRT_B->SizeX, OceanProfile_B->Parameters);
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
			FRHITexture* OutRHI_A = DisplacementRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			FRHITexture* OutRHI_B = DisplacementRT_B->TextureReference.TextureReferenceRHI->GetReferencedTexture();

			OceanTexture_A->Draw(FeatureLevel, DisplacementRT_A->SizeX, SimulationTime, OceanProfile_A->Parameters, OutRHI_A, RHICmdList);
			OceanTexture_B->Draw(FeatureLevel, DisplacementRT_B->SizeX, SimulationTime, OceanProfile_B->Parameters, OutRHI_B, RHICmdList);
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
	if (ShouldDrawTexture()) {
		InitTexture();
	}
}

void UOceanRender::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (ShouldDrawTexture()) {
		DrawTexture(GetWorld()->GetTimeSeconds());
	}
}
PRAGMA_ENABLE_OPTIMIZATION