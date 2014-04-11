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
	SimpleAttackDamage = 25;
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

	// Find Actor To Deal Damage
	for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (IsInRange(*ActorItr)){
			ActorItr->TakeDamageByEnemy(SimpleAttackDamage);
		}
	}

	UE_LOG(LogClass, Log, TEXT("***ENEMY :: ATTACKED SIMPLE***"));
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