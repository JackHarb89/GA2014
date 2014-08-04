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

	SpawnInterval = 1;
	NextWaveTimer = 15;
	NextSpawnActiveTimer = 10;

	SpawnWaveIndex = 0;
	SpawnTimer = 0;

	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;

	PrimaryActorTick.bCanEverTick = true;

	// false = Q
	AllowedToTick = true;
}

void AGAWaves::Tick(float DeltaTime){
	if (AllowedToTick){
		Super::Tick(DeltaTime);
		InitReamingWaves();
		IncreaseSpawnTimer(DeltaTime);
		SetSpawnActive();
		SpawnNextWave();
	}
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
			GetWorld()->GetGameState<AGAGameState>()->IncreaseSpawnedWaves();
		}
	}
}

void AGAWaves::SetSpawnActive(){
	if (Role == ROLE_Authority){
		if (SpawnTimer >= NextSpawnActiveTimer && !IsSpawnSetActive && EnemySpawns.Num()>0){
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
		if (SpawnTimer >= NextWaveTimer &&  EnemySpawns.Num() > 0){	// If we have a Wave left and are allowed to spawn
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: SPAWNING WAVE %d ***"), SpawnWaveIndex + 1);

			TArray< TEnumAsByte<EGAEnemy::Type>> WaveOrder;
			int32 MaxEnemy = (int32)(SpawnWaveIndex / 5) + 7;
			int32 MaxBigEnemy = (int32)(SpawnWaveIndex / 10);
			int32 Points = 2 * SpawnWaveIndex + 1;

			for (int32 i = 0; i < MaxEnemy-MaxBigEnemy; i++){
				if (FMath::RandRange(0, 1) == 0){ WaveOrder.Add(EGAEnemy::GASmallEnemy); }
				else WaveOrder.Add(EGAEnemy::GANormalEnemy);
			}
			for (int32 i = 0; i < MaxBigEnemy; i++){
				WaveOrder.Add(EGAEnemy::GABigEnemy);
			}

			FWave NewWave;
			NewWave.SmallEnemy = SmallEnemy;
			NewWave.NormalEnemy = NormalEnemy;
			NewWave.BigEnemy = BigEnemy;
			NewWave.Points = Points;
			NewWave.SpawnInterval = SpawnInterval;
			NewWave.Wave = WaveOrder;

			((AGAEnemySpawn*)EnemySpawns[RandIndex])->SetCurrentWave(NewWave);

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
	DOREPLIFETIME(AGAWaves, SpawnInterval);
	DOREPLIFETIME(AGAWaves, NextWaveTimer);
	DOREPLIFETIME(AGAWaves, NextSpawnActiveTimer);
}


void AGAWaves::ToggleTick(){
	AllowedToTick = !AllowedToTick;
}