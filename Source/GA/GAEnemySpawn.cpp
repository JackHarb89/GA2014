// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAEnemy.h"
#include "Net/UnrealNetwork.h"


AGAEnemySpawn::AGAEnemySpawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	HasSpawnedEnemy = false;
	HasFinishedWave = false;

	Time = 0;
	SpawnInterval = 1;
	WaveNumber = 0;
	AllowedToSpawn = false;
	isInit = false;

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AGAEnemySpawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// General
	DOREPLIFETIME(AGAEnemySpawn, Time);
	DOREPLIFETIME(AGAEnemySpawn, isInit);
	DOREPLIFETIME(AGAEnemySpawn, Waves);

	// Single Spawn
	DOREPLIFETIME(AGAEnemySpawn, HasSpawnedEnemy); 

	// Wave
	DOREPLIFETIME(AGAEnemySpawn, AllowedToSpawn);
	DOREPLIFETIME(AGAEnemySpawn, HasFinishedWave); 
	DOREPLIFETIME(AGAEnemySpawn, WaveNumber);
	DOREPLIFETIME(AGAEnemySpawn, SpawnInterval);
	DOREPLIFETIME(AGAEnemySpawn, Destructible);

	// Trigger
	DOREPLIFETIME(AGAEnemySpawn, beTriggered);
	DOREPLIFETIME(AGAEnemySpawn, Trigger);

}

void AGAEnemySpawn::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	Time += DeltaTime;
	if (!isInit) InitWave();
	CheckTrigger();
	SetNextWaveStruct();							// Check Wave Clear
	
	if (Time >= SpawnInterval && AllowedToSpawn){
		SpawnWave();
		Time = 0;
	}

}
#pragma region Check Destructible

void AGAEnemySpawn::DestroySpawn(){
	if (Role < ROLE_Authority){
		ServerDestroySpawn();
	}
	else {
		this->Destroy();
	}
}

#pragma endregion

#pragma region Check Trigger

void AGAEnemySpawn::CheckTrigger(){
	if (Role < ROLE_Authority){
		ServerCheckTrigger();
	}
	else {
		if (beTriggered && !AllowedToSpawn) {
			if (Trigger != NULL) AllowedToSpawn = Trigger->isTriggered;
			else UE_LOG(LogClass, Warning, TEXT("*** SPAWN :: NO TRIGGER CONNECTED ***"))
		}
		else if (!beTriggered && !AllowedToSpawn) AllowedToSpawn = true;
	}
}

#pragma endregion

#pragma region Spawn Wave

void AGAEnemySpawn::InitWave(){
	if (Role < ROLE_Authority){
		ServerInitWave();
	}
	else{
		Destructible->SetOwner(this);
		FWave* waveStruct = &Waves[WaveNumber];
		SpawnInterval = waveStruct->SpawnInterval;
		isInit = true;
	}
}

void AGAEnemySpawn::SetNextWaveStruct(){
	if (Role < ROLE_Authority){
		ServerSetNextWaveStruct();
	}
	else{
		FWave* waveStruct = &Waves[WaveNumber];
		if (waveStruct->isWaveFinished()){
			if (WaveNumber < Waves.Num() - 1){
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: FINISHED WAVE %d ***"), WaveNumber);
				WaveNumber++;
				HasFinishedWave = true;
				isInit = false;
			}
		}
	}
}

