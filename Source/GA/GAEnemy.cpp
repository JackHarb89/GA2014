// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemy.h"
#include "GAAIController.h"
#include "GAPlayerController.h"
#include "GACharacter.h"
#include "GAAudioManager.h"
#include "GAGameState.h"
#include "Net/UnrealNetwork.h"


AGAEnemy::AGAEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

	IsInit = false;
	IsAlive = true;

	Armor = 0;
	HealthPoints = 100;

	HasTookDamage = false;

	bUseControllerRotationYaw = true;
	PrimaryActorTick.bCanEverTick = true;

	// Simple Attack
	SimpleAttackDamageMin = 25;
	SimpleAttackDamageMax = 25;
	SimpleAttackCoolDown = 0.75;
	SimpleAttackOnCoolDown = true;
	SimpleAttackRange = 200;


	// Replicate to Server / Clients
	bReplicates = true;
}

void AGAEnemy::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	if (!IsInit) InitPlayer();
	if (AllowedToAttack) ReduceSimpleAttackCoolDown(DeltaTime);
}

void AGAEnemy::InitPlayer(){
	SimpleAttackCoolDownRestValue = SimpleAttackCoolDown;
	IsInit = true;
	Tags.Add("Orc");
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
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.bNoCollisionFail = true;
		SpawnInfo.OverrideLevel = GetLevel();
		Controller = GetWorld()->SpawnActor<AGAAIController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
		if (Controller != NULL)
		{
			Controller->Possess(this);
		}
	}
}

bool AGAEnemy::DealDamage(){
	if (!AllowedToAttack) AllowedToAttack = true;
	AttackSimple();
	return true;
}

#pragma region Attack Simple


void AGAEnemy::AttackSimple(){
	if (Role < ROLE_Authority){
		ServerAttackSimple();
	}
	else{
		// Check If Attack Is On Cool Down
		if (SimpleAttackOnCoolDown) return;

		// Set Cool Down
		SimpleAttackOnCoolDown = true;

		int32 Damage = FMath::RandRange(SimpleAttackDamageMin, SimpleAttackDamageMax);

		// Find Actor To Deal Damage
		for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (IsInRange(*ActorItr)){
				(ActorItr)->TakeDamageByEnemy(Damage);
			}
		}

		CharacterAttackedSimple();

		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->EnemyAttacked(this);
		}

		UE_LOG(LogClass, Log, TEXT("*** Enemy :: ATTACKED SIMPLE  (ServerCall) ***"));
	}
}

void AGAEnemy::ReduceSimpleAttackCoolDown(float Delta){
	if (Role < ROLE_Authority){
		ServerReduceSimpleAttackCoolDown(Delta);
	}
	else{
		// Reduce Cool Down
		if (SimpleAttackOnCoolDown) SimpleAttackCoolDown -= Delta;
		// Check If Cool Down Finished
		if (SimpleAttackCoolDown <= 0){
			SimpleAttackOnCoolDown = false;
			UE_LOG(LogClass, Log, TEXT("*** Enemy :: ATTACK OFF COOLDOWN (ServerCall) ***"));
			SimpleAttackCoolDown = SimpleAttackCoolDownRestValue;
			AllowedToAttack = false;
		}
	}
}

// Only Client
bool AGAEnemy::IsInRange(AActor* target){
	FVector playerLocation = GetActorLocation();
	FVector targetLocation = target->GetActorLocation();

	if (FVector::Dist(playerLocation, targetLocation) <= SimpleAttackRange) return true;
	return false;
}


#pragma endregion

#pragma region Item

void AGAEnemy::CheckItemDrop(){
	if (Role < ROLE_Authority){
		ServerCheckItemDrop();
	}
	else{
		TSubclassOf<class AActor> item;
		TArray <FGameItem> DropItems;
		int random;
		for (int i = 0; i < LootTable.Num(); i++){
			random = FMath::FRandRange(0, 100);
			if (random <= LootTable[i].DropChance) DropItems.Add(LootTable[i]);
		}

		if (DropItems.Num() == 0) return;
		item = DropItems[FMath::RandRange(0, DropItems.Num() - 1)].getItemClass();
		if (item != NULL) DropItem(item);
	}
}

