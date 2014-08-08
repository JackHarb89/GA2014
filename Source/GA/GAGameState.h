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

	void CheckDeatchCondition();

	UFUNCTION(BlueprintCallable, Category = "Waves")
	void IncreaseSpawnedWaves();
	
	UFUNCTION(BlueprintCallable, Category = "Waves")
	int32 GetSpawnedWaves();

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = "Game")
	bool AllPlayerDead;
	
private:

	int32 WavesSpawned;
};
