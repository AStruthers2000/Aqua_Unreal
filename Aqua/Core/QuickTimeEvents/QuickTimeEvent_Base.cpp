// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickTimeEvent_Base.h"

// Sets default values for this component's properties
UQuickTimeEvent_Base::UQuickTimeEvent_Base()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UQuickTimeEvent_Base::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UQuickTimeEvent_Base::StartQTE()
{
	if(!bIsQTEActive)
	{
		bIsQTEActive = true;
		
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString(TEXT("Starting QTE")));
		if(GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				QTETimerHandle,
				this,
				&UQuickTimeEvent_Base::OnQTETimerExpire,
				EventDuration,
				false);
		}
		ReceiveStartQTE();
		return true;
	}

	return false;
}

bool UQuickTimeEvent_Base::EndQTE(const EQuickTimeEventEndType& EndType)
{
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString(TEXT("Ending QTE")));
	QTETimerHandle.Invalidate();
	bIsQTEActive = false;
	ReceiveEndQTE(EndType);
	return true;
}


void UQuickTimeEvent_Base::OnQTETimerExpire()
{
	EndQTE(DefaultEndType);
}

// Called every frame
void UQuickTimeEvent_Base::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsQTEActive)
	{
		
	}
}

