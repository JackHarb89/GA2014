// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GAEnemySpawn.generated.h"


/**
 * 
 */
USTRUCT()
struct FWave
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		UBlueprint* SmallEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		UBlueprint* NormalEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		UBlueprint* BigEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		int32 NumberSmallEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		int32 NumberNormalEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		int32 NumberBigEnemy;

	int32 getNumberEnemies(){
		return NumberSmallEnemy + NumberNormalEnemy + NumberBigEnemy;
	}

	TSubclassOf<class AActor> getSmallEnemy(){
		return (UClass*)SmallEnemy->GeneratedClass;
	}

	TSubclassOf<class AActor> getNormalEnemy(){
		return (UClass*)NormalEnemy->GeneratedClass;
	}

	TSubclassOf<class AActor> getBigEnemy(){
		return (UClass*)BigEnemy->GeneratedClass;
	}
};

UCLASS()
class AGAEnemySpawn : public AActor
{
	GENERATED_UCLASS_BODY()

	//UPROPERTY(EditAnywhere, Category = General)							TSubclassOf<class AActor> enemyClass;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)		int32 enemiesToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)		float SpawnOffset;
	UPROPERTY() float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waves)		TArray<FWave> waves;

	virtual void Tick(float DeltaTime) OVERRIDE;
	void SpawnEnemy(TSubclassOf<class AActor> enemyClass);
	void SpawnWave();
};