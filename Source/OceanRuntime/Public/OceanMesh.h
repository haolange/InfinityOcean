#pragma once
#include "CoreMinimal.h"
#include "PrimitiveSceneProxy.h"
#include "Components/MeshComponent.h"
#include "OceanMesh.generated.h"

PRAGMA_DISABLE_OPTIMIZATION
enum EOceanPatchType
{
	Interior,
	Fat,
	FatX,
	FatXSlimZ,
	FatXOuter,
	FatXZ,
	FatXZOuter,
	SlimX,
	SlimXZ,
	SlimXFatZ,
	Count,
};

USTRUCT(BlueprintType)
struct FOceanMeshBatch
{
	GENERATED_BODY()

public:
	FBox BoundBox;
	EOceanPatchType PatchType;
	TArray<uint32> IndexArray;
	TArray<FVector> VertexArray;
};

USTRUCT(BlueprintType)
struct FOceanMeshBuilder
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FOceanMeshBatch> OceanMeshBatchs;

public:
	void Build(int NumQuad, float MaxScaleHeight);

private:
	FOceanMeshBatch BuildOceanPatch(EOceanPatchType Patch, int NumQuad, float MaxScaleHeight);
};


//OceanMesh Component
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), hideCategories = (Physics, Navigation, VirtualTexture, MaterialParameters, HLOD, Mobile, Tags, Activation, Cooking, AssetUserData, Collision))
class OCEANRUNTIME_API UOceanMesh : public UMeshComponent
{
	GENERATED_BODY()
	
public:
	int LodIndex;
	int NumLOD;
	int NumQuad;
	float MaxScaleHeight;

	UPROPERTY()
	FOceanMeshBatch OceanMeshBatch;

public:
	UOceanMesh();

	~UOceanMesh();

	const FOceanMeshBatch& GetMeshData() const
	{
		return OceanMeshBatch;
	}

	void SetParameter(int InLodIndex, int InNumLOD, int InNumQuad, float InMaxScaleHeight, const FOceanMeshBatch& InOceanMeshBatch)
	{
		LodIndex = InLodIndex;
		NumLOD = InNumLOD;
		NumQuad = InNumQuad;
		MaxScaleHeight = InMaxScaleHeight;
		OceanMeshBatch = InOceanMeshBatch;
	}

	void SetOceanMaterial(UMaterialInterface* InOceanMaterial);

	void SetCustomPrimitiveData(const FVector& InOceanCenterWorldPos);

	virtual int32 GetNumMaterials() const override;

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;	
};

// OceanMesh IndexBuffer
class FOceanMeshIndexBuffer : public FIndexBuffer
{
public:
	TArray<uint32> Indices;

public:
	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(uint32), BUF_Static, CreateInfo);

		//Update Data
		void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(uint32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(uint32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

// OceanMesh VertexFactory
class FOceanMeshSceneProxy : public FPrimitiveSceneProxy
{
private:
	UPROPERTY()
	UMaterialInterface* Material;

	FLocalVertexFactory VertexFactory;
	FMaterialRelevance MaterialRelevance;
	FOceanMeshIndexBuffer IndexBuffer;
	FStaticMeshVertexBuffers VertexBuffers;

public:
	FOceanMeshSceneProxy(UOceanMesh* Component) : FPrimitiveSceneProxy(Component)
		, Material(NULL)
		, VertexFactory(GetScene().GetFeatureLevel(), "FOceanPatchMeshSceneProxy")
		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		const FColor VertexColor(255, 255, 255);

		TArray<FDynamicMeshVertex> Vertices;
		const int32 NumVertices = Component->OceanMeshBatch.VertexArray.Num();
		Vertices.AddUninitialized(NumVertices);
		IndexBuffer.Indices = Component->GetMeshData().IndexArray;

		for (int i = 0; i < NumVertices; i++)
		{
			FDynamicMeshVertex Vert;
			Vert.Color = VertexColor;

			const FVector TangentX = FVector::ForwardVector;
			const FVector TangentZ = FVector::UpVector;
			const FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

			Vert.Color = VertexColor;
			Vert.SetTangents(TangentX, TangentY, TangentZ);
			Vert.Position = Component->OceanMeshBatch.VertexArray[i];

			Vertices[i] = Vert;
		}
		VertexBuffers.InitFromDynamicVertex(&VertexFactory, Vertices);

		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexBuffers.PositionVertexBuffer);
		BeginInitResource(&VertexBuffers.StaticMeshVertexBuffer);
		BeginInitResource(&VertexBuffers.ColorVertexBuffer);
		BeginInitResource(&VertexFactory);

		Material = Component->GetMaterial(0);

		if (Material == NULL)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
	}

	virtual ~FOceanMeshSceneProxy()
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_OceanPatchMeshSceneProxy_GetDynamicMeshElements);

		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
			FLinearColor(0, 0.5f, 1.f)
		);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe) {
			MaterialProxy = WireframeMaterialInstance;
		}
		else {
			MaterialProxy = Material->GetRenderProxy();
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;

				bool bHasPrecomputedVolumetricLightmap;
				FMatrix PreviousLocalToWorld;
				int32 SingleCaptureIndex;
				bool bOutputVelocity;
				GetScene().GetPrimitiveUniformShaderParameters_RenderThread(GetPrimitiveSceneInfo(), bHasPrecomputedVolumetricLightmap, PreviousLocalToWorld, SingleCaptureIndex, bOutputVelocity);

				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		Result.bVelocityRelevance = IsMovable() && Result.bOpaque && Result.bRenderInMainPass;
		return Result;
	}

	virtual bool CanBeOccluded() const override 
	{ 
		return !MaterialRelevance.bDisableDepthTest; 
	}

	virtual uint32 GetMemoryFootprint(void) const override 
	{ 
		return(sizeof(*this) + GetAllocatedSize()); 
	}

	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	uint32 GetAllocatedSize(void) const 
	{ 
		return(FPrimitiveSceneProxy::GetAllocatedSize()); 
	}
};
PRAGMA_ENABLE_OPTIMIZATION