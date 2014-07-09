

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAWaves.h"
#include "GAGameState.h"
#include "Net/UnrealNetwork.h"


AGAWaves::AGAWaves(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	IsFirstTick = true;
	IsSpawnSetActive = false;

	SpawnWaveIndex = 0;
	SpawnTimer = 0;

	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;

	PrimaryActorTick.bCanEverTick = true;
}

void AGAWaves::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	InitReamingWaves();
	IncreaseSpawnTimer(DeltaTime);
	SetSpawnActive();
	SpawnNextWave();
}

// Initial Setting of Remaining Waves in Game State
void AGAWaves::InitReamingWaves(){
	if (IsFirstTick) {
		SetRemainingWaves();
		IsFirstTick = false;
	}
}

// Increases the SpawnTimer  *** Only Server Can Increase the Timer ***
void AGAWaves::IncreaseSpawnTimer(float DeltaTime){
	if (Role == ROLE_Authority){																		// Are we server?
		SpawnTimer += DeltaTime;																		// Add Frametime to SpawnTimer
	}
}
// Sets Remaining Waves in GameState  *** SERVER ONLY ***
void AGAWaves::SetRemainingWaves(){
	if (Role == ROLE_Authority){
		if (GetWorld()->GetGameState<AGAGameState>()) {
			GetWorld()->GetGameState<AGAGameState>()->SetRemainingWaves(Waves.Num() - SpawnWaveIndex);
		}
	}
}

void AGAWaves::SetSpawnActive(){
	if (Role == ROLE_Authority){
		if (SpawnTimer >= NextWaveTimer - 10 && !IsSpawnSetActive){
			// Use a random Spawnlocation
			RandIndex = FMath::RandRange(0, EnemySpawns.Num() - 1);
			((AGAEnemySpawn*)EnemySpawns[RandIndex])->SetSpawnActivationStatusTo(true);
			IsSpawnSetActive = true;
		}
	}
}

// Spawns the Next Wave  *** Only Server Can Spawn Waves ***
void AGAWaves::SpawnNextWave(){
	if (Role == ROLE_Authority){																		// Are we server?
		if (SpawnTimer >= NextWaveTimer && SpawnWaveIndex <= Waves.Num() - 1){							// If we have a Wave left and are allowed to spawn
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: SPAWNING WAVE %d ***"),SpawnWaveIndex+1);
			
			((AGAEnemySpawn*)EnemySpawns[RandIndex])->SetCurrentWave(Waves[SpawnWaveIndex]);
			
			SpawnWaveIndex++;
			SpawnTimer = 0;
			IsSpawnSetActive = false;

			// Set Remaining Waves in GameState
			SetRemainingWaves();
		}
	}
}

// Replicates All Replicated Properties
void AGAWaves::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Wave Timer
	DOREPLIFETIME(AGAWaves, NextWaveTimer);

}
