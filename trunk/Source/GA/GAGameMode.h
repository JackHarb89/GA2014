// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GAGameMode.generated.h"

UCLASS(minimalapi)
class AGAGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	TSubclassOf<AHUD> HUDClassSeamlessTravel;
	

	/** Returns game session class to use */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const OVERRIDE;

	virtual void PostSeamlessTravel() OVERRIDE;
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) OVERRIDE;
};



