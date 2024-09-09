// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingRing.h"

#include "MovingRingDataAsset.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFishingRing::AFishingRing()
{
	//UE_LOG(LogTemp, Display, TEXT("[AFishingRing::AFishingRing]:\tFishing ring constructor called\n"));
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring Mesh"));
	RingMesh->SetCollisionProfileName("NoCollision");
	RingMesh->SetupAttachment(RootComponent);
	RootComponent = RingMesh;

	RingConfiguration = CreateDefaultSubobject<UMovingRingDataAsset>(TEXT("DefaultDataAsset"));
	RingConfiguration->InnerRadius = 0.45f;
	RingConfiguration->InitialRingScale = FVector(1);
	//RingConfiguration->OuterRadius = 0.5f;
	RingConfiguration->RingColor = FColor::White;
	RingConfiguration->GrowSpeed = 0.f;
	RingConfiguration->bGrowableRing = false;
	//UE_LOG(LogTemp, Display, TEXT("[AFishingRing::AFishingRing]:\tFishing ring constructor finished\n"));
}

// Called when the game starts or when spawned
void AFishingRing::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Display, TEXT("[AFishingRing::BeginPlay]:\tFishing ring beginplay called\n"));

	//if(!RingConfiguration)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("[AFishingRing::BeginPlay]:\tNo ring configuration found, creating default ring material."));
	//}

	

	const auto MI = Cast<UMaterialInstance>(RingMesh->GetMaterial(0));
	DynamicRingMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MI);
	if(!DynamicRingMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("[AFishingRing::BeginPlay]:\tFailed to create dynamic ring material."))
		return;
	}

	SetActorScale3D(RingConfiguration->InitialRingScale);

	RingMesh->SetMaterial(0, DynamicRingMaterial);
	UpdateOuterRadius(0.5f);
	UpdateInnerRadius(RingConfiguration->InnerRadius);
	UpdateColor(RingConfiguration->RingColor);

	bRingAllowedToGrow = RingConfiguration->bGrowableRing;
	RandomTimeOffset = FMath::RandRange(0.f, 10.f);
	TimeFromStartOfGame = RandomTimeOffset;
	//CurrentInnerRadius = RingConfiguration->InnerRadius;
}

// Called every frame
void AFishingRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeFromStartOfGame += DeltaTime;

	if(bRingAllowedToGrow)
	{
		float GrowRate;
		if(RingConfiguration->CustomGrowSpeed)
		{
			GrowRate = RingConfiguration->CustomGrowSpeed->GetFloatValue(TimeFromStartOfGame) * DeltaTime;
		}
		else
		{
			GrowRate = RingConfiguration->GrowSpeed * DeltaTime;
		}
		
		const auto NewScale = RingMesh->GetComponentScale() + GrowRate;
		
		if(UKismetMathLibrary::Vector_IsNearlyZero(NewScale) || NewScale.X < 0.f)
		{
			RingMesh->SetWorldScale3D(FVector(0.f, 0.f, 0.f));
			//bRingAllowedToGrow = false;
			return;
		}
		
		RingMesh->SetWorldScale3D(FVector(NewScale.X, NewScale.Y, 1.f));

		if(!UKismetMathLibrary::NearlyEqual_FloatFloat(RingConfiguration->InnerRadius, 0.f))
		{
			const float EdgeDistance = 0.5f - RingConfiguration->InnerRadius;
			const float ScaledDistance = (EdgeDistance / NewScale.X) * RingConfiguration->InitialRingScale.X;
			const float NewInnerRadius = FMath::Clamp(0.5f - ScaledDistance, 0.f, 0.5f);
			
			//if(GEngine) GEngine->AddOnScreenDebugMessage(100, 1, FColor::Green, FString::Printf(TEXT("Scaled inner radius: %2.2f"), NewInnerRadius));
			UpdateInnerRadius(NewInnerRadius);
		}
	}
}

void AFishingRing::UpdateInnerRadius(float NewInnerRadius)
{
	UpdateRadius("Inner Radius", NewInnerRadius);
	CurrentInnerRadius = NewInnerRadius;
	//RingConfiguration->InnerRadius = NewInnerRadius;
}

void AFishingRing::UpdateOuterRadius(float NewOuterRadius) const
{
	UpdateRadius("Outer Radius", NewOuterRadius);
}

