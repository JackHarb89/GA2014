// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GAWaves.h"
#include "GAEnemySpawn.generated.h"


/**
 * 
 */

UCLASS()
class AGAEnemySpawn : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	void SetCurrentWave(FWave NewWave);
	void SetSpawnActivationStatusTo(bool State);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Light")				TArray<ALight*> SpawnLight;
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn Event")					void SpawnBecameActive();
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn Event")					void SpawnBecameInactive();

private:

	float SpawnTimer;
	bool IsNewWave;

	UPROPERTY()	FWave CurrentWave;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsSpawnActive)						bool IsSpawnActive;

	UFUNCTION()																			void OnRep_IsSpawnActive();

	void IncreaseSpawnTimer(float DeltaTime);
	void SpawnCurrentWave();
	void SpawnEnemy(TSubclassOf<class AActor> EnemyClass, int32 Points);
	virtual void Tick(float Delta) OVERRIDE;
};