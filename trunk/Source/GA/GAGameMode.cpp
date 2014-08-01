// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAGameMode.h"
#include "GAPlayerController.h"
#include "GACharacter.h"
#include "GAWeapon.h"
#include "GAGameSession.h"
#include "GAGameState.h"

AGAGameMode::AGAGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{	
	bUseSeamlessTravel = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> HUDOb(TEXT("/Game/UI/Classes/GA_HUD_BP"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnOb(TEXT("/Game/Blueprints/Peddy/Characters/PlayerCharacter_Barbarian"));

	HUDClass  = (UClass*)HUDOb.Object->GeneratedClass;
	DefaultPawnClass = (UClass*)PlayerPawnOb.Object->GeneratedClass;
	HUDClassSeamlessTravel = AHUD::StaticClass();
	PlayerControllerClass = AGAPlayerController::StaticClass();	
	GameStateClass = AGAGameState::StaticClass();
}

void AGAGameMode::PostSeamlessTravel(){
	Super::PostSeamlessTravel();
}


void AGAGameMode::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList){
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
}

/** Returns game session class to use */
TSubclassOf<AGameSession> AGAGameMode::GetGameSessionClass() const
{
	return AGAGameSession::StaticClass();
}
