// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAPlayerController.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Own Initiation
	isInit = false;

	// Equip
	ItemDamage = 0;
	ItemHealth = 0;

	// Simple Attack
	SimpleAttackOnCoolDown = false;

	// Special Attack
	SpecialAttackChargeTimer = 0;
	SpecialAttackTimesCharged = 0;
	SpecialAttackIsCharging = false;
	SpecialAttackOnCoolDown = false;

	// Player Stats
	MaxHealth = 100;
	RegenerationTime = 0;
	RegenerationTimer = 0;
	AllowedToRegenerate = true;

	// General
	PrimaryActorTick.bCanEverTick = true;
}

void AGAPlayerController::InitPlayer(){
	gaCharacter = (AGACharacter*) this->GetPawn();

	SimpleAttackCoolDownRestValue = gaCharacter->SimpleAttackCoolDown;
	SpecialAttackCoolDownRestValue = gaCharacter->SpecialAttackCoolDown;
	MaxHealth = gaCharacter->HealthPoints + ItemHealth;
	isInit = true;
}

void AGAPlayerController::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	if (!isInit) InitPlayer();
	
	// Cooldown Control
	ReduceSimpleAttackCoolDown(DeltaTime);
	ReduceSpecialAttackCoolDown(DeltaTime);

	// Increase Charge
	IncreaseChargeTime(DeltaTime);

	// Regenration
	RegenerateHP(DeltaTime);

	// Check if Health is below zero
	CheckDeath();
}

bool AGAPlayerController::IsInRange(AActor* target, float attackRange){
	FVector playerLocation = gaCharacter->GetActorLocation();
	FVector targetLocation = target->GetActorLocation();

	// Calculate Distance		*** WIP ***
	if (abs(playerLocation.X - targetLocation.X) < attackRange && abs(playerLocation.Y - targetLocation.Y) < attackRange){ return true; }
	return false;
}

void AGAPlayerController::TakeDamageByEnemy(float Damage){
	gaCharacter->HealthPoints -= Damage;
	gaCharacter->CharacterTookDamage();
	if (gaCharacter->HealthPoints <= 0){
		gaCharacter->Destroy();
	}
	AllowedToRegenerate = false;
	RegenerationTimer = 0;
}

void AGAPlayerController::CheckDeath(){
	if (gaCharacter->HealthPoints <= 0){
		UE_LOG(LogClass, Warning, TEXT("*** PLAYER :: DIED ***"));
	}
}

#pragma region Simple Attack

void AGAPlayerController::AttackSimple(){
	// Check If Attack Is On Cool Down
	if (SimpleAttackOnCoolDown) return;

	// Set Cool Down
	SimpleAttackOnCoolDown = true;

	// Find Actor To Deal Damage
	for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (IsInRange(*ActorItr,gaCharacter->SimpleAttackRange)){
			ActorItr->TakeDamageByEnemy(gaCharacter->SimpleAttackDamage + ItemDamage);
		}
	}
	gaCharacter->CharacterAttackedSimple();

	UE_LOG(LogClass, Log, TEXT("*** PLAYER:: ATTACKED SIMPLE ***"));
	UE_LOG(LogClass, Log, TEXT("*** ATTACK:: %f DAMAGE ***"), gaCharacter->SimpleAttackDamage + ItemDamage);
}

void AGAPlayerController::ReduceSimpleAttackCoolDown(float DeltaTime){
	// Reduce Cool Down
	if (SimpleAttackOnCoolDown) gaCharacter->SimpleAttackCoolDown -= DeltaTime;
	// Check If Cool Down Finished
	if (gaCharacter->SimpleAttackCoolDown <= 0){
		SimpleAttackOnCoolDown = false;
		gaCharacter->SimpleAttackCoolDown = SimpleAttackCoolDownRestValue;
	}
}

#pragma endregion

#pragma region Special Attack

void AGAPlayerController::AttackSpecial(){
	// Check If Attack Is On Cool Down
	if (SpecialAttackOnCoolDown) return;

	// Set Cool Down
	SpecialAttackOnCoolDown = true;
	SpecialAttackIsCharging = false;

	float Damage = CalculateSpecialAttackDamage();

	// Find Actor To Deal Damage
	for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (IsInRange(*ActorItr,gaCharacter->SpecialAttackRange)){
			ActorItr->TakeDamageByEnemy(Damage);
		}
	}

	SpecialAttackTimesCharged = 0;
	SpecialAttackChargeTimer = 0;
	gaCharacter->CharacterAttackedSpecial();

	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: ATTACKED SPECIAL***"));
}

void AGAPlayerController::ChargeSpecial(){
	// Check If Attack Is On Cool Down
	if (SpecialAttackOnCoolDown) return;

	SpecialAttackIsCharging = true;
	gaCharacter->CharacterStartedCharging();

	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: START CHARGING SPECIAL ***"));
}

