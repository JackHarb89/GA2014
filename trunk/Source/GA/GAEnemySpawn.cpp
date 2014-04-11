// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAEnemy.h"

AGAEnemySpawn::AGAEnemySpawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	/*
	static ConstructorHelpers::FObjectFinder <UBlueprint> enemyBP(TEXT("Blueprint'/Game/Blueprints/Enemy.Enemy'"));
	enemyClass = (UClass*)enemyBP.Object->GeneratedClass;

	
	enemiesToSpawn = 5;

	Time = 0;
	SpawnOffset = 0;
	*/
	PrimaryActorTick.bCanEverTick = true;

	Time = 0;
	SpawnOffset = 0;
}

void AGAEnemySpawn::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	Time += DeltaTime;
	if (Time >2){
		SpawnWave();
		Time = 0;
	}
}


// Spawn:
// Time
// Wahrscheinlichkeit
// Only SPAWN ONE BIG AT A TIME --> TIMER
// trigger? 

// Enemy:
// Damage max - min  --> even matched
// Drops


void AGAEnemySpawn::SpawnWave(){
	// Stop If No Wave Data
	if (waves.GetData() == NULL) return;

	FWave* waveStruct = &waves[0];

	//if (&waveStruct->getNumberEnemies == 0) return;

	if (waveStruct->NumberSmallEnemy > 0){
		SpawnEnemy(waveStruct->getSmallEnemy());
		waveStruct->NumberSmallEnemy--;
	}
	else if (waveStruct->NumberNormalEnemy > 0 && waveStruct->NumberSmallEnemy == 0){
		SpawnEnemy(waveStruct->getNormalEnemy());
		waveStruct->NumberNormalEnemy--;
	}
	else if (waveStruct->NumberBigEnemy > 0 && waveStruct->NumberNormalEnemy == 0 && waveStruct->NumberSmallEnemy == 0){
		SpawnEnemy(waveStruct->getBigEnemy());
		waveStruct->NumberBigEnemy--;
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

			AGAEnemy* enemy = GetWorld()->SpawnActor<AGAEnemy>(enemyClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (enemy != NULL) {
				enemy->SpawnAIController();
				enemy->BeginPlay();
				UE_LOG(LogClass, Log, TEXT("***SPAWNED***"));
			}
			else {
				UE_LOG(LogClass, Warning, TEXT("***FAILED TO SPAWN***"));
			}
	}
}