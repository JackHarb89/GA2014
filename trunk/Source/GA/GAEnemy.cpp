// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemy.h"
#include "GAAIController.h"
#include "GACharacter.h"


AGAEnemy::AGAEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	HealthPoints = 100;

	bUseControllerRotationYaw = true;
	PrimaryActorTick.bCanEverTick = true;

	// Simple Attack
	SimpleAttackDamageMin = 25;
	SimpleAttackDamageMax = 25;
	SimpleAttackCoolDown = 0.75;
	SimpleAttackCoolDownRestValue = SimpleAttackCoolDown;
	SimpleAttackOnCoolDown = false;
}
void AGAEnemy::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	ReduceSimpleAttackCoolDown(DeltaTime);
}

void AGAEnemy::TakeDamageByEnemy(float Damage){
	HealthPoints -= Damage;
	CharacterTookDamage();
	if (HealthPoints <= 0){
		this->Destroy();
		CheckItemDrop();
	}
}

bool AGAEnemy::DealDamage(){
	AttackSimple();
	return true;
}

void AGAEnemy::AttackSimple(){
	// Check If Attack Is On Cool Down
	if (SimpleAttackOnCoolDown) return;

	// Set Cool Down
	SimpleAttackOnCoolDown = true;

	int32 Damage = FMath::RandRange(SimpleAttackDamageMin, SimpleAttackDamageMax);

	// Find Actor To Deal Damage
	for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (IsInRange(*ActorItr)){
			ActorItr->TakeDamageByEnemy(Damage);
		}
	}

	UE_LOG(LogClass, Log, TEXT("*** ENEMY :: ATTACKED SIMPLE ***"));
}

void AGAEnemy::ReduceSimpleAttackCoolDown(float DeltaTime){
	// Reduce Cool Down
	if (SimpleAttackOnCoolDown) SimpleAttackCoolDown -= DeltaTime;
	// Check If Cool Down Finished
	if (SimpleAttackCoolDown <= 0){
		SimpleAttackOnCoolDown = false;
		SimpleAttackCoolDown = SimpleAttackCoolDownRestValue;
	}
}

bool AGAEnemy::IsInRange(AActor* target){
	FVector playerLocation = GetActorLocation();
	FVector targetLocation = target->GetActorLocation();

	// Calculate Distance		*** WIP ***
	if (abs(playerLocation.X - targetLocation.X) < 100 && abs(playerLocation.Y - targetLocation.Y) < 100){ return true; }
	return false;
}

void AGAEnemy::CheckItemDrop(){
	TSubclassOf<class AActor> item;
	float maxValue = 0;
	int random;
	for (int i = 0; i < LootTable.Num(); i++){
		if (maxValue < LootTable[i].DropChance){
			maxValue = LootTable[i].DropChance;
			item = LootTable[i].getItemClass();
		}
	}

	if (maxValue == 0) return;
	random = FMath::RandRange(0, 100 / maxValue - 1); // 0 == Drop!

	if (random == 0 && item != NULL) DropItem(item);
}

void AGAEnemy::DropItem(TSubclassOf<class AActor> item){
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

		AGAItem* enemy = World->SpawnActor<AGAItem>(item, SpawnLocation, SpawnRotation, SpawnParams);
		if (enemy != NULL) {
			enemy->BeginPlay();
			UE_LOG(LogClass, Log, TEXT("*** ENEMY :: DROPED ITEM ***"));
		}
		else {
			UE_LOG(LogClass, Warning, TEXT("*** ENEMY :: FAILED TO  DROP ITEM ***"));
		}
	}
}

void AGAEnemy::SpawnAIController(){
	if (Controller != NULL || GetNetMode() == NM_Client)
	{
		return;
	}
	AIControllerClass = AGAAIController::StaticClass();
	if (AIControllerClass != NULL)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = Instigator;
		SpawnInfo.bNoCollisionFail = true;
		SpawnInfo.OverrideLevel = GetLevel();
		Controller = GetWorld()->SpawnActor<AController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
		if (Controller != NULL)
		{
			Controller->Possess(this);
		}
	}
}