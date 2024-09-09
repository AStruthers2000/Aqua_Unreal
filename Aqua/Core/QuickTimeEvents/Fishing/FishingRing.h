// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishingRing.generated.h"

class UMovingRingDataAsset;

UENUM(Blueprintable, BlueprintType)
enum EFishingRingOverlapType
{
	NoOverlap_Early,
	NoOverlap_Late,
	InnerEdgeOverlap,
	OuterEdgeOverlap,
	FullOverlap,
};

UCLASS()
class AQUA_API AFishingRing : public AActor
{
	GENERATED_BODY()

	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn="true"))
	UMovingRingDataAsset* RingConfiguration;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* RingMesh;
	
	// Sets default values for this actor's properties
	AFishingRing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
	void UpdateInnerRadius(float NewInnerRadius);
	void UpdateColor(FLinearColor NewColor) const;

	UFUNCTION(BlueprintCallable)
	float GetInnerEdge() const;

	UFUNCTION(BlueprintCallable)
	float GetOuterEdge() const;

	UFUNCTION(BlueprintCallable)
	float GetCenter() const;

	UFUNCTION(BlueprintCallable)
	EFishingRingOverlapType EvaluateOverlap(AFishingRing* OtherRing);

	UFUNCTION()
	void SetRingColorByDistance(float Distance, float MaxDistance) const;

	UFUNCTION()
	float DistanceToPerfectOverlap(const AFishingRing* OtherRing) const;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicRingMaterial;

	void UpdateOuterRadius(float NewOuterRadius) const;
	void UpdateRadius(FName ParamName, float NewRadius) const;
	float GetEdge(float Radius) const;

	float CurrentInnerRadius = 0.f;
	bool bRingAllowedToGrow = false;

	float TimeFromStartOfGame = 0.f;
	float RandomTimeOffset = 0.f;
};

