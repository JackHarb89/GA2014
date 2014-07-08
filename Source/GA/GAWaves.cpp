

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAWaves.h"


AGAWaves::AGAWaves(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SpawnWaveIndex = 0;
	SpawnTimer = 0;
	PrimaryActorTick.bCanEverTick = true;
}

void AGAWaves::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	SpawnTimer += DeltaTime;
	SpawnNextWave();
}

void AGAWaves::SpawnNextWave(){
	if (SpawnTimer >= NextWaveTimer && SpawnWaveIndex <= Waves.Num()-1){
		int32 RandIndex = FMath::RandRange(0, EnemySpawns.Num()-1);
		((AGAEnemySpawn*)EnemySpawns[RandIndex])->SetCurrentWave(Waves[SpawnWaveIndex]);
		SpawnWaveIndex++;
		SpawnTimer = 0;
	}
}
