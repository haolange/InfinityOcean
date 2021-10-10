#include "OceanMesh.h"
#include "OceanActor.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/RenderCore/Public/RenderResource.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Public/PrimitiveSceneProxy.h"
#include "Runtime/Engine/Public/StaticMeshResources.h"
#include "Runtime/Engine/Classes/Engine/CollisionProfile.h"

PRAGMA_DISABLE_OPTIMIZATION
// OceanMesh Builder
void FOceanMeshBuilder::Build(int NumQuad, float MaxScaleHeight)
{
	OceanMeshBatchs.Empty();
	float dx = 1.0f / NumQuad;

	for (int i = 0; i < EOceanPatchType::Count; ++i)
	{
		OceanMeshBatchs.Add(BuildOceanPatch((EOceanPatchType)i, NumQuad, MaxScaleHeight));
	}
}

FOceanMeshBatch FOceanMeshBuilder::BuildOceanPatch(EOceanPatchType Patch, int NumQuad, float MaxScaleHeight)
{
	FOceanMeshBatch OceanBatch;
	OceanBatch.PatchType = Patch;

	// stick a bunch of verts into a 1m x 1m patch (scaling happens later)
	float dx = 1.0f / NumQuad;

	//////////////////////////////////////////////////////////////////////////////////
	// verts

	// skirt widths on left, right, bottom and top (in order)
	float SkirtXminus = 0.0f, SkirtXplus = 0.0f;
	float SkirtYminus = 0.0f, SkirtYplus = 0.0f;

	// set the patch size
	if (Patch == EOceanPatchType::Fat) { SkirtXminus = SkirtXplus = SkirtYminus = SkirtYplus = 1.0f; }
	else if (Patch == EOceanPatchType::FatX || Patch == EOceanPatchType::FatXOuter) { SkirtXplus = 1.0f; }
	else if (Patch == EOceanPatchType::FatXZ || Patch == EOceanPatchType::FatXZOuter) { SkirtXplus = SkirtYplus = 1.0f; }
	else if (Patch == EOceanPatchType::FatXSlimZ) { SkirtXplus = 1.0f; SkirtYplus = -1.0f; }
	else if (Patch == EOceanPatchType::SlimX) { SkirtXplus = -1.0f; }
	else if (Patch == EOceanPatchType::SlimXZ) { SkirtXplus = SkirtYplus = -1.0f; }
	else if (Patch == EOceanPatchType::SlimXFatZ) { SkirtXplus = -1.0f; SkirtYplus = 1.0f; }

	// i choose not classify the patch,so just make the SkirtXplus = 0.0f
	/*SkirtXminus = 0.0f; SkirtXplus = 0.0f;
	SkirtYminus = 0.0f; SkirtYplus = 0.0f;*/

	float SideLength_Verts_X = 1.0f + NumQuad + SkirtXminus + SkirtXplus;
	float SideLength_Verts_Y = 1.0f + NumQuad + SkirtYminus + SkirtYplus;

	float Start_X = -0.5f - SkirtXminus * dx;
	float Start_Z = -0.5f - SkirtYminus * dx;
	float   End_X = 0.5f + SkirtXplus * dx;
	float   End_Z = 0.5f + SkirtYplus * dx;

	for (float j = 0; j < SideLength_Verts_Y; j++)
	{
		// interpolate z across patch

		float y = FMath::Lerp(Start_Z, End_Z, j / (SideLength_Verts_Y - 1.0f));

		// push outermost edge out to horizon
		if (Patch == EOceanPatchType::FatXZOuter && j == SideLength_Verts_Y - 1.0f)
			y *= 32.0f;//100.0f;

		for (float i = 0; i < SideLength_Verts_X; i++)
		{
			// interpolate x across patch
			float x = FMath::Lerp(Start_X, End_X, i / (SideLength_Verts_X - 1.0f));

			// push outermost edge out to horizon
			if (i == SideLength_Verts_X - 1.0f && (Patch == EOceanPatchType::FatXOuter || Patch == EOceanPatchType::FatXZOuter))
				x *= 32.0f;// 100.0f;

			// could store something in y, although keep in mind this is a shared mesh that is shared across multiple lods
			OceanBatch.VertexArray.Add(FVector(x, y, 0));
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	// indices

	int SideLength_Squares_X = (int)SideLength_Verts_X - 1;
	int SideLength_Squares_Y = (int)SideLength_Verts_Y - 1;

	for (int j = 0; j < SideLength_Squares_Y; j++)
	{
		for (int i = 0; i < SideLength_Squares_X; i++)
		{
			bool flipEdge = false;

			if (i % 2 == 1) flipEdge = !flipEdge;
			if (j % 2 == 1) flipEdge = !flipEdge;

			int i0 = i + j * (SideLength_Squares_X + 1);
			int i1 = i0 + 1;
			int i2 = i0 + (SideLength_Squares_X + 1);
			int i3 = i2 + 1;

			if (!flipEdge)
			{
				// Tri 1
				OceanBatch.IndexArray.Add(i3);
				OceanBatch.IndexArray.Add(i1);
				OceanBatch.IndexArray.Add(i0);

				// Tri 2
				OceanBatch.IndexArray.Add(i0);
				OceanBatch.IndexArray.Add(i2);
				OceanBatch.IndexArray.Add(i3);
			} else {
				// Tri 1
				OceanBatch.IndexArray.Add(i3);
				OceanBatch.IndexArray.Add(i1);
				OceanBatch.IndexArray.Add(i2);

				// Tri 2
				OceanBatch.IndexArray.Add(i0);
				OceanBatch.IndexArray.Add(i2);
				OceanBatch.IndexArray.Add(i1);
			}
		}
	}

	FBox MeshBox(ForceInit);
	// Compute Bounds
	for (int i = 0; i < OceanBatch.VertexArray.Num(); i++)
	{
		MeshBox += OceanBatch.VertexArray[i];
	}
	MeshBox.ExpandBy(FVector(dx, dx, 0));
	OceanBatch.BoundBox = FBox::BuildAABB(MeshBox.GetCenter(), FVector(MeshBox.GetExtent().X, MeshBox.GetExtent().Y, MaxScaleHeight));

	return OceanBatch;
}

// OceanMesh Component
UOceanMesh::UOceanMesh()
{
	PrimaryComponentTick.bCanEverTick = true;
	this->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
}

UOceanMesh::~UOceanMesh()
{

}

int32 UOceanMesh::GetNumMaterials() const
{
	return 1;
}

FPrimitiveSceneProxy* UOceanMesh::CreateSceneProxy()
{
	FPrimitiveSceneProxy* Proxy = NULL;
	if (OceanMeshBatch.VertexArray.Num() > 0)
	{
		Proxy = new FOceanMeshSceneProxy(this);
	}
	return Proxy;
}

void UOceanMesh::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOceanMesh::SetOceanMaterial(UMaterialInterface* InOceanMaterial)
{
	SetMaterial(0, InOceanMaterial);
}

void UOceanMesh::SetCustomPrimitiveData(const FVector& InOceanCenterWorldPos)
{
	AOceanActor* OceanActor = Cast<AOceanActor>(this->GetOwner());

	float MeshScaleLerp = LodIndex == 0 ? OceanActor->ViewerAltitudeLevelAlpha : 0.0f;
	float LodScale = FMath::Abs(this->GetComponentScale().X * 0.01f);
	float SquareSize = LodScale / NumQuad;
	FVector2D OceanCenterWorldPos = FVector2D(InOceanCenterWorldPos.X, InOceanCenterWorldPos.Y);
	float LodAlphaBlackPointFade = OceanActor->LodAlphaBlackPointFade;
	float LodAlphaBlackPointWhitePointFade = OceanActor->LodAlphaBlackWhitePointFade;
	SetCustomPrimitiveDataVector4(0, FVector4(MeshScaleLerp, NumQuad, LodScale, SquareSize));
	SetCustomPrimitiveDataVector4(4, FVector4(OceanCenterWorldPos.X, OceanCenterWorldPos.Y, LodAlphaBlackPointFade, LodAlphaBlackPointWhitePointFade));

	/*FVector4 LODColor;
	if (LodIndex == 0) {
		LODColor = FVector4(1, 1, 1, 1);
	} else if (LodIndex == 1) {
		LODColor = FVector4(1, 0, 0, 1);
	} else if (LodIndex == 2) {
		LODColor = FVector4(0, 1, 0, 1);
	} else if (LodIndex == 3) {
		LODColor = FVector4(0, 0, 1, 1);
	} else if (LodIndex == 4) {
		LODColor = FVector4(1, 0.5f, 0, 1);
	} else if (LodIndex == 5) {
		LODColor = FVector4(1, 0, 0.5f, 1);
	} else if (LodIndex == 6) {
		LODColor = FVector4(0.65f, 1, 0, 1);
	} else if (LodIndex == 7) {
		LODColor = FVector4(0, 1, 0.64f, 1);
	} else if (LodIndex == 8) {
		LODColor = FVector4(0.36f, 0, 1, 1);
	} else if (LodIndex == 9) {
		LODColor = FVector4(1, 0.22f, 0, 1);
	}
	SetCustomPrimitiveDataVector4(8, LODColor);*/
}

FBoxSphereBounds UOceanMesh::CalcBounds(const FTransform& LocalToWorld) const
{
	// Calculate bounding box of cable points
	FBox BoundingBox(ForceInit);

	FVector BoundsLocalPositions[8];
	FBox MeshBox = OceanMeshBatch.BoundBox;

	BoundsLocalPositions[0] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector( 1,  1,  1);
	BoundsLocalPositions[1] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector(-1,  1,  1);
	BoundsLocalPositions[2] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector( 1, -1,  1);
	BoundsLocalPositions[3] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector(-1, -1,  1);

	BoundsLocalPositions[4] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector( 1,  1, -1);
	BoundsLocalPositions[5] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector(-1,  1, -1);
	BoundsLocalPositions[6] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector( 1, -1, -1);
	BoundsLocalPositions[7] = MeshBox.GetCenter() + MeshBox.GetExtent() * FVector(-1, -1, -1);

	for (int32 PointIndex = 0; PointIndex < 8; PointIndex++)
	{
		BoundingBox += LocalToWorld.TransformPosition(BoundsLocalPositions[PointIndex]);
	}

	FBoxSphereBounds NewBounds;
	NewBounds.BoxExtent = BoundingBox.GetExtent();
	NewBounds.Origin = BoundingBox.GetCenter();
	NewBounds.SphereRadius = NewBounds.BoxExtent.Size();

	return NewBounds;
}
PRAGMA_ENABLE_OPTIMIZATION