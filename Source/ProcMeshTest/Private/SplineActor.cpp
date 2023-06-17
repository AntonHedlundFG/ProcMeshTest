// Fill out your copyright notice in the Description page of Project Settings.



#include "SplineActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASplineActor::SpawnSplinePoint()
{
	FName Name = FName(FString("Spline Point") + FString::FromInt(SplinePoints.Num()));
	UStaticMeshComponent* SpawnedComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), Name);
	SpawnedComponent->RegisterComponent();
	SpawnedComponent->AttachToComponent(RootComponent.Get(), FAttachmentTransformRules::KeepRelativeTransform);
	SpawnedComponent->CreationMethod = EComponentCreationMethod::Instance;
	if(SphereMesh)
		SpawnedComponent->SetStaticMesh(SphereMesh);
	SplinePoints.Add(SpawnedComponent);
	
	
}

FVector ASplineActor::GetSplinePoint(float Alpha)
{
	float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	FVector ResultVector = FVector::Zero();

	TArray<FVector> PreArray, PostArray;
	
	for (UStaticMeshComponent* Point : SplinePoints)
	{
		PreArray.Add(Point->GetRelativeLocation());
	}

	while (PreArray.Num() >= 2)
	{
		for (int i = 1; i < PreArray.Num(); i++)
		{
			PostArray.Add(Alpha * PreArray[i - 1] + (1.0f - Alpha) * PreArray[i]);
		}
		PreArray.Empty();
		PreArray.Append(PostArray);
		PostArray.Empty();
	}

	return PreArray[0];
}

void ASplineActor::TestDebug()
{
	for (float a = 0.0f; a <= 1.0f; a += 0.01f)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, GetSplinePoint(a) + GetActorLocation(), 20.0f, 12, FLinearColor::Red, 10.0f, 2.0f);
	}
}