// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAEnemy.h"
#include "Net/UnrealNetwork.h"


AGAEnemySpawn::AGAEnemySpawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SpawnTimer = 0;

	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;

	PrimaryActorTick.bCanEverTick = true;
}

void AGAEnemySpawn::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	IncreaseSpawnTimer(DeltaTime);
	SpawnCurrentWave();
}

void AGAEnemySpawn::IncreaseSpawnTimer(float DeltaTime){
	if (Role == ROLE_Authority){
		SpawnTimer += DeltaTime;
	}
}

void AGAEnemySpawn::SpawnCurrentWave(){
	if (Role == ROLE_Authority){
		if (&CurrentWave && SpawnTimer >= CurrentWave.SpawnInterval && CurrentWave.EnemyIndex <= CurrentWave.Wave.Num() - 1){
			switch (CurrentWave.Wave[CurrentWave.EnemyIndex]){
			case(EGAEnemy::GASmallEnemy) :
				SpawnEnemy((&CurrentWave)->GetSmallEnemy());
				break;
			case(EGAEnemy::GANormalEnemy) :
				SpawnEnemy((&CurrentWave)->GetNormalEnemy());
				break;
			case(EGAEnemy::GABigEnemy) :
				SpawnEnemy((&CurrentWave)->GetBigEnemy());
				break;
			}
			SpawnTimer = 0;
			CurrentWave.EnemyIndex++;
		}
	}
}

void AGAEnemySpawn::SpawnEnemy(TSubclassOf<class AActor> EnemyClass){
	if (Role == ROLE_Authority){
		UWorld* const World = GetWorld();
		if (World){
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.bNoCollisionFail = true;

			FVector SpawnLocation;
			FRotator SpawnRotation;

			SpawnLocation = GetActorLocation();
			SpawnRotation = GetActorRotation();

			AGAEnemy* Enemy = World->SpawnActor<AGAEnemy>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (Enemy != NULL) {
				Enemy->SpawnAIController();
				Enemy->BeginPlay();
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: SPAWNED ***"));
			}
			else {
				UE_LOG(LogClass, Warning, TEXT("*** SERVER :: FAILED TO SPAWN ***"));
			}
		}
	}
}

void AGAEnemySpawn::SetCurrentWave(FWave NewWave){
	if (Role == ROLE_Authority){
		CurrentWave = NewWave;
	}
}