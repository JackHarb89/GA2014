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
	UFUNCTION(BlueprintCallable, Category = Waves)
	void SetRemainingWaves(int32 Remaining);
	UFUNCTION(BlueprintCallable, Category = Waves)
	int32 GetRemainingWaves();
	UFUNCTION(BlueprintCallable, Category = Sections)
	void SetBaseHealthPoints(int32 HealthPoints);
	UFUNCTION(BlueprintCallable, Category = Sections)
	int32 GetBaseHealthPoints();

private:

	int32 BaseHealthPoints;
	int32 RemainingWaves;
};
