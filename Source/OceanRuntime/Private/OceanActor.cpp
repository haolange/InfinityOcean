#include "OceanActor.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BillboardComponent.h"

AOceanActor::AOceanActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("RootComponent"));
	OceanRenderer = CreateDefaultSubobject<UOceanRenderer>(TEXT("OceanRenderComponent"));
}

#if WITH_EDITOR
void AOceanActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AOceanActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	OceanRenderer->Profile_A = Profile_A;
	OceanRenderer->Profile_B = Profile_B;
	OceanRenderer->HeightRT_A = HeightRT_A;
	OceanRenderer->NormalRT_A = NormalRT_A;
	OceanRenderer->HeightRT_B = HeightRT_B;
	OceanRenderer->NormalRT_B = NormalRT_B;
}

PRAGMA_DISABLE_OPTIMIZATION
void AOceanActor::BeginPlay()
{
	Super::BeginPlay();

	OceanMeshLODGroups.Reset();
	OceanMeshComponents.Reset();

	int StartLevel = 0;
	OceanMeshBuilder.Build(NumQuad, MaxScaleHeight);
	for (int i = 0; i < NumLOD; i++)
	{
		USceneComponent* ParentNode = CreateLOD(i, i == NumLOD - 1, OceanMeshBuilder);
		OceanMeshLODGroups.Add(ParentNode);

		float HorizontalScale = FMath::Pow(2.0f, (float)(i + StartLevel)) * 100.0f;
		ParentNode->SetRelativeScale3D(FVector(HorizontalScale, HorizontalScale, 1.0f));
	}

	// SetMaterial
	for (auto& OceanMesh : OceanMeshComponents)
	{
		OceanMesh->SetOceanMaterial(OceanMaterial);
	}
	PreSetLocation = GetActorLocation();
}

void AOceanActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->HasBegunPlay())
	{
		FVector NeedSetLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();

		if (bInfiniteOcean)
		{
			if (FVector::DistSquaredXY(PreSetLocation, NeedSetLocation) > MoveFollowDistance * MoveFollowDistance)
			{
				FVector NewLocation = FVector(NeedSetLocation.X, NeedSetLocation.Y, GetActorLocation().Z);
				SetActorLocation(NewLocation);
				PreSetLocation = NewLocation;
			}
		}

		LodAlphaBlackPointFade = 0.4f / (NumQuad / 8.0f);
		LodAlphaBlackWhitePointFade = 1.0f - LodAlphaBlackPointFade - LodAlphaBlackPointFade;
		float ViewY = FMath::Abs((NeedSetLocation.Z - GetActorLocation().Z) * 0.01f);
		ViewY = FMath::Max(ViewY, 8.0f);
		ViewY = FMath::Min(ViewY, 1.99f * 256.0f);

		float L2 = FMath::Loge(ViewY) / FMath::Loge(2.0f);
		float L2F = FMath::FloorToFloat(L2);
		ViewerAltitudeLevelAlpha = L2 - L2F;

		float Scale = FMath::Pow(2.0f, L2F);
		RootComponent->SetWorldScale3D(FVector(Scale, Scale, 1.0f));
	}

	for (UOceanMesh* OceanMesh : OceanMeshComponents)
	{
		OceanMesh->SetCustomPrimitiveData(GetActorLocation());
	}
}

void AOceanActor::BeginDestroy()
{
	Super::BeginDestroy();
}

void AOceanActor::UpdateOceanMaterial()
{
	TArray<UActorComponent*> Results;
	GetComponents(UOceanMesh::StaticClass(), Results);

	for (auto& Component : Results)
	{
		UOceanMesh* OceanMesh = Cast<UOceanMesh>(Component);
		OceanMesh->SetOceanMaterial(OceanMaterial);
	}
}

