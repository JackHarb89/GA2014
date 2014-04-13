// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAEnemy.h"

AGAEnemySpawn::AGAEnemySpawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;

	Time = 0;
	SpawnInterval = 1;
	WaveNumber = 0;
	AllowedToSpawn = false;
	isInit = false;
}

void AGAEnemySpawn::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	Time += DeltaTime;

	if (!isInit) InitFirstWave();					

	CheckTrigger();
	SetNextWaveStruct();							// Check Wave Clear
	
	if (Time >= SpawnInterval && AllowedToSpawn){
		SpawnWave();
		Time = 0;
	}

}

void AGAEnemySpawn::InitFirstWave(){
	SetSpawnInterval();
}

void AGAEnemySpawn::SetSpawnInterval(){
	FWave* waveStruct = &waves[WaveNumber];
	SpawnInterval = waveStruct->SpawnInterval;
}

void AGAEnemySpawn::CheckTrigger(){
	if (beTriggered && !AllowedToSpawn) {
		if (Trigger != NULL) AllowedToSpawn = Trigger->isTriggered;
		else UE_LOG(LogClass, Warning, TEXT("*** SPAWN :: NO TRIGGER CONNECTED ***"))
	}
	else if (!beTriggered && !AllowedToSpawn) AllowedToSpawn = true;
}

void AGAEnemySpawn::SetNextWaveStruct(){
	FWave* waveStruct = &waves[WaveNumber];
	if (waveStruct->isWaveFinished()){
		if (WaveNumber < waves.Num() - 1){
			UE_LOG(LogClass, Log, TEXT("*** SPAWN :: FINISHED WAVE %d ***"), WaveNumber);
			WaveNumber++;
			SetSpawnInterval();
		}
	}
}

void AGAEnemySpawn::SpawnWave(){
	// Stop If No Wave Data
	if (waves.GetData() == NULL) return;

	FWave* waveStruct = &waves[WaveNumber];
	

	if (waveStruct->isWaveFinished()) return;

	const float randomSmallEnemy = FMath::FRandRange(0, waveStruct->SmallEnemyChance);
	const float randomNormalEnemy = FMath::FRandRange(0, waveStruct->NormalEnemyChance);
	const float randomBigEnemy = FMath::FRandRange(0, waveStruct->BigEnemyChance);

	float MaxValue = FMath::Max3(randomSmallEnemy, randomNormalEnemy, randomBigEnemy);

	if (MaxValue == randomSmallEnemy) {
		SpawnEnemy(waveStruct->getSmallEnemy());
		waveStruct->NumberEnemy--;
	}
	else if (MaxValue == randomNormalEnemy){
		SpawnEnemy(waveStruct->getNormalEnemy());
		waveStruct->NumberEnemy--;
	}
	else if (MaxValue == randomBigEnemy){
		SpawnEnemy(waveStruct->getBigEnemy());
		waveStruct->NumberEnemy--;
	}
}

void AGAEnemySpawn::SpawnEnemy(TSubclassOf<class AActor> enemyClass){
		UWorld* const World = GetWorld();
		if (World){
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.bNoCollisionFail = true;

			FVector SpawnLocation;
			FRotator SpawnRotation;

			SpawnLocation = this->GetActorLocation();
			SpawnRotation = this->GetActorRotation();

			AGAEnemy* enemy = World->SpawnActor<AGAEnemy>(enemyClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (enemy != NULL) {
				enemy->SpawnAIController();
				enemy->BeginPlay();
				UE_LOG(LogClass, Log, TEXT("*** SPAWN :: SPAWNED ***"));
			}
			else {
				UE_LOG(LogClass, Warning, TEXT("*** SPAWN :: FAILED TO SPAWN ***"));
			}
	}
}