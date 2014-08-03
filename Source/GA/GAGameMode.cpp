// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAGameMode.h"
#include "GAPlayerController.h"
#include "GACharacter.h"
#include "GA_HUD.h"
#include "GAWeapon.h"
#include "GAGameState.h"

AGAGameMode::AGAGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{	
	bUseSeamlessTravel = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> HUD_Game(TEXT("/Game/UI/Classes/GA_HUD_BP"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnOb(TEXT("/Game/Blueprints/Niklas/PlayerCharakter_Niklas.PlayerCharakter_Niklas"));

	GameHud = (UClass*)HUD_Game.Object->GeneratedClass;

	HUDClass = GameHud;
	DefaultPawnClass = (UClass*)PlayerPawnOb.Object->GeneratedClass;
	PlayerControllerClass = AGAPlayerController::StaticClass();	
	GameStateClass = AGAGameState::StaticClass();


}

void AGAGameMode::PostSeamlessTravel(){
	Super::PostSeamlessTravel();

	for (TActorIterator<AGAWeapon> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		(*ActorItr)->Destroy();
	}

	for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		(*ActorItr)->RemappedWeaponAfterTravel();
	}
}