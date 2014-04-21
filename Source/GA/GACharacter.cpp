// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
#include "GAEnemy.h"
#include "GAPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GASpawnDestructible.h"

//////////////////////////////////////////////////////////////////////////
// AGACharacter

AGACharacter::AGACharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

	isInit = false;
	// Equip
	ItemDamage = 0;
	ItemHealth = 0;

	HasEquipedItem = false; 
	HasPickedUpItem = false;

	// Inventory
	InventorySlots = 10;

	// Attack Speed
	AttackSpeed = 1;

	// Simple Attack
	SimpleAttackOnCoolDown = false;

	SimpleAttackDamage = 25;
	SimpleAttackCoolDown = 0.75;

	// Special Attack
	SpecialAttackChargeTimer = 0;
	SpecialAttackTimesCharged = 0;
	SpecialAttackIsCharging = false;
	SpecialAttackOnCoolDown = false;

	SpecialAttackBaseDamage = 25;
	SpecialAttackMaxCharges = 5;
	SpecialAttackChargeInterval = 0.2;
	SpecialAttackCoolDown = 2;

	// Regeneration
	MaxHealth = 100;
	RegenerationTime = 0;
	RegenerationTimer = 0;
	AllowedToRegenerate = true;
	RegenerationAnimationIsRunning = false;

	HealthPoints = 100;
	OutOfCombatTime = 5;
	RegenerationRate = 1;
	RegenerationAmount = 5;	

	HasTookDamage = false;
	HasDied = false;
	

	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	CharacterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	CharacterMovement->JumpZVelocity = 600.f;
	CharacterMovement->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUseControllerViewRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUseControllerViewRotation = false; // Camera does not rotate relative to arm

	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGACharacter::InitPlayer(){
	SimpleAttackCoolDownRestValue = SimpleAttackCoolDown;
	SpecialAttackCoolDownRestValue = SpecialAttackCoolDown;
	MaxHealth = HealthPoints + ItemHealth;
	
	isInit = true;
}

void AGACharacter::Tick(float Delta){
	Super::Tick(Delta);
	if (!isInit){ InitPlayer();}

	ReduceSimpleAttackCoolDown(Delta);
	ReduceSpecialAttackCoolDown(Delta);
	IncreaseChargeTime(Delta);
	RegenerateHealth(Delta);
	CheckDeath();
}

void AGACharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	// Combat
	InputComponent->BindAction("AttackSimple", IE_Pressed, this, &AGACharacter::AttackSimple);
	InputComponent->BindAction("AttackSpecial", IE_Pressed, this, &AGACharacter::ChargeSpecial);
	InputComponent->BindAction("AttackSpecial", IE_Released, this, &AGACharacter::AttackSpecial);

	// Movement & Camera
	InputComponent->BindAxis("MoveForward", this, &AGACharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGACharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &AGACharacter::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AGACharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AGACharacter::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AGACharacter::LookUpAtRate);
}

#pragma region Movement


void AGACharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGACharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGACharacter::MoveForward(float Value)
{
	if (Controller != NULL && (Value != 0.0f) && isAllowedToMove())
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGACharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && isAllowedToMove())
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

#pragma endregion

#pragma region Simple Attack

void AGACharacter::AttackSimple(){
	if (Role < ROLE_Authority){
		ServerAttackSimple();
	}
	else {
		// Check If Attack Is On Cool Down
		if (SimpleAttackOnCoolDown) return;

		// Set Cool Down
		SimpleAttackOnCoolDown = true;

		// Find Enemy To Deal Damage
		for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			if (IsInRange(*ActorItr, SimpleAttackRange)){
				ActorItr->TakeDamageByEnemy(SimpleAttackDamage + ItemDamage);
			}
		}

		// Find Destructible To Deal Damage
		for (TActorIterator<AGASpawnDestructible> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			if (IsInRange(*ActorItr, SimpleAttackRange)){
				ActorItr->TakeDamageByEnemy(SimpleAttackDamage + ItemDamage);
			}
		}

		CharacterAttackedSimple();
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: ATTACKED SIMPLE (%f) ***"), SimpleAttackDamage + ItemDamage);
	}
}

void AGACharacter::ReduceSimpleAttackCoolDown(float Delta){
	if (Role < ROLE_Authority){
		ServerReduceSimpleAttackCoolDown(Delta);
	}
	else {
		// Reduce Cool Down
		if (!SimpleAttackOnCoolDown) return;

		SimpleAttackCoolDown -= Delta;

		// Check If Cool Down Finished
		if (SimpleAttackCoolDown <= 0){
			SimpleAttackOnCoolDown = false;
			SimpleAttackCoolDown = SimpleAttackCoolDownRestValue / AttackSpeed;

			UE_LOG(LogClass, Log, TEXT("*** SERVER :: ATTACK OFF COOLDOWN ***"));
		}
	}
}

