// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Platformer3DGameMode.h"

// Engine libraries
#include "UObject/ConstructorHelpers.h"
// Custom libraries
#include "Characters/Platformer3DCharacter.h"

APlatformer3DGameMode::APlatformer3DGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
