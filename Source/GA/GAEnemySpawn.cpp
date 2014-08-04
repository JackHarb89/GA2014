// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemySpawn.h"
#include "GAEnemy.h"
#include "Net/UnrealNetwork.h"


AGAEnemySpawn::AGAEnemySpawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SpawnTimer = 0;
	IsSpawnActive = false;
	IsNewWave = false;
	
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
		if (!CurrentWave.HasNoReferences() && SpawnTimer >= CurrentWave.SpawnInterval && CurrentWave.EnemyIndex <= CurrentWave.Wave.Num() - 1 && IsSpawnActive){
			switch (CurrentWave.Wave[CurrentWave.EnemyIndex]){
			case(EGAEnemy::GASmallEnemy) :
				SpawnEnemy((&CurrentWave)->SmallEnemy);
				break;
			case(EGAEnemy::GANormalEnemy) :
				SpawnEnemy((&CurrentWave)->NormalEnemy);
				break;
			case(EGAEnemy::GABigEnemy) :
				SpawnEnemy((&CurrentWave)->BigEnemy);
				break;
			}
			SpawnTimer = 0;
			CurrentWave.EnemyIndex++;
		}
		else if (!CurrentWave.HasNoReferences() && CurrentWave.EnemyIndex > CurrentWave.Wave.Num() - 1 && IsSpawnActive&& IsNewWave){
			SetSpawnActivationStatusTo(false);
			IsNewWave = false;
			UE_LOG(LogClass, Warning, TEXT("*** SERVER :: WAVE SPAWN FINISHED ***"));
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
		IsNewWave = true;
	}
}

void AGAEnemySpawn::SetSpawnActivationStatusTo(bool State){
	if (Role == ROLE_Authority){
		IsSpawnActive = State;
		if (IsSpawnActive){
			SpawnBecameActive();
			UE_LOG(LogClass, Warning, TEXT("*** SERVER :: SPAWN BECAME ACTIVE ***"));
		}
		else {
			SpawnBecameInactive();
			UE_LOG(LogClass, Warning, TEXT("*** SERVER :: SPAWN BECAME INACTIVE ***"));
		}
	}
}

void AGAEnemySpawn::OnRep_IsSpawnActive(){
	if (IsSpawnActive){
		SpawnBecameActive();
		UE_LOG(LogClass, Warning, TEXT("*** CLIENT :: SPAWN BECAME ACTIVE ***"));
	}
	else {
		SpawnBecameInactive();
		UE_LOG(LogClass, Warning, TEXT("*** CLIENT :: SPAWN BECAME INACTIVE ***"));
	}
}

// Replicates All Replicated Properties
void AGAEnemySpawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGAEnemySpawn, IsSpawnActive);
}