void AGAPlayerController::IncreaseChargeTime(float DeltaTime){
	// Check If Character Is Charging
	if (!SpecialAttackIsCharging) return;

	SpecialAttackChargeTimer += DeltaTime;
	// Check If We Reached Charge Interval
	if (SpecialAttackChargeTimer >= gaCharacter->SpecialAttackChargeInterval){
		SpecialAttackTimesCharged++;
		SpecialAttackChargeTimer = 0;
		gaCharacter->CharacterIsCharging();
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: CHARGED ***"));
	}
}

float AGAPlayerController::CalculateSpecialAttackDamage(){
	float Damage;
	Damage = gaCharacter->SpecialAttackBaseDamage * SpecialAttackTimesCharged;
	Damage = (Damage > gaCharacter->SpecialAttackMaxDamage ? gaCharacter->SpecialAttackMaxDamage : Damage);

	return Damage;
}

void AGAPlayerController::ReduceSpecialAttackCoolDown(float DeltaTime){
	// Reduce Cool Down
	if (SpecialAttackOnCoolDown) gaCharacter->SpecialAttackCoolDown -= DeltaTime;
	// Check If Cool Down Finished
	if (gaCharacter->SpecialAttackCoolDown <= 0){
		SpecialAttackOnCoolDown = false;
		gaCharacter->SpecialAttackCoolDown = SpecialAttackCoolDownRestValue;
	}
}

bool AGAPlayerController::isAllowedToMove(){
	return !SpecialAttackIsCharging;
}

#pragma endregion

#pragma region Regeneration

void AGAPlayerController::RegenerateHP(float DeltaTime){
	if (RegenerationTimer < gaCharacter->OutOfCombatTime){
		RegenerationTimer += DeltaTime;
		if (RegenerationTimer >= gaCharacter->OutOfCombatTime) AllowedToRegenerate = true;
		else return;
	}
	RegenerationTime += DeltaTime;

	if (AllowedToRegenerate && RegenerationTime >= gaCharacter->RegenerationRate && gaCharacter->HealthPoints < MaxHealth){
		gaCharacter->HealthPoints = (gaCharacter->HealthPoints + gaCharacter->RegenerationAmount > MaxHealth ? MaxHealth : gaCharacter->HealthPoints + gaCharacter->RegenerationAmount);
		RegenerationTime = 0;
		gaCharacter->CharacterRegenerated();
	}
}

#pragma endregion

#pragma region Items

void AGAPlayerController::CalculateItems(){
/*	// Attack Damage
	float percentDamage = item->AuraPlayer.PercentDamage;
	ItemDamage += gaCharacter->SimpleAttackDamage*percentDamage / 100;

	// Armor

	// Health
	float percentLife = item->AuraPlayer.PercentHealth;
	ItemHealth += gaCharacter->HealthPoints*percentLife / 100;
	MaxHealth = gaCharacter->HealthPoints + ItemHealth;*/
}

void AGAPlayerController::EquipItem(AGAItem* item){
	FEquipment equip = gaCharacter->EquipItems;
	switch (item->Slot){
		case(0):
			if (gaCharacter->EquipItems.Helm != NULL) gaCharacter->InventoryItems.Add(gaCharacter->EquipItems.Helm);
			gaCharacter->InventoryItems.Remove(item);
			gaCharacter->EquipItems.Helm = item;
			break;
		case(1) :
			if (gaCharacter->EquipItems.Chest != NULL) gaCharacter->InventoryItems.Add(gaCharacter->EquipItems.Chest);
			gaCharacter->InventoryItems.Remove(item);
			gaCharacter->EquipItems.Chest = item;
			break;
		case(2) :
			if (gaCharacter->EquipItems.Weapon != NULL) gaCharacter->InventoryItems.Add(gaCharacter->EquipItems.Weapon);
			gaCharacter->InventoryItems.Remove(item);
			gaCharacter->EquipItems.Weapon = item;
			break;
		case(3) :
			if (gaCharacter->EquipItems.Trinket00 != NULL && gaCharacter->EquipItems.Trinket01 != NULL) {
				gaCharacter->InventoryItems.Add(gaCharacter->EquipItems.Trinket00);
				gaCharacter->InventoryItems.Remove(item);
				gaCharacter->EquipItems.Trinket00 = item;
			}
			else if (gaCharacter->EquipItems.Trinket00 == NULL) {
				gaCharacter->InventoryItems.Remove(item);
				gaCharacter->EquipItems.Trinket00 = item;
			}
			if (gaCharacter->EquipItems.Trinket00 != NULL && gaCharacter->EquipItems.Trinket01 == NULL) {
				gaCharacter->InventoryItems.Remove(item);
				gaCharacter->EquipItems.Trinket01 = item;
			}
			break;
	}
	CalculateItems();

	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: EQUIPED ITEM ***"));
}



void AGAPlayerController::PickUpItem(AGAItem* item){
	if (gaCharacter->InventoryItems.Num() < gaCharacter->InventorySlots){
		AGAItem* inventoryItem = item;
		item->Destroy();
		gaCharacter->InventoryItems.Add(inventoryItem);
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: PICKED UP ITEM ***"));
		EquipItem(item);
	}
	else UE_LOG(LogClass, Log, TEXT("*** PLAYER :: INVENTORY IS FULL ***"));
}

#pragma endregion