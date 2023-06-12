// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProcMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"
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

}

void AMyProcMeshActor::DrawMesh()
{
	RootProcMeshComponent->ClearAllMeshSections();

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FLinearColor> Colors;
	TArray<FVector2D> UV0;

	TArray<FVector> MidPoints;
	TArray<FVector> Derivates;

	int PointCount = SegmentCount + 1;

	//Generate MidPoints, points along the line; F(X) = Y^2 / 200
	for (int i = 0; i < PointCount; i++)
	{
		FVector Point;
		Point.Y = (i - PointCount / 2) * Length / PointCount;
		Point.Z = Point.Y * Point.Y / Flattening;
		MidPoints.Add(Point);
	}

	//Generate Derivates
	Derivates.Add(MidPoints[1] - MidPoints[0]);
	Derivates[0].Normalize();
	for (int i = 1; i < PointCount - 1; i++)
	{
		FVector DeltaForward = MidPoints[i + 1] - MidPoints[i];
		FVector DeltaBackward = MidPoints[i] - MidPoints[i - 1];
		FVector Derivate = (DeltaForward + DeltaBackward) / 2;
		Derivate.Normalize();
		Derivates.Add(Derivate);
	}
	Derivates.Add(MidPoints[PointCount - 1] - MidPoints[PointCount - 2]);
	Derivates[PointCount - 1].Normalize();

	//Generate vertices and their UV coordinates
	for (int i = 0; i < PointCount; i++)
	{
		//Creates BaseVector, an arbitrarilly chosen vector orthogonal to the derivative
		FVector BaseVector;
		BaseVector.X = Derivates[i].Y - Derivates[i].Z;
		BaseVector.Y = Derivates[i].Z - Derivates[i].X;
		BaseVector.Z = Derivates[i].X - Derivates[i].Y;
		BaseVector.Normalize();

		TArray<FVector> SegmentVertices;
		for (int j = 0; j < VerticesPerPoint; j++)
		{
			//Rotate BaseVector around the normal (the derivative) to create a circle of points
			FVector Vertex = MidPoints[i] + Thickness * BaseVector.RotateAngleAxis(360.0f * j / VerticesPerPoint, Derivates[i]);
			SegmentVertices.Add(Vertex);

			//Determine UV coordinate; X coordinate goes along the pipe; Y coordinate goes around.
			FVector2D UV;
			UV.X = (double)i / (double)PointCount;
			UV.Y = FMath::Abs((double)j - (double)VerticesPerPoint / 2.0) / ((double)VerticesPerPoint / 2.0);
			UV0.Add(UV);
		}
		while (SegmentVertices[0].Z < SegmentVertices[1].Z || SegmentVertices[0].Z < SegmentVertices.Last().Z)
		{
			FVector RemoveVertex = SegmentVertices[0];
			SegmentVertices.RemoveAt(0);
			SegmentVertices.Add(RemoveVertex);
		}

		Vertices.Append(SegmentVertices);

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
			UKismetSystemLibrary::DrawDebugPoint(this, BasePos + MidPoints[i], 5.0f, FLinearColor::Blue, 20.0f);
			if (i > 0)
				UKismetSystemLibrary::DrawDebugLine(this, BasePos + MidPoints[i - 1], BasePos + MidPoints[i], FLinearColor::Blue, 20.0f);

			UKismetSystemLibrary::DrawDebugLine(this, BasePos + MidPoints[i], BasePos + MidPoints[i] + Derivates[i] * 100.0f, FLinearColor::Red, 20.0f);
		}
		for (FVector Vertex : Vertices)
		{
			UKismetSystemLibrary::DrawDebugPoint(this, BasePos + Vertex, 5.0f, FLinearColor::Green, 20.0f);
		}
	}

	RootProcMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), UV0, Colors, TArray<FProcMeshTangent>(), true);
	RootProcMeshComponent->SetMaterial(0, Material);
}

