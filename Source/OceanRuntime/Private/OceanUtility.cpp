#include "OceanUtility.h"

void UOceanUtility::GeneratePlaneMesh(bool CenterPivot, bool FlipEdge, int NumQuad, float GridSize, FVector OffsetPosition, TArray<int32>& IndexArray, TArray<FVector2D>& UVArray, TArray<FVector>& VertexArray)
{
	// VertexBuffer
	for (int32 j = 0; j < NumQuad + 1; j++)
	{
		float z = FMath::Lerp(-0.5f, 0.5f, j / (float)NumQuad);

		for (int32 i = 0; i < NumQuad + 1; i++)
		{
			float x = FMath::Lerp(-0.5f, 0.5f, i / (float)NumQuad);

			UVArray.Add( FVector2D(i / (float)NumQuad, j / (float)NumQuad) );

			if (CenterPivot) {
				VertexArray.Add(FVector(x, z, 0) * GridSize * 100 + OffsetPosition);
			} else {
				VertexArray.Add((FVector(x, z, 0) * GridSize * 100 + OffsetPosition) + FVector(((GridSize * 100) / 2), ((GridSize * 100) / 2), 0));
			}
		}
	}

	// IndexBuffer
	for (int32 j = 0; j < NumQuad; j++)
	{
		for (int32 i = 0; i < NumQuad; i++)
		{
			bool flipEdge = false;
			if (i % 2 == 1) flipEdge = !flipEdge;
			if (j % 2 == 1) flipEdge = !flipEdge;
			if (!FlipEdge) flipEdge = false;

			int i0 = i + j * (NumQuad + 1);
			int i1 = i0 + 1;
			int i2 = i0 + (NumQuad + 1);
			int i3 = i2 + 1;

			if (!flipEdge)
			{
				//Tri 1
				IndexArray.Add(i3);
				IndexArray.Add(i1);
				IndexArray.Add(i0);

				//Tri 2
				IndexArray.Add(i0);
				IndexArray.Add(i2);
				IndexArray.Add(i3);
			} else {
				//Tri 1
				IndexArray.Add(i3);
				IndexArray.Add(i1);
				IndexArray.Add(i2);

				//Tri 2
				IndexArray.Add(i0);
				IndexArray.Add(i2);
				IndexArray.Add(i1);
			}
		}
	}
}