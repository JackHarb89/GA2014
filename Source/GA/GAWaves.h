

#pragma once

#include "GameFramework/Actor.h"
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

	UPROPERTY()															int32 EnemyIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")		UBlueprint* SmallEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")		UBlueprint* NormalEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")		UBlueprint* BigEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")		float SpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")		TArray< TEnumAsByte<EGAEnemy::Type> > Wave;


	TSubclassOf<class AActor> GetSmallEnemy(){
		return (UClass*)SmallEnemy->GeneratedClass;
	}

	TSubclassOf<class AActor> GetNormalEnemy(){
		return (UClass*)NormalEnemy->GeneratedClass;
	}

	TSubclassOf<class AActor> GetBigEnemy(){
		return (UClass*)BigEnemy->GeneratedClass;
	}

	bool HasNoReferences(){
		if (SmallEnemy == NULL && NormalEnemy == NULL && BigEnemy == NULL) return true;
		return false;
	}


	bool IsWaveFinished(){
		if (EnemyIndex == Wave.Num()-1) return true;
		return false;
	}

	FWave(){
		EnemyIndex = 0;
	}
};

UCLASS()
class AGAWaves : public AActor
{
	GENERATED_UCLASS_BODY()

	float SpawnTimer;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		float NextWaveTimer;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Waves")		float NextSpawnActiveTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")					TArray<FWave> Waves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")					TArray<AActor*> EnemySpawns;

private:

	int32 SpawnWaveIndex;
	int32 RandIndex;

	bool IsFirstTick;
	bool IsSpawnSetActive;

	void SetSpawnActive();
	void InitReamingWaves();
	void SetRemainingWaves();
	void IncreaseSpawnTimer(float DeltaTime);
	void SpawnNextWave();
	virtual void Tick(float Delta) OVERRIDE;
};
