#include "OceanRenderer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UOceanRenderer::UOceanRenderer()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UOceanRenderer::~UOceanRenderer()
{
	OceanTexture_A = nullptr;
	OceanTexture_B = nullptr;
}

//PRAGMA_DISABLE_OPTIMIZATION
inline bool UOceanRenderer::CanSimulation()
{
	return Profile_A && Profile_B && HeightRT_A && NormalRT_A && HeightRT_B && NormalRT_B;
}

void UOceanRenderer::InitTexture()
{
	OceanTexture_A = NewObject<UOceanTexture>();
	OceanTexture_B = NewObject<UOceanTexture>();
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

void UOceanRenderer::DrawTexture(float SimulationTime)
{
	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(DrawOceanTexture)
	(
		[FeatureLevel, SimulationTime, this](FRHICommandListImmediate& RHICmdList)
		{
			FRHITexture* OutHeight_A = HeightRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			FRHITexture* OutNormal_A = NormalRT_A->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			OceanTexture_A->Draw(FeatureLevel, HeightRT_A->SizeX, SimulationTime, Profile_A->Parameters, OutHeight_A, OutNormal_A, RHICmdList);

			FRHITexture* OutHeight_B = HeightRT_B->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			FRHITexture* OutNormal_B = NormalRT_B->TextureReference.TextureReferenceRHI->GetReferencedTexture();
			OceanTexture_B->Draw(FeatureLevel, HeightRT_B->SizeX, SimulationTime, Profile_B->Parameters, OutHeight_B, OutNormal_B, RHICmdList);
		}
	);
}

#if WITH_EDITOR
void UOceanRenderer::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UOceanRenderer::BeginPlay()
{
	Super::BeginPlay();
	if (!CanSimulation()) { return; }

	InitTexture();
}

void UOceanRenderer::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);
	if (!CanSimulation()) { return; }

	DrawTexture(GetWorld()->GetTimeSeconds());
}
//PRAGMA_ENABLE_OPTIMIZATION