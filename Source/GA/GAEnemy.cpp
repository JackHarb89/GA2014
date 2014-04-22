// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAEnemy.h"
#include "GAAIController.h"
#include "GAPlayerController.h"
#include "GACharacter.h"
#include "Net/UnrealNetwork.h"


AGAEnemy::AGAEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Armor = 0;
	HealthPoints = 100;

	HasTookDamage = false;

	bUseControllerRotationYaw = true;
	PrimaryActorTick.bCanEverTick = true;

	// Simple Attack
	SimpleAttackDamageMin = 25;
	SimpleAttackDamageMax = 25;
	SimpleAttackCoolDown = 0.75;
	SimpleAttackCoolDownRestValue = SimpleAttackCoolDown;
	SimpleAttackOnCoolDown = false;

	// Replicate to Server / Clients
	bReplicates = true;
}

void AGAEnemy::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	ReduceSimpleAttackCoolDown(DeltaTime);
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

bool AGAEnemy::DealDamage(){
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
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: ATTACKED SIMPLE ***"));
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
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: ATTACK OFF COOL DOWN ***"));
			SimpleAttackCoolDown = SimpleAttackCoolDownRestValue;
		}
	}
}

// Only Client
bool AGAEnemy::IsInRange(AActor* target){
	FVector playerLocation = GetActorLocation();
	FVector targetLocation = target->GetActorLocation();

	// Calculate Distance		*** WIP ***
	if (abs(playerLocation.X - targetLocation.X) < 100 && abs(playerLocation.Y - targetLocation.Y) < 100){ return true; }
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
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;

		FVector SpawnLocation;
		FRotator SpawnRotation;

		SpawnLocation = this->GetActorLocation();
		SpawnRotation = this->GetActorRotation();

		AGAItem* DropedItem = World->SpawnActor<AGAItem>(item, SpawnLocation, SpawnRotation, SpawnParams);
		if (DropedItem != NULL) {
			switch (DropedItem->Slot){
				case(EGASlot::GAMoney) :
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED MONEY (%f) ***"), DropedItem->Value);
					break;
				case(EGASlot::GAHead) :
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED HEAD ***"));
					break;
				case(EGASlot::GAChest) :
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED CHEST ***"));
					break;
				case(EGASlot::GATrinket) :
					UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DROPED TRINKET ***"));
					break;
				case(EGASlot::GAWeapon) :
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
		HealthPoints -= (Damage - (Armor*0.25*Damage)/100);
		HasTookDamage = true;
		CharacterTookDamage();
		if (HealthPoints <= 0){
			CheckItemDrop();
			this->Destroy();
		}
	}
}

#pragma endregion

#pragma region Network - Attack Simple

void AGAEnemy::OnRep_SimpleAttackOnCoolDown(){
	if (SimpleAttackOnCoolDown){
		CharacterAttackedSimple();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: ATTACKED SIMPLE ***"));
	}
	else{
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: ATTACK OFF COOL DOWN ***"));
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
	
	DOREPLIFETIME(AGAEnemy, SimpleAttackOnCoolDown);
	DOREPLIFETIME(AGAEnemy, SimpleAttackCoolDownRestValue);
	DOREPLIFETIME(AGAEnemy, SimpleAttackDamageMin);
	DOREPLIFETIME(AGAEnemy, SimpleAttackDamageMax);
	DOREPLIFETIME(AGAEnemy, SimpleAttackCoolDown);

	DOREPLIFETIME(AGAEnemy, EnemyBehavior);
}