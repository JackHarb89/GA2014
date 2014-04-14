// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
#include "GAEnemy.h"

//////////////////////////////////////////////////////////////////////////
// AGACharacter

AGACharacter::AGACharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	isInit = false;

	// Equip
	ItemDamage = 0;
	ItemHealth = 0;

	// Inventory
	InventorySlots = 10;

	// Simple Attack
	SimpleAttackDamage = 25;
	SimpleAttackCoolDown = 0.75;
	SimpleAttackCoolDownRestValue = SimpleAttackCoolDown;
	SimpleAttackOnCoolDown = false;

	// Special Attack
	SpecialAttackBaseDamage = 25;
	SpecialAttackMaxDamage = 100;
	SpecialAttackChargeInterval = 0.2;
	SpecialAttackCoolDown = 5;
	SpecialAttackCoolDownRestValue = SpecialAttackCoolDown;
	SpecialAttackChargeTimer = 0;
	SpecialAttackTimesCharged = 0;
	SpecialAttackIsCharging = false;
	SpecialAttackOnCoolDown = false;

	// Player Stats
	HealthPoints = 100;
	OutOfCombatTime = 5;
	RegenerationRate = 1;
	RegenerationAmount = 5;
	RegenerationTime = 0;

	MaxHP = 100;
	AllowedToRegenerate = true;
	RegenerationTimer = 0;

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AGACharacter::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	if (!isInit) InitPlayer();

	// Simple Attack
	ReduceSimpleAttackCoolDown(DeltaTime);

	// Special Attack
	IncreaseChargeTime(DeltaTime);
	ReduceSpecialAttackCoolDown(DeltaTime);

	// Regenrate HP
	RegenerateHP(DeltaTime);

	CheckDeath();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGACharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("AttackSimple", IE_Pressed, this, &AGACharacter::AttackSimple);
	InputComponent->BindAction("AttackSpecial", IE_Pressed, this, &AGACharacter::ChargeSpecial);
	InputComponent->BindAction("AttackSpecial", IE_Released, this, &AGACharacter::AttackSpecial);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAxis("MoveForward", this, &AGACharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGACharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AGACharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AGACharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGACharacter::TouchStarted);
}


void AGACharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

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
	if ((Controller != NULL) && (Value != 0.0f) && !SpecialAttackIsCharging)
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
	if ((Controller != NULL) && (Value != 0.0f) && !SpecialAttackIsCharging)
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

void AGACharacter::AttackSimple(){
	// Check If Attack Is On Cool Down
	if (SimpleAttackOnCoolDown) return;

	// Set Cool Down
	SimpleAttackOnCoolDown = true;

	// Find Actor To Deal Damage
	for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (IsInRange(*ActorItr)){
			ActorItr->TakeDamageByEnemy(SimpleAttackDamage + ItemDamage);
		}
	}
	CharacterAttackedSimple();

	UE_LOG(LogClass, Log, TEXT("*** PLAYER:: ATTACKED SIMPLE ***"));
	UE_LOG(LogClass, Log, TEXT("*** ATTACK:: %f DAMAGE ***"), SimpleAttackDamage + ItemDamage);
}

void AGACharacter::ReduceSimpleAttackCoolDown(float DeltaTime){
	// Reduce Cool Down
	if(SimpleAttackOnCoolDown) SimpleAttackCoolDown -= DeltaTime;
	// Check If Cool Down Finished
	if(SimpleAttackCoolDown <= 0){
		SimpleAttackOnCoolDown = false;
		SimpleAttackCoolDown = SimpleAttackCoolDownRestValue;
	}
}

void AGACharacter::ChargeSpecial(){
	// Check If Attack Is On Cool Down
	if (SpecialAttackOnCoolDown) return;

	SpecialAttackIsCharging = true;
	CharacterStartedCharging();

	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: START CHARGING SPECIAL ***"));
}

void AGACharacter::AttackSpecial(){
	// Check If Attack Is On Cool Down
	if (SpecialAttackOnCoolDown) return;

	// Set Cool Down
	SpecialAttackOnCoolDown = true;
	SpecialAttackIsCharging = false;

	float Damage = CalculateSpecialAttackDamage();

	// Find Actor To Deal Damage
	for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (IsInRange(*ActorItr)){
			ActorItr->TakeDamageByEnemy(Damage);
		}
	}

	SpecialAttackTimesCharged = 0;
	SpecialAttackChargeTimer = 0;
	CharacterAttackedSpecial();

	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: ATTACKED SPECIAL***"));
}