void AGAEnemySpawn::SpawnWave(){
	if (Role < ROLE_Authority){
		ServerSpawnWave();
	}
	else{
		// Stop If No Wave Data
		if (Waves.GetData() == NULL) return;

		FWave* waveStruct = &Waves[WaveNumber];

		if (waveStruct->isWaveFinished()) return;
		int32 SpawnedEnemies = 0;
		SpawnLocationsRemaining.Empty();
		for (int32 i = 0; i < SpawnLocations.Num(); i++){
			SpawnLocationsRemaining.Add(SpawnLocations[i]);
		}
		while (SpawnedEnemies < waveStruct->NumberEnemySameTime && SpawnLocationsRemaining.Num()>0){
			const float randomSmallEnemy = FMath::FRandRange(0, waveStruct->SmallEnemyChance);
			const float randomNormalEnemy = FMath::FRandRange(0, waveStruct->NormalEnemyChance);
			const float randomBigEnemy = FMath::FRandRange(0, waveStruct->BigEnemyChance);

			float MaxValue = FMath::Max3(randomSmallEnemy, randomNormalEnemy, randomBigEnemy);
			if (MaxValue == randomSmallEnemy && waveStruct->NumberEnemy > 0){
				HasSpawnedEnemy = false;
				SpawnEnemy(waveStruct->getSmallEnemy());
				waveStruct->NumberEnemy--;
			}
			else if (MaxValue == randomNormalEnemy && waveStruct->NumberEnemy > 0){
				HasSpawnedEnemy = false;
				SpawnEnemy(waveStruct->getNormalEnemy());
				waveStruct->NumberEnemy--;
			}
			else if (MaxValue == randomBigEnemy && waveStruct->NumberEnemy > 0){
				HasSpawnedEnemy = false;
				SpawnEnemy(waveStruct->getBigEnemy());
				waveStruct->NumberEnemy--;
			}
			SpawnedEnemies++;
		}
	}
}

#pragma endregion

#pragma region Spawn Enemy

void AGAEnemySpawn::SpawnEnemy(TSubclassOf<class AActor> enemyClass){
	if (Role < ROLE_Authority){
		ServerSpawnEnemy(enemyClass);
	}
	else{
		UWorld* const World = GetWorld();
		if (World){
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.bNoCollisionFail = true;

			FVector SpawnLocation;
			FRotator SpawnRotation;

			int32 randomIndex = FMath::RandRange(0, SpawnLocationsRemaining.Num() - 1);
			
			SpawnLocation = SpawnLocationsRemaining[randomIndex]->GetActorLocation();
			SpawnRotation = SpawnLocationsRemaining[randomIndex]->GetActorRotation();

			SpawnLocationsRemaining.RemoveAt(randomIndex);

			AGAEnemy* enemy = World->SpawnActor<AGAEnemy>(enemyClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (enemy != NULL) {
				enemy->SpawnAIController();
				enemy->BeginPlay();
				HasSpawnedEnemy = true;
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: SPAWNED ***"));
			}
			else {
				UE_LOG(LogClass, Warning, TEXT("*** SERVER :: FAILED TO SPAWN ***"));
			}
		}
	}
}

#pragma  endregion

#pragma region Network - Spawn Enemy
void AGAEnemySpawn::OnRep_HasSpawnedEnemy(){
	if (HasSpawnedEnemy){
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: SPAWNED ***"));
	}
}

bool  AGAEnemySpawn::ServerSpawnEnemy_Validate(TSubclassOf<class AActor> enemyClass){return true;}
void  AGAEnemySpawn::ServerSpawnEnemy_Implementation(TSubclassOf<class AActor> enemyClass){SpawnEnemy(enemyClass);}

#pragma endregion

#pragma region Network - Spawn Wave

void AGAEnemySpawn::OnRep_HasFinishedWave(){
	if (HasFinishedWave){
		ServerResetHasFinishedWave();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: FINISHED WAVE %d ***"), WaveNumber);
	}
}
bool AGAEnemySpawn::ServerInitWave_Validate(){return true;}
void AGAEnemySpawn::ServerInitWave_Implementation(){InitWave();}

bool AGAEnemySpawn::ServerSetNextWaveStruct_Validate(){return true;}
void AGAEnemySpawn::ServerSetNextWaveStruct_Implementation(){SetNextWaveStruct();}

bool AGAEnemySpawn::ServerSpawnWave_Validate(){return true;}
void AGAEnemySpawn::ServerSpawnWave_Implementation(){SpawnWave();}

bool AGAEnemySpawn::ServerResetHasFinishedWave_Validate(){ return true; }
void AGAEnemySpawn::ServerResetHasFinishedWave_Implementation(){ HasFinishedWave = false; }
#pragma endregion

#pragma region Network - Check Trigger

bool AGAEnemySpawn::ServerCheckTrigger_Validate(){return true;}
void AGAEnemySpawn::ServerCheckTrigger_Implementation(){CheckTrigger();}

#pragma endregion

#pragma region Network - Check Destructible

bool AGAEnemySpawn::ServerDestroySpawn_Validate(){ return true; }
void AGAEnemySpawn::ServerDestroySpawn_Implementation(){ DestroySpawn(); }

#pragma endregion