USceneComponent* AOceanActor::CreateLOD(int LodIndex, bool bBiggestLOD, const FOceanMeshBuilder& OceanMeshBulder)
{
	USceneComponent* ParentRoot = NewObject<USceneComponent>(this, *FString::Printf(TEXT("Lod-%d"), LodIndex));
	AddOwnedComponent(ParentRoot);
	ParentRoot->RegisterComponent();
	ParentRoot->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ParentRoot->SetWorldLocationAndRotation(FVector::ZeroVector, FQuat::Identity);

	TArray<FVector2D> Offsets;
	TArray<EOceanPatchType> PatchTypes;

	bool generateSkirt = true && (bBiggestLOD && (LodIndex != 0));

	EOceanPatchType leadSideType = generateSkirt ? EOceanPatchType::FatXOuter : EOceanPatchType::SlimX;
	EOceanPatchType trailSideType = generateSkirt ? EOceanPatchType::FatXOuter : EOceanPatchType::FatX;
	EOceanPatchType leadCornerType = generateSkirt ? EOceanPatchType::FatXZOuter : EOceanPatchType::SlimXZ;
	EOceanPatchType trailCornerType = generateSkirt ? EOceanPatchType::FatXZOuter : EOceanPatchType::FatXZ;
	EOceanPatchType tlCornerType = generateSkirt ? EOceanPatchType::FatXZOuter : EOceanPatchType::SlimXFatZ;
	EOceanPatchType brCornerType = generateSkirt ? EOceanPatchType::FatXZOuter : EOceanPatchType::FatXSlimZ;

#define M_AddPatch(p) PatchTypes.Add(p);
#define M_AddOffset(x,y) Offsets.Add(FVector2D(x,y));

	if (LodIndex != 0)
	{
		M_AddOffset(-1.5f, 1.5f) M_AddOffset(-0.5f, 1.5f) M_AddOffset(0.5f, 1.5f)  M_AddOffset(1.5f, 1.5f)
		M_AddOffset(-1.5f, 0.5f)												   M_AddOffset(1.5f, 0.5f)
		M_AddOffset(-1.5f, -0.5f)												   M_AddOffset(1.5f, -0.5f)
		M_AddOffset(-1.5f, -1.5f) M_AddOffset(-0.5f, -1.5f) M_AddOffset(0.5f, -1.5f)  M_AddOffset(1.5f, -1.5f)

		M_AddPatch(tlCornerType)	M_AddPatch(leadSideType)	M_AddPatch(leadSideType)	M_AddPatch(leadCornerType)
		M_AddPatch(trailSideType)															M_AddPatch(leadSideType)
		M_AddPatch(trailSideType)															M_AddPatch(leadSideType)
		M_AddPatch(trailCornerType)	M_AddPatch(trailSideType)	M_AddPatch(trailSideType)	M_AddPatch(brCornerType)
	} else {
		M_AddOffset(-1.5f, 1.5f) M_AddOffset(-0.5f, 1.5f)  M_AddOffset(0.5f, 1.5f)  M_AddOffset(1.5f, 1.5f)
		M_AddOffset(-1.5f, 0.5f) M_AddOffset(-0.5f, 0.5f)  M_AddOffset(0.5f, 0.5f)  M_AddOffset(1.5f, 0.5f)
		M_AddOffset(-1.5f, -0.5f) M_AddOffset(-0.5f, -0.5f) M_AddOffset(0.5f, -0.5f) M_AddOffset(1.5f, -0.5f)
		M_AddOffset(-1.5f, -1.5f) M_AddOffset(-0.5f, -1.5f) M_AddOffset(0.5f, -1.5f) M_AddOffset(1.5f, -1.5f)

		EOceanPatchType InnerType = EOceanPatchType::Interior;
		M_AddPatch(tlCornerType)	M_AddPatch(leadSideType)	M_AddPatch(leadSideType)	M_AddPatch(leadCornerType)
		M_AddPatch(trailSideType)	M_AddPatch(InnerType)		M_AddPatch(InnerType)		M_AddPatch(leadSideType)
		M_AddPatch(trailSideType)	M_AddPatch(InnerType)		M_AddPatch(InnerType)		M_AddPatch(leadSideType)
		M_AddPatch(trailCornerType)	M_AddPatch(trailSideType)	M_AddPatch(trailSideType)	M_AddPatch(brCornerType)
	}

	// create the ocean patches
	for (int i = 0; i < Offsets.Num(); i++)
	{
		FVector2D Pos = Offsets[i];
		//UOceanMesh* OceanMeshComponent = CreateDefaultSubobject<UOceanMesh>(*FString::Printf(TEXT("OceanMesh%dx%d"), LodIndex, i));
		UOceanMesh* OceanMeshComponent = NewObject<UOceanMesh>(this, *FString::Printf(TEXT("OceanMesh%dx%d"), LodIndex, i));
		AddOwnedComponent(OceanMeshComponent);
		OceanMeshComponent->RegisterComponent();
		OceanMeshComponent->AttachToComponent(ParentRoot, FAttachmentTransformRules::KeepRelativeTransform);
		OceanMeshComponent->SetRelativeLocation(FVector(Pos.X, Pos.Y, 0.0f));
		OceanMeshComponent->SetRelativeScale3D(FVector::OneVector);
		OceanMeshComponents.Add(OceanMeshComponent);

		//bool rotateXOutwards = PatchTypes[i] == EOceanPatchType::FatXOuter;
		bool rotateXOutwards = PatchTypes[i] == EOceanPatchType::FatX || PatchTypes[i] == EOceanPatchType::FatXOuter || PatchTypes[i] == EOceanPatchType::SlimX || PatchTypes[i] == EOceanPatchType::SlimXFatZ;

		if (rotateXOutwards)
		{
			if (FMath::Abs(Pos.Y) >= FMath::Abs(Pos.X))
			{
				OceanMeshComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector::UpVector * 90.0f * FMath::Sign(Pos.Y)));
			} else {
				OceanMeshComponent->SetRelativeRotation(FQuat::MakeFromEuler(Pos.X < 0.0f ? FVector::UpVector * 180.0f : FVector::ZeroVector));
			}
		}

		//bool rotateXZOutwards = PatchTypes[i] == EOceanPatchType::FatXZOuter;
		bool rotateXZOutwards = PatchTypes[i] == EOceanPatchType::FatXZ || PatchTypes[i] == EOceanPatchType::SlimXZ || PatchTypes[i] == EOceanPatchType::FatXSlimZ || PatchTypes[i] == EOceanPatchType::FatXZOuter;

		if (rotateXZOutwards)
		{
			FVector from = FVector(1.0f, 1.0f, 0.0f).GetSafeNormal();
			FVector to = OceanMeshComponent->GetRelativeLocation().GetSafeNormal();
			FVector LocalPosition = OceanMeshComponent->GetRelativeLocation();
			bool bIgnoreRotateXY = false;
			if (FMath::Abs(LocalPosition.X) < 0.0001f || FMath::Abs(FMath::Abs(LocalPosition.X) - FMath::Abs(LocalPosition.Y)) > 0.001f)
			{
				bIgnoreRotateXY = true;
				continue;
			}

			if (!bIgnoreRotateXY)
			{
				if (FVector::DotProduct(from, to) < -0.99f) {
					OceanMeshComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector::UpVector * 180.0f));
				} else {
					OceanMeshComponent->SetRelativeRotation(FQuat::FindBetweenNormals(from, to));
					if (bBiggestLOD) {
						FVector Euler = OceanMeshComponent->GetRelativeRotation().Euler();
					}
				}
			}
		}

		OceanMeshComponent->SetParameter(LodIndex, NumLOD, NumQuad, MaxScaleHeight, OceanMeshBuilder.OceanMeshBatchs[PatchTypes[i]]);
	}
	return ParentRoot;
}
PRAGMA_ENABLE_OPTIMIZATION