void AGAEnemy::DropItem(TSubclassOf<class AActor> item){
	UWorld* const World = GetWorld();
	if (World){
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.bNoCollisionFail = true;

		FVector SpawnLocation;
		FRotator SpawnRotation;

		SpawnLocation = GetActorLocation();
		SpawnRotation = GetActorRotation();

		AGAItem* DropedItem = World->SpawnActor<AGAItem>(item, SpawnLocation, SpawnRotation, SpawnParams);
		if (DropedItem != NULL) {
			switch (DropedItem->Slot){
				case(EGASlot::GAMoney) :
					for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
						(*ActorItr)->EnemyDropedGold(this);
					}
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED MONEY (%f) ***"), DropedItem->Value);
					break;
				case(EGASlot::GAHead) :
					for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
						(*ActorItr)->EnemyDropedItem(this);
					}
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED HEAD ***"));
					break;
				case(EGASlot::GAChest) :
					for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
						(*ActorItr)->EnemyDropedItem(this);
					}
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED CHEST ***"));
					break;
				case(EGASlot::GATrinket) :
					for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
						(*ActorItr)->EnemyDropedItem(this);
					}
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED TRINKET ***"));
					break;
				case(EGASlot::GAWeapon) :
					for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
						(*ActorItr)->EnemyDropedItem(this);
					}
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED WEAPON ***"));
					break;
			}
		}
		else {
			UE_LOG(LogClass, Warning, TEXT("*** CLIENT :: FAILED TO  DROP ITEM ***"));
		}
	}
}

#pragma endregion

#pragma region Take Damage

void AGAEnemy::TakeDamageByEnemy(float Damage){
	if (Role < ROLE_Authority){
		ServerTakeDamageByEnemy(Damage);
	}
	else{
		ApplyDamage(Damage);
	}
}

void AGAEnemy::ApplyDamage(float Damage) {
	if (Damage == -1){HealthPoints = 0;}							// One Hit Kill || Shard Kill
	else { HealthPoints -= (Damage - (Armor*0.25*Damage) / 100); }

	HasTookDamage = true;
	CharacterTookDamage();

	for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		(*ActorItr)->EnemyTookDamage(this);
	}

	CheckDeath();
}

#pragma endregion

#pragma region Check Death

void AGAEnemy::CheckDeath(){
	if (Role < ROLE_Authority){
		ServerCheckDeath();
	}
	else {
		if (HealthPoints <= 0){
			for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				(*ActorItr)->EnemyDied(this);
			}
			IsAlive = false;
			CheckItemDrop();
			CharacterDied();
		}
	}
}

void AGAEnemy::OnRep_IsAlive(){
	if (!IsAlive){
		CharacterDied();
	}
}

bool AGAEnemy::ServerCheckDeath_Validate(){return true;}
void AGAEnemy::ServerCheckDeath_Implementation(){CheckDeath();}

#pragma endregion

#pragma region Network - Attack Simple

void AGAEnemy::OnRep_SimpleAttackOnCoolDown(){
	if (SimpleAttackOnCoolDown){
		CharacterAttackedSimple();
		UE_LOG(LogClass, Log, TEXT("*** Enemy :: ATTACKED SIMPLE (ServerCall) ***"));
	}
	else{
		UE_LOG(LogClass, Log, TEXT("*** Enemy :: ATTACK OFF COOLDOWN (ServerCall) ***"));
	}
}

bool AGAEnemy::ServerReduceSimpleAttackCoolDown_Validate(float Delta){ return true; }
void AGAEnemy::ServerReduceSimpleAttackCoolDown_Implementation(float Delta){ ReduceSimpleAttackCoolDown(Delta); }

bool AGAEnemy::ServerAttackSimple_Validate(){ return true; }
void AGAEnemy::ServerAttackSimple_Implementation(){ AttackSimple(); }

#pragma endregion

#pragma region Network - Item

bool AGAEnemy::ServerCheckItemDrop_Validate(){return true;}
void AGAEnemy::ServerCheckItemDrop_Implementation(){CheckItemDrop();}

#pragma endregion

#pragma region Network - Take Damage

void AGAEnemy::OnRep_HasTookDamage(){
	if (HasTookDamage){
		CharacterTookDamage();

		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->EnemyTookDamage(this);
		}
		ServerResetHasTookDamage();
	}
}

bool AGAEnemy::ServerTakeDamageByEnemy_Validate(float Damage){return true;}
void AGAEnemy::ServerTakeDamageByEnemy_Implementation(float Damage){TakeDamageByEnemy(Damage);}

bool AGAEnemy::ServerResetHasTookDamage_Validate(){return true;}
void AGAEnemy::ServerResetHasTookDamage_Implementation(){HasTookDamage = false;}

#pragma endregion

void AGAEnemy::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AGAEnemy, Armor);
	DOREPLIFETIME(AGAEnemy, LootTable);
	
	DOREPLIFETIME(AGAEnemy, HealthPoints);
	DOREPLIFETIME(AGAEnemy, HasTookDamage);
	DOREPLIFETIME(AGAEnemy, IsAlive);
	
	DOREPLIFETIME(AGAEnemy, SimpleAttackOnCoolDown);
	DOREPLIFETIME(AGAEnemy, SimpleAttackCoolDownRestValue);
	DOREPLIFETIME(AGAEnemy, SimpleAttackDamageMin);
	DOREPLIFETIME(AGAEnemy, SimpleAttackDamageMax);
	DOREPLIFETIME(AGAEnemy, SimpleAttackCoolDown);
	DOREPLIFETIME(AGAEnemy, SimpleAttackRange);

	DOREPLIFETIME(AGAEnemy, EnemyBehavior);
}