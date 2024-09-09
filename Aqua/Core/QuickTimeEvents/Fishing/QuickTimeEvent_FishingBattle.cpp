// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickTimeEvent_FishingBattle.h"

#include "EnhancedInputComponent.h"
#include "FishingRing.h"
#include "Aqua/Core/Player/AquaPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"

bool UQuickTimeEvent_FishingBattle::StartQTE()
{
	if(Super::StartQTE())
	{
		//parent successfully started QTE
		const auto Player = Cast<AAquaPlayerCharacter>(GetOwner());
		if(Player == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("[UQuickTimeEvent_FishingBattle::StartQTE]:\tFailed to cast owner actor to AAquaPlayerCharacter\n"));
			return false;
		}
		
		if(const auto PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			const auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PC->InputComponent);
			const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
			if(Subsystem && EnhancedInputComponent)
			{
				Subsystem->RemoveMappingContext(Player->GetDefaultMappingContext());
				Subsystem->AddMappingContext(FishingMappingContext, 100);
				EnhancedInputComponent->BindAction(YankAction, ETriggerEvent::Triggered, this, &UQuickTimeEvent_FishingBattle::Yank);
			}
		}

		//FVector FishCastingLocation;
		//FVector CastingHalfDistance;
		GetCastingLocation();
		ConfigurePlayerCamera(Player);

		CatchCircle = SpawnFishingRing(FishingRingData, FishCastingLocation);
		FishCircle = SpawnFishingRing(CaptureRingData, FishCastingLocation + FVector(0.f, 0.f, 0.1f));

		ResetState();

		/** Debugging **/
		//DrawDebugSphere(GetWorld(), CastingHalfDistance, 20.f, 12, FColor::Red, false, 20.f, 0, 1.f);
		//DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), FishCastingLocation, FColor::Black, false, 20.f, 0, 1.f);

		//ReceiveStartQTE();

		return true;
	}

	return false;
}

bool UQuickTimeEvent_FishingBattle::EndQTE(const EQuickTimeEventEndType& EndType)
{
	if(Super::EndQTE(EndType))
	{
		const auto Player = Cast<AAquaPlayerCharacter>(GetOwner());
		if(Player == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("[UQuickTimeEvent_FishingBattle::EndQTE]:\tFailed to cast owner actor to AAquaPlayerCharacter\n"));
			return false;
		}
		
		if(const auto PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			if(const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FishingMappingContext);
				Subsystem->AddMappingContext(Player->GetDefaultMappingContext(), 0);
			}
		}

		DestroyFishingRing(CatchCircle);
		DestroyFishingRing(FishCircle);
		ResetState();
		ResetPlayerCamera(Player);

		//ReceiveEndQTE();
		return true;
	}
	return false;
}

void UQuickTimeEvent_FishingBattle::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(IsQTEActive() && IsValid(FishCircle) && IsValid(CatchCircle))
	{
		FishEnergy += EnergyRechargeRate * DeltaTime;
		LineTension -= TensionRelaxRate * DeltaTime;
		ClampTensionAndEnergy();
		DistanceBetweenRings = FishCircle->DistanceToPerfectOverlap(CatchCircle);
		FishCircle->SetRingColorByDistance(DistanceBetweenRings, CatchCircle->GetCenter() / 2.f);

		
		if(GEngine)
		{
			const FString msg = FString::Printf(TEXT("Current fishing stats:\n\tFish energy: %2.4f\n\tLine Tension: %2.4f\n\tCombo Multiplier: %d\n\tDistance between rings: %2.4f\n\t"), FishEnergy, LineTension, ComboMultiplier, DistanceBetweenRings);
			GEngine->AddOnScreenDebugMessage(12345, 1.f, FColor::Green, msg);
		}
	}
}


void UQuickTimeEvent_FishingBattle::Yank(const FInputActionValue& Value)
{
	//if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT("YANKED"));

	if(IsValid(FishCircle) && IsValid(CatchCircle))
	{

		//handle clicking with current ring
		const auto OverlapType = FishCircle->EvaluateOverlap(CatchCircle);

		if(const auto ContinueMinigame = UpdateTensionAndDistance(OverlapType))
		{
			//destroy and respawn new fishing ring
			DestroyFishingRing(FishCircle);
			FishCircle = SpawnFishingRing(CaptureRingData, FishCastingLocation + FVector(0.f, 0.f, 0.1f));
		}
	}
}

