// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAGameState.h"
#include "GACharacter.h"


AGAGameState::AGAGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	MiniMapCamera = NULL;
	WavesSpawned = 0;
}

void AGAGameState::CheckDeatchCondition(){
	bool AllPlayerDead = true;

	for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		if (!ActorItr->HasDied) AllPlayerDead = false;
	}

	if (AllPlayerDead){
		for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			ActorItr->CharacterLostGame();
		}
	}
}

int32 AGAGameState::GetSpawnedWaves(){
	return WavesSpawned;
}


void AGAGameState::IncreaseSpawnedWaves(){
	WavesSpawned++;
}