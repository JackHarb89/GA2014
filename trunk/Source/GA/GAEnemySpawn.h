// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GASpawnTrigger.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)		bool beTriggered;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = General)		AGASpawnTrigger* Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		TArray<FWave> waves;

	float Time;
	float WaveNumber;
	float SpawnInterval;
	bool AllowedToSpawn;
	bool isInit;

	virtual void Tick(float DeltaTime) OVERRIDE;

	void InitFirstWave();
	void SetSpawnInterval();
	void CheckTrigger();
	void SetNextWaveStruct();
	void SpawnWave();
	void SpawnEnemy(TSubclassOf<class AActor> enemyClass);
};