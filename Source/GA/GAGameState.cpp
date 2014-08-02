// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAGameState.h"
#include "GACharacter.h"


AGAGameState::AGAGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	MiniMapCamera = NULL;
	BaseHealthPoints = 3;
}

void AGAGameState::ReduceBaseHealth(){
	BaseHealthPoints--;
	UE_LOG(LogClass, Log, TEXT("*** SERVER :: BASE TOOK DAMAGE (%d) ***"), BaseHealthPoints);

	if (BaseHealthPoints == 0){
		for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			ActorItr->CharacterLostGame();
		}
	}

}

void AGAGameState::FinishGame(){
	for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		ActorItr->CharacterWonGame();
	}
	UE_LOG(LogClass, Log, TEXT("*** SERVER :: HEART WAS DESTROYED ***"));
}


void AGAGameState::SetRemainingWaves(int32 Remaining){
	RemainingWaves = Remaining;
	UE_LOG(LogClass, Log, TEXT("*** SERVER :: %d REMAINING WAVES ***"), RemainingWaves);
}
int32 AGAGameState::GetRemainingWaves(){
	return RemainingWaves;
}


void AGAGameState::SetBaseHealthPoints(int32 Remaining){
	BaseHealthPoints = Remaining;
	UE_LOG(LogClass, Log, TEXT("*** SERVER :: %d BASE HEALTH POINTS ***"), BaseHealthPoints);
}

int32 AGAGameState::GetBaseHealthPoints(){
	return BaseHealthPoints;
}