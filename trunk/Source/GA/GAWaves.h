

#pragma once

#include "GameFramework/Actor.h"
#include "GAEnemy.h"
#include "GAWaves.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
namespace EGAEnemy
{
	enum Type
	{
		GASmallEnemy		UMETA(DisplayName = "Small"),
		GANormalEnemy	 	UMETA(DisplayName = "Normal"),
		GABigEnemy			UMETA(DisplayName = "Big"),
	};
}

USTRUCT()
struct FWave
{
	GENERATED_USTRUCT_BODY();

	TArray< TEnumAsByte<EGAEnemy::Type> > Wave;

	TSubclassOf<class AGAEnemy> SmallEnemy;
	TSubclassOf<class AGAEnemy> NormalEnemy;
	TSubclassOf<class AGAEnemy> BigEnemy;

	float SpawnInterval;



	bool HasNoReferences(){
		if (SmallEnemy == NULL && NormalEnemy == NULL && BigEnemy == NULL) return true;
		return false;
	}


	bool IsWaveFinished(){
		if (Wave.Num() == 0) return true;
		return false;
	}

	FWave(){
	}
};

UCLASS()
class AGAWaves : public AActor
{
	GENERATED_UCLASS_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemey Class")		TSubclassOf<class AGAEnemy> SmallEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemey Class")		TSubclassOf<class AGAEnemy> NormalEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemey Class")		TSubclassOf<class AGAEnemy> BigEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemey Value")		int32 SmallEnemyValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemey Value")		int32 NormalEnemyValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemey Value")		int32 BigEnemyValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Calculation")		float ValueGrowth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Calculation")		float ValueShift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Max Enemy Calculation")		float MaxEnemyGrowth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Max Enemy Calculation")		float MaxEnemyShift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Max Big Enemy Calculation")		float MaxBigGrowth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Max Big Enemy Calculation")		float MaxBigShift;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		int32 SpawnWaveIndex;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		float SpawnInterval;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		float NextWaveTimer;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		float NextSpawnActiveTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")					TArray<AActor*> EnemySpawns;

	// ONLY MS2
	bool AllowedToTick;
	UFUNCTION(BlueprintCallable, Category = "Tick")									void ToggleTick();

private:

	float SpawnTimer;
	int32 RandIndex;
	FWave CurrentWave;

	bool IsFirstTick;
	bool IsSpawnSetActive;

	void SetSpawnActive();
	void InitReamingWaves();
	void SetRemainingWaves();
	void IncreaseSpawnTimer(float DeltaTime);
	void SpawnNextWave();
	virtual void Tick(float Delta) OVERRIDE;
};
