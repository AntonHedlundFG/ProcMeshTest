// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProcMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SplineActor.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AMyProcMeshActor::AMyProcMeshActor()
{
	RootProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RootMeshComponent"));
	SetRootComponent(RootProcMeshComponent);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void AMyProcMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyProcMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawMesh(DeltaTime);
}

void AMyProcMeshActor::DrawMesh(float DeltaTime)
{
	RootProcMeshComponent->ClearAllMeshSections();

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FLinearColor> Colors;
	TArray<FVector2D> UV0;

	TArray<FVector> MidPoints;
	TArray<FVector> Tangents;

	int PointCount = SegmentCount + 1;

	if (!Spline) return;
	if (!bDrawMesh && !bDrawDebug) return;

	//Generate points along the spline
	for (int i = 0; i < PointCount; i++)
	{
		float a = (float)i / ((float)(PointCount-1));
		MidPoints.Add(Spline->GetSplinePoint(a));
	}

	//Generate Tangents using simplified central difference. Edge cases use forward/backward difference
	Tangents.Add(MidPoints[1] - MidPoints[0]);
	Tangents[0].Normalize();
	for (int i = 1; i < PointCount - 1; i++)
	{
		FVector DeltaForward = MidPoints[i + 1] - MidPoints[i];
		FVector DeltaBackward = MidPoints[i] - MidPoints[i - 1];
		FVector Tangent = (DeltaForward + DeltaBackward) / 2;
		Tangent.Normalize();
		Tangents.Add(Tangent);
	}
	Tangents.Add(MidPoints[PointCount - 1] - MidPoints[PointCount - 2]);
	Tangents[PointCount - 1].Normalize();
	
	//Generate the first bitangent vector and segment vertices. 
	//Note that the first bitangent is selected arbitrarilly. 
	//BitangentRotation variable should lets user manually select start point.
	FVector BaseBitangent;
	TArray<FVector> SegmentVertices;
	BaseBitangent.X = Tangents[0].Y - Tangents[0].Z;
	BaseBitangent.Y = Tangents[0].Z - Tangents[0].X;
	BaseBitangent.Z = Tangents[0].X - Tangents[0].Y;
	BaseBitangent.Normalize();
	BaseBitangent = BaseBitangent.RotateAngleAxis(BitangentRotation, Tangents[0]);
	for (int i = 0; i < VerticesPerPoint; i++)
	{
		FVector Vertex = MidPoints[0] + ThicknessStart * BaseBitangent.RotateAngleAxis(360.0f * i / VerticesPerPoint, Tangents[0]);
		SegmentVertices.Add(Vertex);
	}
	Vertices.Append(SegmentVertices);

	//Generate the remaining segment vertices by projecting the previous bitangent onto the current tangents normal plane.
	for (int i = 1; i < PointCount; i++)
	{
		SegmentVertices.Empty();
		FVector NextBitangent = Tangents[i].Cross(BaseBitangent.Cross(Tangents[i]));
		NextBitangent.Normalize();
		BaseBitangent = NextBitangent;
		for (int j = 0; j < VerticesPerPoint; j++)
		{
			FVector Vertex = MidPoints[i] + FMath::Lerp(ThicknessStart, ThicknessEnd, (float)i / (float)PointCount) * BaseBitangent.RotateAngleAxis(360.0f * j / VerticesPerPoint, Tangents[i]);
			SegmentVertices.Add(Vertex);
		}
		Vertices.Append(SegmentVertices);
	}

	//Determine UV coordinate; X coordinate goes along the pipe; Y coordinate goes around.
	for (int i = 0; i < PointCount; i++)
	{
		for (int j = 0; j < VerticesPerPoint; j++)
		{
			FVector2D UV;
			UV.X = (double)i / (double)PointCount;
			UV.Y = FMath::Abs((double)j - (double)VerticesPerPoint / 2.0) / ((double)VerticesPerPoint / 2.0);
			UV0.Add(UV);
		}
	}

	//Generate triangles
	for (int i = 0; i < PointCount - 1; i++)
	{
		for (int j = 0; j < VerticesPerPoint; j++)
		{
			int FirstVertex = i * VerticesPerPoint + j;
			int SecondVertex = FirstVertex + 1;
			int ThirdVertex = FirstVertex + VerticesPerPoint;
			int FourthVertex = ThirdVertex + 1;
			if (j == VerticesPerPoint - 1)
			{
				SecondVertex -= VerticesPerPoint;
				FourthVertex -= VerticesPerPoint;
			}
			Triangles.Add(FirstVertex);
			Triangles.Add(ThirdVertex);
			Triangles.Add(SecondVertex);
			Triangles.Add(SecondVertex);
			Triangles.Add(ThirdVertex);
			Triangles.Add(FourthVertex);
		}
	}
	TriangleCount = Triangles.Num() / 3;

	if (bDrawDebug)
	{
		FVector BasePos = GetActorLocation();
		for (int i = 0; i < PointCount; i++)
		{
			UKismetSystemLibrary::DrawDebugPoint(this, BasePos + MidPoints[i], 5.0f, FLinearColor::Blue, DeltaTime * 2.0f);
			if (i > 0)
				UKismetSystemLibrary::DrawDebugLine(this, BasePos + MidPoints[i - 1], BasePos + MidPoints[i], FLinearColor::Blue, DeltaTime * 2.0f);

			UKismetSystemLibrary::DrawDebugLine(this, BasePos + MidPoints[i], BasePos + MidPoints[i] + Tangents[i] * 100.0f, FLinearColor::Red, DeltaTime * 2.0f);
		}
		for (FVector Vertex : Vertices)
		{
			UKismetSystemLibrary::DrawDebugPoint(this, BasePos + Vertex, 5.0f, FLinearColor::Green, DeltaTime * 2.0f);
		}
	}
	if (bDrawMesh) 
	{
		RootProcMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), UV0, Colors, TArray<FProcMeshTangent>(), true);
		RootProcMeshComponent->SetMaterial(0, Material);
	}
}

