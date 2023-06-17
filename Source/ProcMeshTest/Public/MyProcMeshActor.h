// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProcMeshActor.generated.h"

class UProceduralMeshComponent;
class UMaterial;
class ASplineActor;

UCLASS()
class PROCMESHTEST_API AMyProcMeshActor : public AActor
{
	GENERATED_BODY()
	
protected:

	TObjectPtr<UProceduralMeshComponent> RootProcMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TObjectPtr<UMaterial> Material;

	UFUNCTION(CallInEditor, Category = "MeshGeneration")
	void DrawMesh(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	int SegmentCount = 32;
	
	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float Length = 100.0f;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	int VerticesPerPoint = 8;
	
	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float ThicknessStart = 10.0f;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float ThicknessEnd = 10.0f;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float Flattening = 200.0f;
	
	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	bool bDrawMesh = true;

	UPROPERTY(VisibleAnywhere, Category = "MeshGeneration")
	int TriangleCount = 0;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float BitangentRotation = 0.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<ASplineActor> Spline;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	AMyProcMeshActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

};
