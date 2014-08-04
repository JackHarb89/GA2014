#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAWaves.h"
#include "GAGameState.h"
#include "Net/UnrealNetwork.h"


AGAWaves::AGAWaves(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SmallEnemyValue = 1;
	NormalEnemyValue = 3;
	BigEnemyValue = 10;

	ValueGrowth = 2.0;
	ValueShift = 1.0;

    MaxEnemyGrowth = 0.2;
	MaxEnemyShift = 7.0;

	MaxBigGrowth = 0.1;
	MaxBigShift = 0.0;

	IsFirstTick = true;
	IsSpawnSetActive = false;

	SpawnInterval = 1;
	NextWaveTimer = 15;
	NextSpawnActiveTimer = 10;

	SpawnWaveIndex = 1;
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
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: SPAWNING WAVE %d ***"), SpawnWaveIndex);

			TArray< TEnumAsByte<EGAEnemy::Type>> WaveOrder;
			int32 Value = (int32) (ValueGrowth * SpawnWaveIndex + ValueShift);
			int32 MaxEnemy = (int32)(MaxEnemyGrowth * SpawnWaveIndex + MaxEnemyShift);
			int32 MaxBigEnemy = (int32)(MaxBigGrowth * SpawnWaveIndex + MaxBigShift);

			while (Value > 0){
				bool SmallAllowed = false;
				bool NoramlAllowed = false;
				bool BigAllowed = false;
				if (Value / SmallEnemyValue <= MaxEnemy){
					SmallAllowed = true;
				}
				if (Value / NormalEnemyValue <= MaxEnemy && (int32) (Value / NormalEnemyValue) > 0){
					NoramlAllowed = true;
				}
				if (MaxBigEnemy > 0 && Value / BigEnemyValue <= MaxEnemy && (int32)(Value / BigEnemyValue) > 0){
					BigAllowed = true;
				}

				int32 Rand;
				Rand = FMath::RandRange(0, 2);
				if (Rand == 0 && SmallAllowed){
					WaveOrder.Add(EGAEnemy::GASmallEnemy);
					Value -= SmallEnemyValue;
				}
				else if (Rand == 0 && !SmallAllowed){
					Rand = FMath::RandRange(1, 2);
				}

				if (Rand == 1 && NoramlAllowed){
					WaveOrder.Add(EGAEnemy::GANormalEnemy);
					Value -= NormalEnemyValue;
				}
				else if (Rand == 1 && !NoramlAllowed){
					Rand = 2;
				}

				if (Rand == 2 && BigAllowed){
					Value -= BigEnemyValue;
					MaxBigEnemy--;
					WaveOrder.Add(EGAEnemy::GABigEnemy);
				}
				if (!SmallAllowed && !NoramlAllowed && !BigAllowed && MaxEnemy > 0){
					UE_LOG(LogClass, Error, TEXT("*** NOT ENOUGH ENEMIES FOR POINTS AVAILABLE ***"));
					UE_LOG(LogClass, Error, TEXT("***        CHECK MAX ENEMY AND POINTS       ***"));
					break;
				}
				MaxEnemy--;
			}

			FWave NewWave;
			NewWave.SmallEnemy = SmallEnemy;
			NewWave.NormalEnemy = NormalEnemy;
			NewWave.BigEnemy = BigEnemy;
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