bool AGACharacter::IsInRange(AActor* target, float attackRange){
	FVector playerLocation = GetActorLocation();
	FVector targetLocation = target->GetActorLocation();

	// Calculate Distance		*** WIP ***
	if (abs(playerLocation.X - targetLocation.X) < attackRange && abs(playerLocation.Y - targetLocation.Y) < attackRange){ return true; }
	return false;
}

#pragma endregion

#pragma region Special Attack

void AGACharacter::AttackSpecial(){
	if (Role < ROLE_Authority){
		ServerAttackSpecial();
	}
	else {
		// Check If Attack Is On Cool Down
		if (SpecialAttackOnCoolDown) return;

		// Set Cool Down
		SpecialAttackOnCoolDown = true;
		SpecialAttackIsCharging = false;

		float Damage = CalculateSpecialAttackDamage();

		// Find Actor To Deal Damage
		for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (IsInRange(*ActorItr, SpecialAttackRange)){
				ActorItr->TakeDamageByEnemy(Damage);
			}
		}

		SpecialAttackTimesCharged = 0;
		SpecialAttackChargeTimer = 0;
		CharacterAttackedSpecial();

		UE_LOG(LogClass, Log, TEXT("*** SERVER :: ATTACKED SPECIAL ***"));
	}
}

void AGACharacter::ChargeSpecial(){
	if (Role < ROLE_Authority){
		ServerChargeSpecial();
	}
	else{
		// Check If Attack Is On Cool Down
		if (SpecialAttackOnCoolDown) return;

		SpecialAttackIsCharging = true;
		CharacterStartedCharging();

		UE_LOG(LogClass, Log, TEXT("*** SERVER :: START CHARGING SPECIAL ***"));
	}
}

void AGACharacter::IncreaseChargeTime(float Delta){
	if (Role < ROLE_Authority){
		ServerIncreaseChargeTime(Delta);
	}
	else{
		// Check If Character Is Charging
		if (!SpecialAttackIsCharging) return;

		SpecialAttackChargeTimer += Delta;
		// Check If We Reached Charge Interval
		if (SpecialAttackChargeTimer >= SpecialAttackChargeInterval && SpecialAttackTimesCharged < SpecialAttackMaxCharges){
			SpecialAttackTimesCharged++;
			SpecialAttackChargeTimer = 0;
			CharacterIsCharging();
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: CHARGED  %d ***"), SpecialAttackTimesCharged);
		}
	}
}

float AGACharacter::CalculateSpecialAttackDamage(){
	return	SpecialAttackBaseDamage * SpecialAttackTimesCharged;;
}

void AGACharacter::ReduceSpecialAttackCoolDown(float Delta){
	if (Role < ROLE_Authority){
		ServerReduceSimpleAttackCoolDown(Delta);
	}
	else {
		if (SpecialAttackOnCoolDown){
			// Reduce Cool Down
			SpecialAttackCoolDown -= Delta;

			// Check If Cool Down Finished
			if (SpecialAttackCoolDown <= 0){
				SpecialAttackOnCoolDown = false;
				SpecialAttackCoolDown = SpecialAttackCoolDownRestValue;
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: SPECIAL OFF COOLDOWN ***"));
			}
		}
	}
}

bool AGACharacter::isAllowedToMove(){
	return !SpecialAttackIsCharging;
}

#pragma endregion

#pragma region Regeneration

void AGACharacter::RegenerateHealth(float Delta){
	if (Role < ROLE_Authority){
		ServerRegenerateHealth(Delta);
	}
	else{
		// Check If Out Of Combat
		if (!AllowedToRegenerate && HealthPoints != MaxHealth){
			RegenerationTimer += Delta;
			// Start Regeneration
			if (RegenerationTimer >= OutOfCombatTime) {
				AllowedToRegenerate = true;
				CharacterStartedRegeneration();
				RegenerationAnimationIsRunning = true;
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: STARTED REGENERATION ***"));
			}
			else return;
		}
		else {
			// Stop Regen If Max Health
			if (HealthPoints == MaxHealth && RegenerationAnimationIsRunning){
				AllowedToRegenerate = false;
				RegenerationTime = 0;
				RegenerationTimer = 0;
				CharacterFinishedRegeneration();
				RegenerationAnimationIsRunning = false;
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: STOPED REGENERATION ***"));
				return;
			}
			RegenerationTime += Delta;
			// Regenerate Health
			if(RegenerationTime >= RegenerationRate && HealthPoints < MaxHealth){
				HealthPoints = (HealthPoints + RegenerationAmount > MaxHealth ? MaxHealth : HealthPoints + RegenerationAmount);
				RegenerationTime = 0;
			}
		}
	}
}

