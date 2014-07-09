

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAWaves.h"
#include "Net/UnrealNetwork.h"


AGAWaves::AGAWaves(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SpawnWaveIndex = 0;
	SpawnTimer = 0;

	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;

	PrimaryActorTick.bCanEverTick = true;
}

void AGAWaves::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	IncreaseSpawnTimer(DeltaTime);
	SpawnNextWave();
}

// Increases the SpawnTimer  *** Only Server Can Increase the Timer ***
void AGAWaves::IncreaseSpawnTimer(float DeltaTime){
	if (Role == ROLE_Authority){																			// Are we server?
		SpawnTimer += DeltaTime;																		// Add Frametime to SpawnTimer
	}
}

// Spawns the Next Wave  *** Only Server Can Spawn Waves ***
void AGAWaves::SpawnNextWave(){
	if (Role == ROLE_Authority){																			// Are we server?
		if (SpawnTimer >= NextWaveTimer && SpawnWaveIndex <= Waves.Num() - 1){							// If we have a Wave left and are allowed to spawn
			int32 RandIndex = FMath::RandRange(0, EnemySpawns.Num() - 1);								// Use a random Spawnlocation
			((AGAEnemySpawn*)EnemySpawns[RandIndex])->SetCurrentWave(Waves[SpawnWaveIndex]);
			SpawnWaveIndex++;
			SpawnTimer = 0;
		}
	}
}

// Replicates All Replicated Properties
void AGAWaves::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Wave Timer
	DOREPLIFETIME(AGAWaves, NextWaveTimer);

}
