// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

class UStaticMeshComponent;
class UStaticMesh;

UCLASS()
class PROCMESHTEST_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(CallInEditor, Category = "Spline Generation")
	void SpawnSplinePoint();

	//Alpha is clamped between 0 and 1. 
	//Gets point along spline
	UFUNCTION()
	FVector GetSplinePoint(float Alpha);

	UFUNCTION(CallInEditor, Category = "Spline Generation")
	void TestDebug();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spline Generation")
	TArray<TObjectPtr<UStaticMeshComponent>> SplinePoints;

	UPROPERTY(EditAnywhere, Category = "Spline Generation")
	TObjectPtr<UStaticMesh> SphereMesh;

public:	
	// Sets default values for this actor's properties
	ASplineActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
