// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProcMeshActor.generated.h"

class UProceduralMeshComponent;
class UMaterial;

UCLASS()
class PROCMESHTEST_API AMyProcMeshActor : public AActor
{
	GENERATED_BODY()
	
protected:

	TObjectPtr<UProceduralMeshComponent> RootProcMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TObjectPtr<UMaterial> Material;

	UFUNCTION(CallInEditor, Category = "MeshGeneration")
	void DrawMesh();

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	int SegmentCount = 32;
	
	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float Length = 100.0f;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	int VerticesPerPoint = 8;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float Thickness = 10.0f;

	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	float Flattening = 200.0f;
	
	UPROPERTY(EditAnywhere, Category = "MeshGeneration")
	bool bDrawDebug = false;

	UPROPERTY(VisibleAnywhere, Category = "MeshGeneration")
	int TriangleCount = 0;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	AMyProcMeshActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
