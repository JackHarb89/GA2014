// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"
#include "StrategyMiniMapCapture.h"
#include "GAGameState.generated.h"

/**
 * 
 */
UCLASS()
class AGAGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

	TWeakObjectPtr<class AStrategyMiniMapCapture> MiniMapCamera;
	FBox WorldBounds;

	void ReduceBaseHealth();
	void FinishGame();

private:
	int32 BaseHealthPoints;
};
