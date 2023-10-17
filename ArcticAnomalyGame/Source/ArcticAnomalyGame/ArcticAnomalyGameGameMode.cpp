// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArcticAnomalyGameGameMode.h"
#include "ArcticAnomalyGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AArcticAnomalyGameGameMode::AArcticAnomalyGameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