#pragma endregion

#pragma region Take Damage

void AGACharacter::TakeDamageByEnemy(float Damage){
	if (Role < ROLE_Authority){
		ServerTakeDamageByEnemy(Damage);
	}
	else {
		AllowedToRegenerate = false;
		RegenerationAnimationIsRunning = false;
		RegenerationTimer = 0;
		RegenerationTime = 0;
		CharacterFinishedRegeneration();
		HealthPoints -= Damage;
		HasTookDamage = true;
		CharacterTookDamage();
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: TOOK DAMAGE ***"));
	}
}

#pragma endregion

#pragma region Check Death

void AGACharacter::CheckDeath(){
	if (Role < ROLE_Authority){
		ServerCheckDeath();
	}
	else{
		if (HealthPoints <= 0){
			HasDied = true;
			DestroyConstructedComponents();
			UE_LOG(LogClass, Warning, TEXT("*** SERVER :: DIED ***"));
		}
	}
}

#pragma endregion

#pragma region Items

void AGACharacter::CalculateItems(){
	if (Role < ROLE_Authority){
		ServerCalculateItems();
	}
	else {
		FPlayerAura PercentBonus;
		TArray <AGAItem*> EquipedItems;
		if (EquipItems.Head) EquipedItems.Add(EquipItems.Head);
		if (EquipItems.Chest) EquipedItems.Add(EquipItems.Chest);
		if (EquipItems.Weapon) EquipedItems.Add(EquipItems.Weapon);
		if (EquipItems.Trinket00) EquipedItems.Add(EquipItems.Trinket00);
		if (EquipItems.Trinket01) EquipedItems.Add(EquipItems.Trinket01);

		// Gather All Bonus
		for (int i = 0; i < EquipedItems.Num(); i++){
			PercentBonus.PercentDamage += EquipedItems[i]->AuraPlayer.PercentDamage;
			PercentBonus.PercentAttackSpeed += EquipedItems[i]->AuraPlayer.PercentAttackSpeed;
			PercentBonus.PercentHealth += EquipedItems[i]->AuraPlayer.PercentHealth;
		}

		// Damage Bonus
		ItemDamage += SimpleAttackDamage * PercentBonus.PercentDamage / 100;

		// Attack Speed
		AttackSpeed = 1 + 1 * PercentBonus.PercentAttackSpeed / 100;
		SimpleAttackCoolDown = SimpleAttackCoolDownRestValue / AttackSpeed;


		// Movement Speed

		// Armor

		// Health
		ItemHealth += HealthPoints * PercentBonus.PercentHealth / 100;
		MaxHealth = HealthPoints + ItemHealth;

		UE_LOG(LogClass, Log, TEXT("*** SERVER :: CALCULATED EQUIPED ITEMS ***"));
	}
}

void AGACharacter::EquipItem(AGAItem* item){
	if (Role < ROLE_Authority){
		ServerEquipItem(item);
	}
	else {
		switch (item->Slot){
		case(EGASlot::GAHead) :
			if (EquipItems.Head != NULL) InventoryItems.Add(EquipItems.Head);
			InventoryItems.Remove(item);
			EquipItems.Head = item;
			break;
		case(EGASlot::GAChest) :
			if (EquipItems.Chest != NULL) InventoryItems.Add(EquipItems.Chest);
			InventoryItems.Remove(item);
			EquipItems.Chest = item;
			break;
		case(EGASlot::GAWeapon) :
			if (EquipItems.Weapon != NULL) InventoryItems.Add(EquipItems.Weapon);
			InventoryItems.Remove(item);
			EquipItems.Weapon = item;
			break;
		case(EGASlot::GATrinket) :
			if (EquipItems.Trinket00 != NULL && EquipItems.Trinket01 != NULL) {
				InventoryItems.Add(EquipItems.Trinket00);
				InventoryItems.Remove(item);
				EquipItems.Trinket00 = item;
			}
			else if (EquipItems.Trinket00 == NULL) {
				InventoryItems.Remove(item);
				EquipItems.Trinket00 = item;
			}
			if (EquipItems.Trinket00 != NULL && EquipItems.Trinket01 == NULL) {
				InventoryItems.Remove(item);
				EquipItems.Trinket01 = item;
			}
			break;
		}
		CalculateItems();
		HasEquipedItem = true;
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: EQUIPED ITEM ***"));
	}
}



