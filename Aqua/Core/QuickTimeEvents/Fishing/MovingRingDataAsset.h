// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MovingRingDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class AQUA_API UMovingRingDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor RingColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector InitialRingScale = FVector(1);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InnerRadius;

	/*
	 * This value is subtracted from the distance between two rings, so that pure green color happens starting at +/- GreenFloorOffset away from the center of circle overlaps
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GreenFloorOffset;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//float OuterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bGrowableRing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bGrowableRing"))
	float GrowSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bGrowableRing"))
	UCurveFloat* CustomGrowSpeed;
	

	
};
