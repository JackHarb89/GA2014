// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAGameMode.h"
#include "GAPlayerController.h"
#include "GAGameState.h"

AGAGameMode::AGAGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{	
	playerCount = 0;


	//static ConstructorHelpers::FObjectFinder<UBlueprint> HUDOb(TEXT("/Game/UI/Classes/GA_HUD_BP"));
	//HUDClass = (UClass*)HUDOb.Object->GeneratedClass;
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnOb(TEXT("/Game/Blueprints/Peddy/Characters/PlayerCharacter_Barbarian"));
	DefaultPawnClass = (UClass*)PlayerPawnOb.Object->GeneratedClass;
	PlayerControllerClass = AGAPlayerController::StaticClass();	
	GameStateClass = AGAGameState::StaticClass();
}

#pragma region Player Spawn

// Select Player Start Depending On Player Index
AActor* AGAGameMode::ChoosePlayerStart(AController* Player)
{
	APlayerStart* BestStart = PlayerStarts[playerCount];
	playerCount++;
	return BestStart;
}
#pragma endregion