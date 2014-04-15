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

	// Attack Speed
	AttackSpeed = 1;

	// Simple Attack
	SimpleAttackOnCoolDown = false;

	// Special Attack
	SpecialAttackChargeTimer = 0;
	SpecialAttackTimesCharged = 0;
	SpecialAttackIsCharging = false;
	SpecialAttackOnCoolDown = false;

	// Regeneration
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
	gaCharacter->CharacterFinishedRegeneration();
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
		gaCharacter->SimpleAttackCoolDown = SimpleAttackCoolDownRestValue / AttackSpeed;
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
		if (RegenerationTimer >= gaCharacter->OutOfCombatTime && !AllowedToRegenerate) {
			AllowedToRegenerate = true;
			gaCharacter->CharacterStartedRegeneration();
		}
		else return;
	}
	RegenerationTime += DeltaTime;

	if (AllowedToRegenerate && RegenerationTime >= gaCharacter->RegenerationRate && gaCharacter->HealthPoints < MaxHealth){
		gaCharacter->HealthPoints = (gaCharacter->HealthPoints + gaCharacter->RegenerationAmount > MaxHealth ? MaxHealth : gaCharacter->HealthPoints + gaCharacter->RegenerationAmount);
		RegenerationTime = 0;
	}
	if (AllowedToRegenerate && gaCharacter->HealthPoints == MaxHealth)  gaCharacter->CharacterFinishedRegeneration();
}

#pragma endregion

#pragma region Items

void AGAPlayerController::CalculateItems(){
	FPlayerAura PercentBonus;
	TArray <AGAItem*> EquipedItems;
	if (gaCharacter->EquipItems.Head) EquipedItems.Add(gaCharacter->EquipItems.Head);
	if (gaCharacter->EquipItems.Chest) EquipedItems.Add(gaCharacter->EquipItems.Chest);
	if (gaCharacter->EquipItems.Weapon) EquipedItems.Add(gaCharacter->EquipItems.Weapon);
	if (gaCharacter->EquipItems.Trinket00) EquipedItems.Add(gaCharacter->EquipItems.Trinket00);
	if (gaCharacter->EquipItems.Trinket01) EquipedItems.Add(gaCharacter->EquipItems.Trinket01);

	// Gather All Bonus
	for (int i = 0; i < EquipedItems.Num(); i++){
		PercentBonus.PercentDamage += EquipedItems[i]->AuraPlayer.PercentDamage;
		PercentBonus.PercentAttackSpeed += EquipedItems[i]->AuraPlayer.PercentAttackSpeed;
		PercentBonus.PercentHealth += EquipedItems[i]->AuraPlayer.PercentHealth;
	}

	// Damage Bonus
	ItemDamage += gaCharacter->SimpleAttackDamage * PercentBonus.PercentDamage / 100;

	// Attack Speed
	AttackSpeed = 1 + 1 * PercentBonus.PercentAttackSpeed / 100;
	gaCharacter->SimpleAttackCoolDown = SimpleAttackCoolDownRestValue / AttackSpeed;


	// Movement Speed

	// Armor

	// Health
	ItemHealth += gaCharacter->HealthPoints * PercentBonus.PercentHealth / 100;
	MaxHealth = gaCharacter->HealthPoints + ItemHealth;
}

void AGAPlayerController::EquipItem(AGAItem* item){
	switch (item->Slot){
		case(EGASlot::GAHead):
			if (gaCharacter->EquipItems.Head != NULL) gaCharacter->InventoryItems.Add(gaCharacter->EquipItems.Head);
			gaCharacter->InventoryItems.Remove(item);
			gaCharacter->EquipItems.Head = item;
			break;
		case(EGASlot::GAChest) :
			if (gaCharacter->EquipItems.Chest != NULL) gaCharacter->InventoryItems.Add(gaCharacter->EquipItems.Chest);
			gaCharacter->InventoryItems.Remove(item);
			gaCharacter->EquipItems.Chest = item;
			break;
		case(EGASlot::GAWeapon) :
			if (gaCharacter->EquipItems.Weapon != NULL) gaCharacter->InventoryItems.Add(gaCharacter->EquipItems.Weapon);
			gaCharacter->InventoryItems.Remove(item);
			gaCharacter->EquipItems.Weapon = item;
			break;
		case(EGASlot::GATrinket) :
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
	if (gaCharacter->InventoryItems.Num() < gaCharacter->InventorySlots && item->finishedDropAnimation){
		AGAItem* inventoryItem = item;
		item->DestroyConstructedComponents();
		gaCharacter->InventoryItems.Add(inventoryItem);
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: PICKED UP ITEM ***"));
		EquipItem(item);
	}
	else UE_LOG(LogClass, Log, TEXT("*** PLAYER :: INVENTORY IS FULL ***"));
}

#pragma endregion

#pragma region Shop
// *** SYSTEM IS EXPERIMENTAL AND WIP ***

void AGAPlayerController::SellItem(AGAItem* item){
	gaCharacter->Ressource += item->Value;
	gaCharacter->InventoryItems.Remove(item);
	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: SOLD ITEM ***"));
	item->Destroy();
}
void AGAPlayerController::BuyItem(AGAItem* item){
	if (gaCharacter->Ressource - item->Value >= 0){
		gaCharacter->Ressource -= item->Value;
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: BAUGHT ITEM ***"));
	}
}

#pragma endregion