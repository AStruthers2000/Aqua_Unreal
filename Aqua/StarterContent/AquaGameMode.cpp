// Copyright Epic Games, Inc. All Rights Reserved.

#include "AquaGameMode.h"
#include "AquaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAquaGameMode::AAquaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
