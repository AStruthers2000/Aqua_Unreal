// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Aqua/Core/QuickTimeEvents/QuickTimeEvent_Base.h"
#include "GameFramework/Character.h"
#include "AquaPlayerCharacter.generated.h"

class UQuickTimeEvent_Base;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;



UCLASS(config=Game)
class AQUA_API AAquaPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;
	//TSoftClassPtr<USpringArmComponent> SpringArm; 

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FishAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = QuickTimeEvents, meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<EQuickTimeEventType>, TSubclassOf<UQuickTimeEvent_Base>> PossibleQuickTimeEvents;

public:
	// Sets default values for this character's properties
	AAquaPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE auto GetCameraSpringArm() const { return SpringArm; }
	FORCEINLINE auto GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE auto GetDefaultMappingContext() const { return DefaultMappingContext; }

private:
	void BeginFishing(const FInputActionValue& value);

	TMap<TEnumAsByte<EQuickTimeEventType>, UQuickTimeEvent_Base*> QuickTimeEventInstances;
};
