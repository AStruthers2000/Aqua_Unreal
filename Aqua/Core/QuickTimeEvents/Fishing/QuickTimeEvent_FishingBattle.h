// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
//#include "FishingRing.h"
//#include "QuickTimeEvent_FishingBattle.h"
#include "../QuickTimeEvent_Base.h"
#include "QuickTimeEvent_FishingBattle.generated.h"

enum EFishingRingOverlapType : int;
class AFishingRing;
class UInputAction;
struct FInputActionValue;
class UMovingRingDataAsset;
class AAquaPlayerCharacter;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class AQUA_API UQuickTimeEvent_FishingBattle : public UQuickTimeEvent_Base
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* FishingMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* YankAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	float FishCastingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AFishingRing> RingGenericToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	UMovingRingDataAsset* FishingRingData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	UMovingRingDataAsset* CaptureRingData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|FishStats", meta = (AllowPrivateAccess = "true"))
	float InitialLineTension = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|FishStats", meta = (AllowPrivateAccess = "true"))
	float InitialFishEnergy = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats", meta = (AllowPrivateAccess = "true"))
	float PerfectMultiplier = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats", meta = (AllowPrivateAccess = "true"))
	int32 MaxComboMultiplier = 10;
	
	/*
	 * Base tension amount applied to the fishing line. This base amount is applied
	 * during the InnerEdgeOverlap and OuterEdgeOverlap events, and serves as the base
	 * for the NoOverlap and FullOverlap special cases.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Tension", meta = (AllowPrivateAccess = "true"))
	float BaseTensionChange = 5.f;
	/*
	 * Represents the multiplier to the BaseTensionChange applied as an INCREASE in tension during a NoOverlap_Early event.
	 * Expected values >= 0. Negative values will DECREASE tension during a NoOverlap_Early event.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Tension", meta = (AllowPrivateAccess = "true", DisplayName = "Early Yank Tension Multiplier"))
	float TensionMultiplier_EarlyNoOverlap = 4.f;
	/*
	 * Represents the multiplier to the BaseTensionChange applied as a DECREASE in tension during a NoOverlap_Late event.
	 * Expected values >= 0. Negative values will INCREASE tension during a NoOverlap_Late event.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Tension", meta = (AllowPrivateAccess = "true", DisplayName = "Late Yank Tension Multiplier"))
	float TensionMultiplier_LateNoOverlap = 2.f;
	/*
	 * Represents the multiplier to the BaseTensionChange applied as a DECREASE in tension during a FullOverlap event.
	 * Expected values >= 0. Negative values will INCREASE tension during a FullOverlap event.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Tension", meta = (AllowPrivateAccess = "true", DisplayName = "Great Yank Tension Multiplier"))
	float TensionMultiplier_FullOverlap = 2.f;
	/*
	 * Rate in which the line loses tension. Rate is a function of time (aka this will be multiplied by deltaTime).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Tension", meta = (AllowPrivateAccess = "true"))
	float TensionRelaxRate = 20.f;

	/*
	 * Base energy amount applied to the fishing line. This base amount is applied
	 * during the InnerEdgeOverlap and OuterEdgeOverlap events, and serves as the base
	 * for the NoOverlap and FullOverlap special cases.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Energy", meta = (AllowPrivateAccess = "true"))
	float BaseEnergyChange = 10.f;
	/*
	 * Represents the multiplier to the BaseEnergyChange applied as an DECREASE in energy during a NoOverlap_Early event.
	 * Expected values >= 0. Negative values will INCREASE energy during a NoOverlap_Early event.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Energy", meta = (AllowPrivateAccess = "true", DisplayName = "Early Yank Energy Multiplier"))
	float EnergyMultiplier_EarlyNoOverlap = 1.5f;
	/*
	 * Represents the multiplier to the BaseEnergyChange applied as an INCREASE in energy during a NoOverlap_Late event.
	 * Expected values >= 0. Negative values will DECREASE energy during a NoOverlap_Late event.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Energy", meta = (AllowPrivateAccess = "true", DisplayName = "Late Yank Energy Multiplier"))
	float EnergyMultiplier_LateNoOverlap = 1.5f;
	/*
	 * Represents the multiplier to the BaseEnergyChange applied as an DECREASE in energy during a FullOverlap event.
	 * Expected values >= 0. Negative values will INCREASE energy during a FullOverlap event.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Energy", meta = (AllowPrivateAccess = "true", DisplayName = "Great Yank Energy Multiplier"))
	float EnergyMultiplier_FullOverlap = 2.f;
	/*
	 * Rate in which the fish regains energy. Rate is a function of time (aka this will be multiplied by deltaTime).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishingMinigame|PoleStats|Energy", meta = (AllowPrivateAccess = "true"))
	float EnergyRechargeRate = 20.f;
	

public:
	virtual bool StartQTE() override;
	virtual bool EndQTE(const EQuickTimeEventEndType&EndType) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void GetCastingLocation();
	AFishingRing* SpawnFishingRing(UMovingRingDataAsset* RingData, const FVector& SpawnLocation) const;
	static void DestroyFishingRing(AFishingRing* Ring);
	UPROPERTY(BlueprintReadOnly, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	AFishingRing* CatchCircle;
	UPROPERTY(BlueprintReadOnly, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	AFishingRing* FishCircle;

	void ConfigurePlayerCamera(const AAquaPlayerCharacter* Player);
	void ResetPlayerCamera(const AAquaPlayerCharacter* Player) const;
	FVector StoredSpringArmLocation;
	float StoredSpringArmLength;

	UFUNCTION()
	void Yank(const FInputActionValue& Value);

	UFUNCTION()
	bool UpdateTensionAndDistance(EFishingRingOverlapType OverlapType);

	UFUNCTION()
	void ClampTensionAndEnergy();

	FVector FishCastingLocation;
	FVector CastingHalfDistance;

	UFUNCTION(BlueprintCallable, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	float GetFishEnergy() const { return FishEnergy; }

	UFUNCTION(BlueprintCallable, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	float GetMaxFishEnergy() const { return MaxFishEnergy; }

	UFUNCTION(BlueprintCallable, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	float GetLineTension() const { return LineTension; }

	UFUNCTION(BlueprintCallable, Category = FishingMinigame, meta = (AllowPrivateAccess = "true"))
	int32 GetComboMultiplier() const { return ComboMultiplier; }

	float DistanceBetweenRings;
	float FishEnergy;
	float MaxFishEnergy;
	float LineTension;
	int32 ComboMultiplier;

	void ResetState();
};