void AGACharacter::PickUpItem(AGAItem* item){
	if (Role < ROLE_Authority){
		ServerPickUpItem(item);
	}
	if(InventoryItems.Num() < InventorySlots && item->finishedDropAnimation){
		TouchedItem = item;
		HasPickedUpItem = true;
		if (item->IsMoney){
			Ressource += item->Value;
			item->DestroyConstructedComponents();
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: PICKED UP %f.2 Money (%f.2) ***"), item->Value, Ressource);
		}
		else{
			InventoryItems.Add(item);
			item->DestroyConstructedComponents();
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: PICKED UP ITEM ***"));
			EquipItem(item);
		}
	}
	else UE_LOG(LogClass, Log, TEXT("*** SERVER :: INVENTORY IS FULL ***"));
}

void AGACharacter::ReceiveActorBeginOverlap(class AActor* OtherActor){
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Item")){
		TouchedItem = (AGAItem*)OtherActor;
		HasPickedUpItem = false;
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: TOUCHED ITEM ***"));
		PickUpItem((AGAItem*)OtherActor);
	}
}

#pragma endregion

#pragma region Network - Simple Attack
void AGACharacter::OnRep_SimpleAttackOnCoolDown(){
	if (SimpleAttackOnCoolDown){
		CharacterAttackedSimple();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: ATTACKED SIMPLE (%f) ***"), SimpleAttackDamage + ItemDamage);
	}
	else {
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: ATTACK OFF COOLDOWN ***"));
	}
}

bool AGACharacter::ServerAttackSimple_Validate(){return true;}
void AGACharacter::ServerAttackSimple_Implementation(){AttackSimple();}

bool AGACharacter::ServerReduceSimpleAttackCoolDown_Validate(float DeltaTime){return true;}
void AGACharacter::ServerReduceSimpleAttackCoolDown_Implementation(float DeltaTime){ReduceSimpleAttackCoolDown(DeltaTime);}

#pragma endregion

#pragma region Network - Special Attack

void AGACharacter::OnRep_SpecialAttackOnCoolDown(){
	if (SpecialAttackOnCoolDown){
		CharacterAttackedSpecial();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: ATTACKED SPECIAL ***"));
	}
	else{
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: SPECIAL OFF COOLDOWN ***"));
	}
}

void AGACharacter::OnRep_SpecialAttackIsCharging(){
	if (SpecialAttackIsCharging){
		CharacterStartedCharging();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: START CHARGING SPECIAL ***"));
	}
}

void AGACharacter::OnRep_SpecialAttackTimesCharged(){
	if (SpecialAttackTimesCharged > 0){
		CharacterIsCharging();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: CHARGED  %d ***"), SpecialAttackTimesCharged);
	}
}

bool AGACharacter::ServerAttackSpecial_Validate(){return true;}
void AGACharacter::ServerAttackSpecial_Implementation(){AttackSpecial();}


bool AGACharacter::ServerReduceSpecialAttackCoolDown_Validate(float Delta){return true;}
void AGACharacter::ServerReduceSpecialAttackCoolDown_Implementation(float Delta){ReduceSpecialAttackCoolDown(Delta);}


bool AGACharacter::ServerChargeSpecial_Validate(){return true;}
void AGACharacter::ServerChargeSpecial_Implementation(){ChargeSpecial();}


bool AGACharacter::ServerIncreaseChargeTime_Validate(float Delta){return true;}
void AGACharacter::ServerIncreaseChargeTime_Implementation(float Delta){IncreaseChargeTime(Delta);}

#pragma endregion

#pragma  region Network - Regeneration

void AGACharacter::OnRep_AllowedToRegenerate(){
	if (AllowedToRegenerate){
		CharacterStartedRegeneration();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: STARTED REGENERATION ***"));
	}
	else{
		CharacterFinishedRegeneration();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: STOPED REGENERATION ***"));
	}
}

bool AGACharacter::ServerRegenerateHealth_Validate(float Delta){return true;}
void AGACharacter::ServerRegenerateHealth_Implementation(float Delta){ RegenerateHealth(Delta); }

#pragma endregion

#pragma region Network - Take Damage

void AGACharacter::OnRep_HasTookDamage(){
	if (HasTookDamage){
		CharacterTookDamage();
		CharacterFinishedRegeneration();
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: TOOK DAMAGE ***"));
		HasTookDamage = false;
	}
}

bool AGACharacter::ServerTakeDamageByEnemy_Validate(float Damage){return true;}
void AGACharacter::ServerTakeDamageByEnemy_Implementation(float Damage){TakeDamageByEnemy(Damage);}

