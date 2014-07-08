// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GASpawnTrigger.h"
#include "GASpawnDestructible.h"
#include "GAEnemySpawnLocation.h"
#include "GAWaves.h"
#include "GAEnemySpawn.generated.h"


/**
 * 
 */

UCLASS()
class AGAEnemySpawn : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()	FWave CurrentWave;
	float SpawnTimer;

	void IncreaseSpawnTimer(float DeltaTime);
	void SpawnCurrentWave();
	void SpawnEnemy(TSubclassOf<class AActor> EnemyClass);
	void SetCurrentWave(FWave NewWave);
	virtual void Tick(float Delta) OVERRIDE;
};