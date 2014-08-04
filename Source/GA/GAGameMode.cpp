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
	static ConstructorHelpers::FObjectFinder<UBlueprint> HUD_Menu(TEXT("/Game/UI/Classes/GA_MainMenu.GA_MainMenu"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> HUD_Transition(TEXT("/Game/UI/Classes/GA_TransitionHUD.GA_TransitionHUD"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnOb(TEXT("/Game/Blueprints/Niklas/PlayerCharakter_Niklas.PlayerCharakter_Niklas"));

	MainMenuHud = (UClass*)HUD_Menu.Object->GeneratedClass;
	GameHud = (UClass*)HUD_Game.Object->GeneratedClass;
	TransitionHud = (UClass*)HUD_Transition.Object->GeneratedClass;

	HUDClass = AGA_HUD::StaticClass();
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

void AGAGameMode::StartNewPlayer(APlayerController* NewPlayer){
	// tell client what hud class to use
	if (GetLevel()->OwningWorld->GetName().Contains("SG_Game")){
		NewPlayer->ClientSetHUD(GameHud);
	}
	else if (GetLevel()->OwningWorld->GetName().Contains("SG_TransitionMap")){
		NewPlayer->ClientSetHUD(TransitionHud);
	}
	else if (GetLevel()->OwningWorld->GetName().Contains("SG_MainMenu")){
		NewPlayer->ClientSetHUD(MainMenuHud);
	}

	// If players should start as spectators, leave them in the spectator state
	if (!bStartPlayersAsSpectators && !NewPlayer->PlayerState->bOnlySpectator)
	{
		// If match is in progress, start the player
		if (IsMatchInProgress())
		{
			RestartPlayer(NewPlayer);

			if (NewPlayer->GetPawn() != NULL)
			{
				NewPlayer->GetPawn()->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation());
			}
		}
		// Check to see if we should start right away, avoids a one frame lag in single player games
		else if (GetMatchState() == MatchState::WaitingToStart)
		{
			// Check to see if we should start the match
			if (ReadyToStartMatch())
			{
				StartMatch();
			}
		}
	}
}