bool UQuickTimeEvent_FishingBattle::UpdateTensionAndDistance(EFishingRingOverlapType OverlapType)
{
	switch (OverlapType) {
	case NoOverlap_Early:
		FishEnergy -= BaseEnergyChange * EnergyMultiplier_EarlyNoOverlap;
		LineTension += BaseTensionChange * TensionMultiplier_EarlyNoOverlap;
		ComboMultiplier = 1;
		break;
	case NoOverlap_Late:
		FishEnergy += BaseEnergyChange * EnergyMultiplier_LateNoOverlap;
		LineTension -= BaseTensionChange * TensionMultiplier_LateNoOverlap;
		ComboMultiplier = 1;
		break;
	case InnerEdgeOverlap:
		FishEnergy -= BaseEnergyChange;
		LineTension += BaseTensionChange;
		break;
	case OuterEdgeOverlap:
		FishEnergy += BaseEnergyChange;
		LineTension -= BaseTensionChange;
		break;
	case FullOverlap:
		{
			FishEnergy -= BaseEnergyChange * EnergyMultiplier_FullOverlap * ComboMultiplier;
			const auto TensionDecrease = BaseTensionChange * TensionMultiplier_FullOverlap * ComboMultiplier;
			LineTension = FMath::Max(LineTension - TensionDecrease, 0.f);
			ComboMultiplier = FMath::Min(ComboMultiplier + 1, MaxComboMultiplier);
			break;
		}
	default: ;
	}

	ClampTensionAndEnergy();
	
	if(FishEnergy <= 0.f)
	{
		//Caught fish
		EndQTE(WonQTE);
		return false;
	}

	if(LineTension >= 100.f)
	{
		//Broke line
		EndQTE(LostQTE);
		return false;
	}

	return true;
}

/** Helpers **/
void UQuickTimeEvent_FishingBattle::ConfigurePlayerCamera(const AAquaPlayerCharacter* Player)
{
	auto* PlayerSpringArm = Player->GetCameraSpringArm();
	StoredSpringArmLocation = PlayerSpringArm->GetComponentLocation();
	StoredSpringArmLength = PlayerSpringArm->TargetArmLength;
	PlayerSpringArm->TargetArmLength = FishCastingDistance;
	PlayerSpringArm->SetWorldLocation(CastingHalfDistance);
	PlayerSpringArm->bUsePawnControlRotation = false;
}

void UQuickTimeEvent_FishingBattle::ResetPlayerCamera(const AAquaPlayerCharacter* Player) const
{
	auto* PlayerSpringArm = Player->GetCameraSpringArm();
	PlayerSpringArm->TargetArmLength = StoredSpringArmLength;
	PlayerSpringArm->SetWorldLocation(StoredSpringArmLocation);
	PlayerSpringArm->bUsePawnControlRotation = true;
}

void UQuickTimeEvent_FishingBattle::GetCastingLocation()
{
	const auto ActorLocation = GetOwner()->GetActorLocation();
	const auto RightVector = GetOwner()->GetActorRightVector();
	const auto RightLook = RightVector * FishCastingDistance;

	FishCastingLocation = ActorLocation + RightLook;
	CastingHalfDistance = ActorLocation + (RightLook / 2.f);
}

AFishingRing* UQuickTimeEvent_FishingBattle::SpawnFishingRing(UMovingRingDataAsset* RingData,
                                                              const FVector& SpawnLocation) const
{
	FTransform RingTransform;
	RingTransform.SetLocation(SpawnLocation);
	RingTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
	RingTransform.SetScale3D(FVector(1));

	FActorSpawnParameters RingParams;
	RingParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	RingParams.bDeferConstruction = true;

	//UE_LOG(LogTemp, Display, TEXT("[UQuickTimeEvent_FishingBattle::SpawnFishingRing]:\tRight before spawning Fishing Ring\n"));
	if(auto* SpawnedRing = GetWorld()->SpawnActor<AFishingRing>(RingGenericToSpawn, RingTransform, RingParams))
	{
		//UE_LOG(LogTemp, Display, TEXT("[UQuickTimeEvent_FishingBattle::SpawnFishingRing]:\tSpawned Fishing Ring\n"));
		SpawnedRing->RingConfiguration = RingData;
		SpawnedRing->DispatchBeginPlay();
		//UE_LOG(LogTemp, Display, TEXT("[UQuickTimeEvent_FishingBattle::SpawnFishingRing]:\tSet Fishing Ring data asset\n"));
		return SpawnedRing;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[UQuickTimeEvent_FishingBattle::SpawnFishingRing]:\tFailed to spawn fishing ring\n"));
	return nullptr;
}

void UQuickTimeEvent_FishingBattle::ClampTensionAndEnergy()
{
	FishEnergy = FMath::Clamp(FishEnergy, 0.f, MaxFishEnergy);
	LineTension = FMath::Clamp(LineTension, 0.f, 100.f);
}

void UQuickTimeEvent_FishingBattle::ResetState()
{
	FishEnergy = InitialFishEnergy;
	MaxFishEnergy = FishEnergy;
	LineTension = InitialLineTension;
	ComboMultiplier = 1;
}

void UQuickTimeEvent_FishingBattle::DestroyFishingRing(AFishingRing* Ring)
{
	if(Ring && IsValid(Ring))
	{
		Ring->Destroy();
		Ring = nullptr;
	}
}
