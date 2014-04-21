// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GASpawnTrigger.h"
#include "GASpawnDestructible.h"
#include "GAEnemySpawn.generated.h"


/**
 * 
 */
USTRUCT()
struct FWave
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		int32 NumberEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		UBlueprint* SmallEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		UBlueprint* NormalEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		UBlueprint* BigEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		float SmallEnemyChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		float NormalEnemyChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		float BigEnemyChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		float SpawnInterval;


	TSubclassOf<class AActor> getSmallEnemy(){
		return (UClass*)SmallEnemy->GeneratedClass;
	}

	TSubclassOf<class AActor> getNormalEnemy(){
		return (UClass*)NormalEnemy->GeneratedClass;
	}

	TSubclassOf<class AActor> getBigEnemy(){
		return (UClass*)BigEnemy->GeneratedClass;
	}

	bool isWaveFinished(){
		if (NumberEnemy == 0) return true;
		return false;
	}
	FWave(){

	}
};

UCLASS()
class AGAEnemySpawn : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "General")	bool beTriggered;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "General")	AGASpawnTrigger* Trigger;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		TArray<FWave> waves;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		AGASpawnDestructible* Destructible;

	UPROPERTY(Replicated)															float Time;
	UPROPERTY(Replicated)															float WaveNumber;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasSpawnedEnemy)					bool HasSpawnedEnemy;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasFinishedWave)					bool HasFinishedWave;
		
	UPROPERTY(Replicated)															float SpawnInterval;
	UPROPERTY(Replicated)															bool AllowedToSpawn;
	UPROPERTY(Replicated)															bool isInit;

	// Network
	UFUNCTION(reliable, server, WithValidation)										void ServerSpawnEnemy(TSubclassOf<class AActor> enemyClass);
	UFUNCTION(reliable, server, WithValidation)										void ServerSpawnWave();
	UFUNCTION(reliable, server, WithValidation)										void ServerSetNextWaveStruct();
	UFUNCTION(reliable, server, WithValidation)										void ServerResetHasFinishedWave();
	UFUNCTION(reliable, server, WithValidation)										void ServerInitWave();
	UFUNCTION(reliable, server, WithValidation)										void ServerCheckTrigger();
	UFUNCTION(reliable, server, WithValidation)										void ServerDestroySpawn();

	UFUNCTION()																		void OnRep_HasSpawnedEnemy();
	UFUNCTION()																		void OnRep_HasFinishedWave();

	virtual void Tick(float DeltaTime) OVERRIDE;

	void InitWave();
	void CheckTrigger();
	void DestroySpawn();
	void SetNextWaveStruct();
	void SpawnWave();
	void SpawnEnemy(TSubclassOf<class AActor> enemyClass);
};