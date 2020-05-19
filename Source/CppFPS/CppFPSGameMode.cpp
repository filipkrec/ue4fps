// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CppFPSGameMode.h"
#include "CppFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACppFPSGameMode::ACppFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/NewBlueprint.NewBlueprint"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