void AGACharacter::IncreaseChargeTime(float DeltaTime){
	// Check If Character Is Charging
	if(!SpecialAttackIsCharging) return;

	SpecialAttackChargeTimer += DeltaTime;
	// Check If We Reached Charge Interval
	if (SpecialAttackChargeTimer >= SpecialAttackChargeInterval){
		SpecialAttackTimesCharged++;
		SpecialAttackChargeTimer = 0;
		CharacterIsCharging();
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: CHARGED ***"));
	}
}

float AGACharacter::CalculateSpecialAttackDamage(){
	float Damage;
	Damage = SpecialAttackBaseDamage * SpecialAttackTimesCharged;
	Damage = (Damage > SpecialAttackMaxDamage ? SpecialAttackMaxDamage : Damage);
	
	return Damage;
}

void AGACharacter::ReduceSpecialAttackCoolDown(float DeltaTime){
	// Reduce Cool Down
	if (SpecialAttackOnCoolDown) SpecialAttackCoolDown -= DeltaTime;
	// Check If Cool Down Finished
	if (SpecialAttackCoolDown <= 0){
		SpecialAttackOnCoolDown = false;
		SpecialAttackCoolDown = SpecialAttackCoolDownRestValue;
	}
}

bool AGACharacter::IsInRange(AActor* target){
	FVector playerLocation = GetActorLocation();
	FVector targetLocation = target->GetActorLocation();

	// Calculate Distance		*** WIP ***
	if (abs(playerLocation.X - targetLocation.X) < 250 && abs(playerLocation.Y - targetLocation.Y) < 250){ return true; }
	return false;
}

void AGACharacter::TakeDamageByEnemy(float Damage){
	HealthPoints -= Damage;
	CharacterTookDamage();
	if (HealthPoints <= 0){
		this->Destroy();
	}
	AllowedToRegenerate = false;
	RegenerationTimer = 0;
}

void AGACharacter::CheckDeath(){
	if (HealthPoints <= 0){
		UE_LOG(LogClass, Warning, TEXT("*** PLAYER DIED ***"));
	}
}

void AGACharacter::RegenerateHP(float DeltaTime){
	if (RegenerationTimer < OutOfCombatTime){
		RegenerationTimer += DeltaTime;
		if (RegenerationTimer >= OutOfCombatTime) AllowedToRegenerate = true;
		else return;
	}
	RegenerationTime += DeltaTime;

	if (AllowedToRegenerate && RegenerationTime >= RegenerationRate){
		HealthPoints = (HealthPoints + RegenerationAmount > MaxHP ? MaxHP : HealthPoints + RegenerationAmount);
		RegenerationTime = 0;
		CharacterRegenerated();
	}
}

void AGACharacter::InitPlayer(){
	isInit = true;
	MaxHP = HealthPoints + ItemHealth;
}

void AGACharacter::CalculateItem(AGAItem* item){
	// Attack Damage
	float percentDamage = item->AuraPlayer.PercentDamage;
	ItemDamage += SimpleAttackDamage*percentDamage / 100;

	// Armor

	// Health
	float percentLife = item->AuraPlayer.PercentHealth;
	ItemHealth += HealthPoints*percentLife / 100;
	MaxHP = HealthPoints + ItemHealth;
}

void AGACharacter::EquipItem(AGAItem* item){
	EquipItems.Add(item);
	InventoryItems.Remove(item);
	CalculateItem(item);
	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: EQUIPED ITEM ***"));
}

void AGACharacter::PickUpItem(AGAItem* item){
	if (InventoryItems.Num() < InventorySlots){
		InventoryItems.Add(item);
		EquipItem(item);
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: PICKED UP ITEM ***"));
	}
	else UE_LOG(LogClass, Log, TEXT("*** PLAYER :: INVENTORY IS FULL ***"));
}

void AGACharacter::ReceiveActorBeginOverlap(class AActor* OtherActor){
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Item")){
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: TOUCHED ITEM ***"));
		PickUpItem((AGAItem*)OtherActor);
	}
}