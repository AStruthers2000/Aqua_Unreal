// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuickTimeEvent_Base.generated.h"

UENUM()
enum EQuickTimeEventType
{
	DefaultEventType = 0,
	Fishing
};

UENUM(BlueprintType)
enum EQuickTimeEventEndType
{
	DefaultEndType = 0,
	LostQTE,
	WonQTE
};

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class AQUA_API UQuickTimeEvent_Base : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuickTimeEvent_Base();
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = QuickTimeEventParameters, meta = (AllowPrivateAccess = "true"))
	float EventDuration;
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Start Quick Time Event"))
	void ReceiveStartQTE();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "End Quick Time Event"))
	void ReceiveEndQTE(const TEnumAsByte<EQuickTimeEventEndType>& EndType);


	UFUNCTION()
	void OnQTETimerExpire();

	UFUNCTION()
	bool IsQTEActive() const { return bIsQTEActive; }

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool StartQTE();
	virtual bool EndQTE(const EQuickTimeEventEndType& EndType = DefaultEndType);

private:
	FTimerHandle QTETimerHandle;
	bool bIsQTEActive = false;
};