void AFishingRing::UpdateColor(FLinearColor NewColor) const
{
	DynamicRingMaterial->SetVectorParameterValue("Color", NewColor);
}



float AFishingRing::GetInnerEdge() const
{
	return GetEdge(CurrentInnerRadius);
	//return GetEdge(RingConfiguration->InnerRadius);
}

float AFishingRing::GetOuterEdge() const
{
	return GetEdge(0.5f);
}

float AFishingRing::GetCenter() const
{
	const auto Radius = (GetOuterEdge() - GetInnerEdge()) / 2.f;
	const auto Center = GetOuterEdge() - Radius;
	return Center;
}

EFishingRingOverlapType AFishingRing::EvaluateOverlap(AFishingRing* OtherRing)
{
	const auto MyInner = GetInnerEdge();
	const auto MyOuter = GetOuterEdge();
	const auto OtherInner = OtherRing->GetInnerEdge();
	const auto OtherOuter = OtherRing->GetOuterEdge();

	//full overlap
	if(MyInner < OtherOuter && MyOuter > OtherInner)
	{
		if(MyOuter > OtherInner && MyInner < OtherInner)
		{
			//if(GEngine) GEngine->AddOnScreenDebugMessage(100, 5, FColor::Blue, TEXT("Inner edge overlap between rings"));
			return InnerEdgeOverlap;
		}

		if(MyOuter > OtherOuter && MyInner < OtherOuter)
		{
			//if(GEngine) GEngine->AddOnScreenDebugMessage(100, 5, FColor::Blue, TEXT("Outer edge overlap between rings"));
			return OuterEdgeOverlap;
		}

		//if(GEngine) GEngine->AddOnScreenDebugMessage(100, 5, FColor::Green, TEXT("Full overlap between rings"));
		return FullOverlap;
	}
	else
	{
		if(MyInner < OtherInner)
		{
			//if(GEngine) GEngine->AddOnScreenDebugMessage(100, 5, FColor::Red, TEXT("No overlap between rings. Clicked too early"));
		
			return NoOverlap_Early;
		}
		else
		{
			//if(GEngine) GEngine->AddOnScreenDebugMessage(100, 5, FColor::Red, TEXT("No overlap between rings. Clicked too late"));
			return NoOverlap_Late;
		}
	}
	//if(MyOuter > OtherInner && MyOuter < OtherOuter)
}

void AFishingRing::SetRingColorByDistance(float Distance, float MaxDistance) const
{
	//const auto GreenFloorFlipFlop = 
	const auto NormalizedDistance = FMath::GetMappedRangeValueClamped(\
		UE::Math::TVector2(0.f, MaxDistance),
		UE::Math::TVector2(0.f, 1.f),
		Distance - RingConfiguration->GreenFloorOffset);

	const auto NewColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Green, 1.f - NormalizedDistance);
	const auto msg = FString::Printf(TEXT("Color of ring: %fr, %fg, %fb\n"), NewColor.R, NewColor.G, NewColor.B);
	//if(GEngine) GEngine->AddOnScreenDebugMessage(212, 1, NewColor.ToFColor(false), msg);
	UpdateColor(NewColor);
}

float AFishingRing::DistanceToPerfectOverlap(const AFishingRing* OtherRing) const
{
	const auto Distance = FMath::Abs(GetCenter() - OtherRing->GetCenter());
	/*
	if(GEngine)
	{
		const auto msg = FString::Printf(TEXT("I am ring %s and my center is: %2.4f. The other ring has a center at: %2.4f. The distance between us is: %2.4f\n"), *GetActorNameOrLabel(), GetCenter(), OtherRing->GetCenter(), Distance);
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, msg);
	}
	*/
	return Distance;
}

void AFishingRing::UpdateRadius(FName ParamName, float NewRadius) const
{
	DynamicRingMaterial->SetScalarParameterValue(ParamName, NewRadius);
}

float AFishingRing::GetEdge(float Radius) const
{
	const auto RingScale = RingMesh->GetComponentScale();
	const auto WorldRadius = Radius * 100.f;
	const auto EdgeVector = WorldRadius * RingScale;
	
	check(EdgeVector.X == EdgeVector.Y);

	return EdgeVector.X;
}