#pragma endregion

#pragma region Network - Check Death

void AGACharacter::OnRep_HasDied(){
	if (HasDied){
		DestroyConstructedComponents();
		UE_LOG(LogClass, Warning, TEXT("*** CLIENT :: DIED ***"));
	}
}

bool AGACharacter::ServerCheckDeath_Validate(){return true;}
void AGACharacter::ServerCheckDeath_Implementation(){ CheckDeath(); }

#pragma endregion

#pragma region Network - Items

void AGACharacter::OnRep_HasPickedUpItem(){
	if (HasPickedUpItem){
		if (TouchedItem->IsMoney){
			Ressource += TouchedItem->Value;
			TouchedItem->DestroyConstructedComponents();
			UE_LOG(LogClass, Log, TEXT("*** CLIENT :: PICKED UP %f.2 Money (%f.2) ***"), TouchedItem->Value, Ressource);
		}
		else{
			TouchedItem->DestroyConstructedComponents();
			CharacterPickedUpItem();
			ServerResetHasPickedUpItem();
			UE_LOG(LogClass, Log, TEXT("*** CLIENT :: PICKED UP ITEM ***"));
		}
	}
}
void AGACharacter::OnRep_HasEquipedItem(){
	if (HasEquipedItem){
		CharacterEquipedItem();
		ServerResetHasEquipedItem();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: EQUIPED ITEM ***"));
	}
}

bool AGACharacter::ServerPickUpItem_Validate(AGAItem* item){return true;}
void AGACharacter::ServerPickUpItem_Implementation(AGAItem* item){PickUpItem(item);}

bool AGACharacter::ServerEquipItem_Validate(AGAItem* item){return true;}
void AGACharacter::ServerEquipItem_Implementation(AGAItem* item){EquipItem(item);}

bool AGACharacter::ServerCalculateItems_Validate(){return true;}
void AGACharacter::ServerCalculateItems_Implementation(){CalculateItems();}

bool AGACharacter::ServerResetHasPickedUpItem_Validate(){return true;}
void AGACharacter::ServerResetHasPickedUpItem_Implementation(){HasPickedUpItem = false;}

bool AGACharacter::ServerResetHasEquipedItem_Validate(){return true;}
void AGACharacter::ServerResetHasEquipedItem_Implementation(){HasEquipedItem = false;}

#pragma endregion

void AGACharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Simple Attack
	DOREPLIFETIME(AGACharacter, SimpleAttackDamage);
	DOREPLIFETIME(AGACharacter, SimpleAttackCoolDown);
	DOREPLIFETIME(AGACharacter, SimpleAttackCoolDownRestValue);
	DOREPLIFETIME(AGACharacter, SimpleAttackOnCoolDown);

	// Special Attack
	DOREPLIFETIME(AGACharacter, SpecialAttackChargeTimer);
	DOREPLIFETIME(AGACharacter, SpecialAttackTimesCharged);
	DOREPLIFETIME(AGACharacter, SpecialAttackMaxCharges);
	DOREPLIFETIME(AGACharacter, SpecialAttackCoolDownRestValue);
	DOREPLIFETIME(AGACharacter, SpecialAttackOnCoolDown);
	DOREPLIFETIME(AGACharacter, SpecialAttackIsCharging);

	// Player Stats
	DOREPLIFETIME(AGACharacter, HealthPoints);
	DOREPLIFETIME(AGACharacter, Armor);
	DOREPLIFETIME(AGACharacter, OutOfCombatTime);
	DOREPLIFETIME(AGACharacter, RegenerationAmount);
	DOREPLIFETIME(AGACharacter, RegenerationRate);
	DOREPLIFETIME(AGACharacter, AllowedToRegenerate);
	DOREPLIFETIME(AGACharacter, RegenerationTimer);
	DOREPLIFETIME(AGACharacter, MaxHealth);
	DOREPLIFETIME(AGACharacter, RegenerationTime);
	DOREPLIFETIME(AGACharacter, HasTookDamage);
	DOREPLIFETIME(AGACharacter, HasDied);

	// Items
	DOREPLIFETIME(AGACharacter, HasPickedUpItem);
	DOREPLIFETIME(AGACharacter, HasEquipedItem);
	DOREPLIFETIME(AGACharacter, EquipItems);
	DOREPLIFETIME(AGACharacter, InventoryItems);
	DOREPLIFETIME(AGACharacter, InventorySlots);
	DOREPLIFETIME(AGACharacter, TouchedItem);	

	// Ressource
	DOREPLIFETIME(AGACharacter, Ressource);
}