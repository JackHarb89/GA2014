// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
#include "GAEnemy.h"
#include "GAPlayerController.h"
#include "GAGameState.h"
#include "GA_HUD.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// AGACharacter

AGACharacter::AGACharacter(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{

	FootStepTime = 0.38;
	FootStepTimer = 0;

	IsAllowedToChannelShard = false;
	ShardTimer = 0;
	ShardActivationTime = 2;

	GAUserName = "Anonymous";

	//static ConstructorHelpers::FObjectFinder<UBlueprint> Spectator_HUD(TEXT("/Game/UI/Classes/GA_SpectatorHUD"));
	SpectatorHUD = AGA_HUD::StaticClass();// UClass*)Spectator_HUD.Object->GeneratedClass;

	CurrenSpecatorPlayerIndex = 0;
	ShardAvailable = true;
	IsPowerUpActive = false;

	isInit = false;
	WeaponActor = nullptr;
	IsSimpleAttacking = false;
	IsSpecialAttacking = false;

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
	SpecialAttackTimesCharged = 1; 
	StoredCharges = 1;
	SpecialAttackIsCharging = false;
	SpecialAttackOnCoolDown = false;

	SpecialAttackBaseDamage = 10;
	SpecialAttackMaxCharges = 5;
	SpecialAttackChargeInterval = 0.2;
	SpecialAttackCoolDown = 10;

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

	ArmorReductionPercent = 0.25;

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

	// Shop
	static ConstructorHelpers::FObjectFinder<UBlueprint> ShopBP(TEXT("/Game/UI/Classes/Shop.Shop"));
	ShopClass = (UClass*)ShopBP.Object->GeneratedClass;
}

// Initalize Player - Setting Reset Values
void AGACharacter::InitPlayer(){
	if (GetLevel()->OwningWorld->GetName().Contains("SG_Game")){
		CharacterSpawned();
	}
	
	// Set Reset Values
	SimpleAttackCoolDownResetValue = SimpleAttackCoolDown;
	SpecialAttackCoolDownResetValue = SpecialAttackCoolDown;
	HealthResetValue = HealthPoints;
	MaxHealth = HealthResetValue + ItemHealth;
	BaseMovementSpeed = CharacterMovement->MaxWalkSpeed;


	// Shop
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.bNoCollisionFail = true;

	FVector SpawnLocation = { 0, 0, 0 };
	FRotator SpawnRotation = { 0, 0, 0 };

	Shop = GetWorld()->SpawnActor<AGAShop>(ShopClass, SpawnLocation, SpawnRotation, SpawnParams);

	Tags.Add("Player");

	isInit = true;
}

void AGACharacter::Tick(float Delta){
	Super::Tick(Delta);
	if (!isInit){ InitPlayer(); }
	ReduceSimpleAttackCoolDown(Delta);
	ReduceSpecialAttackCoolDown(Delta);
	IncreaseChargeTime(Delta);
	ReducePowerUpDuration(Delta);
	ChannelShard(Delta);
	if (!HasDied) CheckDeath();
	if (Role == ROLE_Authority){GetWorld()->GetGameState<AGAGameState>()->CheckDeatchCondition();}
	FootStepTimer += Delta;
}

void AGACharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	if (GetLevel()->OwningWorld->GetName().Contains("SG_Game")){
		// Set up gameplay key bindings
		check(InputComponent);
		// Shard
		InputComponent->BindAction("ActivateShard", IE_Pressed, this, &AGACharacter::StartShardChanneling);

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
	if (Controller != NULL && (Value != 0.0f) && isAllowedToMove() && !HasDied)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		if (FootStepTimer >= FootStepTime){
			for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				(*ActorItr)->CharacterWalked(this);
			}
			FootStepTimer = 0;
		}
	}
}

void AGACharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && isAllowedToMove() && !HasDied)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);

		if (FootStepTimer >= FootStepTime){
			for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				(*ActorItr)->CharacterWalked(this);
			}
			FootStepTimer = 0;
		}
	}
}

#pragma endregion

#pragma region Spectating

void AGACharacter::UnmapKeybindings(){
	if (!InputComponent) return;
	for (int32 BindingIndex = InputComponent->GetNumActionBindings() - 1; BindingIndex >= 0; --BindingIndex){
		InputComponent->RemoveActionBinding(BindingIndex);
	}

	InputComponent->BindAction("AttackSimple", IE_Pressed, this, &AGACharacter::SpectateNextPlayer);
	InputComponent->BindAction("AttackSpecial", IE_Pressed, this, &AGACharacter::SpectatePrevPlayer);
}

void AGACharacter::SpectateNextPlayer(){
	if (CurrentPlayers.Num() == 0)
	{
		return;
	}
	CurrenSpecatorPlayerIndex++;
	if (CurrenSpecatorPlayerIndex > CurrentPlayers.Num() - 1){
		CurrenSpecatorPlayerIndex = 0;
	}

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom->DetachFromParent();
	CameraBoom->AttachTo(CurrentPlayers[CurrenSpecatorPlayerIndex]->RootComponent);
}

void AGACharacter::SpectatePrevPlayer(){
	if (CurrentPlayers.Num() == 0)
	{
		return;
	}
	CurrenSpecatorPlayerIndex--;
	if (CurrenSpecatorPlayerIndex<0){
		CurrenSpecatorPlayerIndex = CurrentPlayers.Num() - 1;
	}

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom->DetachFromParent();
	CameraBoom->AttachTo(CurrentPlayers[CurrenSpecatorPlayerIndex]->RootComponent);
}

#pragma endregion

#pragma region Simple Attack

void AGACharacter::SetIsSimpleAttackingTo(bool NewState){
	IsSimpleAttacking = NewState;	
	if (!IsSimpleAttacking){
		WeaponActor->RemoveHitedActors();
	}
}

void AGACharacter::SetIsSpecialAttackingTo(bool NewState){
	IsSpecialAttacking = NewState;
	if (!IsSpecialAttacking){
		TArray <AActor*> Actors;
	}
}

void AGACharacter::DealDamage(class AActor* OtherActor){
	if (Role < ROLE_Authority){
		ServerDealDamage(OtherActor);
	}
	else{
		float Damage, Range;

		// Damage & Range Calculation
		if (IsSimpleAttacking){
			Damage = SimpleAttackDamage + ItemDamage;
			Range = SimpleAttackRange;
			CharacterAppliedSimpleForce();
			WeaponActor->HitedActors.Add(OtherActor);
		}
		else if (IsSpecialAttacking){
			Damage = CalculateSpecialAttackDamage();
			Range = SpecialAttackRange;
			CharacterAppliedSpecialForce();
			WeaponActor->HitedActors.Add(OtherActor);

			StoredCharges = 1;

			SpecialAttackChargeTimer = 0;
			SpecialAttackTimesCharged = 1;
		}
		else return;

		// Critical Hit
		float random = FMath::RandRange(0, 100);
		if (FMath::Max3(float(0), random, Critical) == Critical) Damage *= 2;

		if (IsOneAttackKill){
			Damage = -1;					// One Hit Kill 
		}
		// Deal Damage
		((AGAAttackableCharacter*)OtherActor)->TakeDamageByEnemy(Damage);

		UE_LOG(LogClass, Log, TEXT("*** WEAPON :: DEALT DAMAGE (%.1f)  (ServerCall) ***"), Damage);
	}
}

// Simple Attack - Call This Function If The Player Should Attack Normal
void AGACharacter::AttackSimple(){
	if (!isAllowedToMove()){
		return;
	}
	if (Role < ROLE_Authority){
		ServerAttackSimple();
	}
	else {
		// Check If Attack Is On Cool Down
		if (SimpleAttackOnCoolDown) return;

		// Set Cool Down
		SimpleAttackOnCoolDown = true;

		CharacterAttackedSimple();

		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->CharacterAttackedSimple(this);
		}

		UE_LOG(LogClass, Log, TEXT("*** %s :: ATTACKED SIMPLE (ServerCall) ***"),*GetName());
	}
}

// Reduces The Simple Attack Cool Down  - Called by Tick
void AGACharacter::ReduceSimpleAttackCoolDown(float Delta){
	if (Role == ROLE_Authority){
		if (IsSimpleAttacking){
			WeaponActor->UpdateOverlaps(false);
		}
		// Reduce Cool Down
		if (!SimpleAttackOnCoolDown) return;

		SimpleAttackCoolDown -= Delta;

		// Check If Cool Down Finished
		if (SimpleAttackCoolDown <= 0){
			SimpleAttackOnCoolDown = false;
			SimpleAttackCoolDown = SimpleAttackCoolDownResetValue / AttackSpeed;

			UE_LOG(LogClass, Log, TEXT("*** %s :: ATTACK OFF COOLDOWN (ServerCall) ***"), *GetName());
		}
	}
}

// Range Check Of A Target And A Given Range - Called By Attack Simple and Attack Special
bool AGACharacter::IsInRange(AActor* target, float attackRange){
	FVector playerLocation = GetActorLocation();
	FVector targetLocation = target->GetActorLocation();

	if (FVector::Dist(playerLocation, targetLocation) <= attackRange) return true;
	return false;
}

bool AGACharacter::ServerDealDamage_Validate(class AActor* OtherActor){ return true; }
void AGACharacter::ServerDealDamage_Implementation(class AActor* OtherActor){ DealDamage(OtherActor); }

// Client Reaction On Replication Notification - Simple Event Call
void AGACharacter::OnRep_SimpleAttackOnCoolDown(){
	if (SimpleAttackOnCoolDown){
		CharacterAttackedSimple();
		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->CharacterAttackedSimple(this);
		}
		UE_LOG(LogClass, Log, TEXT("*** %s :: ATTACKED SIMPLE (Client RepNotify) ***"), *GetName());
	}
	else {
		UE_LOG(LogClass, Log, TEXT("*** %s :: ATTACK OFF COOLDOWN (Client RepNotify) ***"), *GetName());
	}
}

bool AGACharacter::ServerAttackSimple_Validate(){ return true; }
void AGACharacter::ServerAttackSimple_Implementation(){ AttackSimple(); }

bool AGACharacter::ServerReduceSimpleAttackCoolDown_Validate(float DeltaTime){ return true; }
void AGACharacter::ServerReduceSimpleAttackCoolDown_Implementation(float DeltaTime){ ReduceSimpleAttackCoolDown(DeltaTime); }

#pragma endregion

#pragma region Special Attack

// Special Attack - Call This Function If The Player Should Attack Special
void AGACharacter::AttackSpecial(){
	if (IsAllowedToChannelShard){
		return;
	}
	if (Role < ROLE_Authority){
		ServerAttackSpecial();
	}
	else {
		// Set Cool Down
		if (!SpecialAttackIsCharging) return;

		SpecialAttackOnCoolDown = true;
		SpecialAttackIsCharging = false;

		StoredCharges = SpecialAttackTimesCharged;
		SpecialAttackChargeTimer = 0;
		SpecialAttackTimesCharged = 1;

		CharacterAttackedSpecial();

		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->CharacterAttackedSpecial(this);
		}
		UE_LOG(LogClass, Log, TEXT("*** %s :: ATTACKED SPECIAL (ServerCall) ***") ,*GetName());
	}
}

// Function Charges Special Attack
void AGACharacter::ChargeSpecial(){
	if (IsAllowedToChannelShard){
		return; 
	}
	if (Role < ROLE_Authority){
		ServerChargeSpecial();
	}
	else{
		if (!SpecialAttackOnCoolDown && !SpecialAttackIsCharging){
			SpecialAttackOnCoolDown = true;
			SpecialAttackIsCharging = true;
			CharacterStartedCharging();
			UE_LOG(LogClass, Log, TEXT("*** %s :: START CHARGING SPECIAL (ServerCall) ***"), *GetName());
			UE_LOG(LogClass, Log, TEXT("*** %s :: CHARGED  %d (ServerCall) ***"), *GetName(), SpecialAttackTimesCharged);
		}

	}
}

//  Function Increases The Special Attack Charge Timer - Called By Tick 
void AGACharacter::IncreaseChargeTime(float Delta){
	if (Role == ROLE_Authority){
		// Check If Character Is Charging
		if (!SpecialAttackIsCharging) return;

		SpecialAttackChargeTimer += Delta;
		// Check If We Reached Charge Interval
		if (SpecialAttackChargeTimer >= SpecialAttackChargeInterval && SpecialAttackTimesCharged < SpecialAttackMaxCharges){
			SpecialAttackTimesCharged++;
			SpecialAttackChargeTimer = 0;
			CharacterIsCharging();
			UE_LOG(LogClass, Log, TEXT("*** %s :: CHARGED  %d (ServerCall) ***"), *GetName(), SpecialAttackTimesCharged);
		}
	}
}

// Calculation For The Special Attack (Depends on Times Charged and Base Damage)
float AGACharacter::CalculateSpecialAttackDamage(){
	return SpecialAttackBaseDamage * StoredCharges;
}

// Reduces The Special Attack Cool Down  - Called by Tick
void AGACharacter::ReduceSpecialAttackCoolDown(float Delta){
	if (Role == ROLE_Authority){
		if (IsSpecialAttacking){
			WeaponActor->UpdateOverlaps(true);
		}
		if (SpecialAttackOnCoolDown){
			// Reduce Cool Down
			SpecialAttackCoolDown -= Delta;

			// Check If Cool Down Finished
			if (SpecialAttackCoolDown <= 0){
				SpecialAttackOnCoolDown = false;
				SpecialAttackCoolDown = SpecialAttackCoolDownResetValue;
				UE_LOG(LogClass, Log, TEXT("*** %s :: SPECIAL OFF COOLDOWN (ServerCall) ***"), *GetName());
			}
		}
	}
}

// If Player Is Charging Special Attack He Is Not Allowed To Move
bool AGACharacter::isAllowedToMove(){
	return !SpecialAttackIsCharging && !IsAllowedToChannelShard;
}

bool AGACharacter::IsCharging(){
	return SpecialAttackIsCharging;
}

// Client Reaction On Replication Notification - Simple Event Call
void AGACharacter::OnRep_SpecialAttackOnCoolDown(){
	if (!SpecialAttackOnCoolDown){
		UE_LOG(LogClass, Log, TEXT("*** %s :: SPECIAL OFF COOLDOWN (ClientCall) ***"), *GetName());
	}
}

// Client Reaction On Replication Notification - Simple Event Call
void AGACharacter::OnRep_SpecialAttackIsCharging(){
	if (SpecialAttackIsCharging){
		CharacterStartedCharging();
		UE_LOG(LogClass, Log, TEXT("*** %s :: START CHARGING SPECIAL (ClientCall) ***"), *GetName());
	}
	else{
		CharacterAttackedSpecial();
		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->CharacterAttackedSpecial(this);
		}
		UE_LOG(LogClass, Log, TEXT("*** %s :: ATTACKED SPECIAL (ClientCall) ***"), *GetName());
	}
}

// Client Reaction On Replication Notification - Simple Event Call
void AGACharacter::OnRep_SpecialAttackTimesCharged(){
	if (SpecialAttackTimesCharged > 0){
		CharacterIsCharging();
		UE_LOG(LogClass, Log, TEXT("*** %s :: CHARGED  %d (ClientCall) ***"), *GetName(), SpecialAttackTimesCharged);
	}
}

bool AGACharacter::ServerAttackSpecial_Validate(){ return true; }
void AGACharacter::ServerAttackSpecial_Implementation(){ AttackSpecial(); }


bool AGACharacter::ServerReduceSpecialAttackCoolDown_Validate(float Delta){ return true; }
void AGACharacter::ServerReduceSpecialAttackCoolDown_Implementation(float Delta){ ReduceSpecialAttackCoolDown(Delta); }


bool AGACharacter::ServerChargeSpecial_Validate(){ return true; }
void AGACharacter::ServerChargeSpecial_Implementation(){ ChargeSpecial(); }


bool AGACharacter::ServerIncreaseChargeTime_Validate(float Delta){ return true; }
void AGACharacter::ServerIncreaseChargeTime_Implementation(float Delta){ IncreaseChargeTime(Delta); }

#pragma endregion

#pragma region Take Damage

// Player Take Damage By Enemy For The Given Amount (Reduced By Armor)
void AGACharacter::TakeDamageByEnemy(float Damage) {
	if (Role < ROLE_Authority) {
		ServerTakeDamageByEnemy(Damage);
	}
	else {
		if (!IsInvulnerable){
			ApplyDamage(Damage);
		}
	}
}

void AGACharacter::ApplyDamage(float Damage) {
	AllowedToRegenerate = false;
	RegenerationAnimationIsRunning = false;
	RegenerationTimer = 0;
	RegenerationTime = 0;
	HealthPoints -= (Damage - (Armor * ArmorReductionPercent * Damage) / 100);
	CharacterFinishedRegeneration();
	CharacterTookDamage();
	HasTookDamage = true;
	UE_LOG(LogClass, Log, TEXT("*** SERVER :: TOOK DAMAGE ***"));
}


// Client Reaction On Replication Notification - Only React If True
void AGACharacter::OnRep_HasTookDamage(){
	if (HasTookDamage){
		CharacterFinishedRegeneration();
		CharacterTookDamage();
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: TOOK DAMAGE ***"));
		HasTookDamage = false;
	}
}

bool AGACharacter::ServerTakeDamageByEnemy_Validate(float Damage){ return true; }
void AGACharacter::ServerTakeDamageByEnemy_Implementation(float Damage){ TakeDamageByEnemy(Damage); }

#pragma endregion

#pragma region Check Death

// Checks The Health Amount And Kill The Player If 0 Or Below
void AGACharacter::CheckDeath(){
	if (Role < ROLE_Authority){
		ServerCheckDeath();
	}
	else{
		if (HealthPoints <= 0){
			HasDied = true;

			for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				if (!ActorItr->HasDied){
					CurrentPlayers.Add(*ActorItr);
				}
			}
			UnmapKeybindings();
			CharacterDied();

			for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				(*ActorItr)->CharacterDied(this);
			}

			UE_LOG(LogClass, Warning, TEXT("*** SERVER :: DIED ***"));
		}
	}
}



// Client Reaction On Replication Notification - Only React If True
void AGACharacter::OnRep_HasDied(){
	if (HasDied){
		UnmapKeybindings();
		CharacterDied();
		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->CharacterDied(this);
		}
		UE_LOG(LogClass, Warning, TEXT("*** CLIENT :: DIED ***"));
	}
}

bool AGACharacter::ServerCheckDeath_Validate(){ return true; }
void AGACharacter::ServerCheckDeath_Implementation(){ CheckDeath(); }


#pragma endregion

#pragma region Power Up

void AGACharacter::HealPlayer(float HealAmount){
	if (Role < ROLE_Authority){
		ServerHealPlayer(HealAmount);
	}
	else{
		if (HealthPoints < MaxHealth){
			HealthPoints += HealAmount;
			HasBeenHealed = !HasBeenHealed;
			if (HealthPoints > MaxHealth){ HealthPoints = MaxHealth;}
			CharacterStartedRegeneration();
		}
	}
}


void AGACharacter::ActivatePowerUp(EGAPowerUp::Type PowerUpType, float EffectDuration){
	if (Role < ROLE_Authority){
		ServerActivatePowerUp(PowerUpType, EffectDuration);
	}
	else {
		IsPowerUpActive = true;
		ActivePowerUp = PowerUpType;
		PowerUpDuration = EffectDuration;

		if (ActivePowerUp == EGAPowerUp::GAInvulnerability){
			IsInvulnerable = true;
		}
		else if (ActivePowerUp == EGAPowerUp::GAAttackBoost){
			IsOneAttackKill = true;
		}
		CharacterActivatedPowerUp(PowerUpType);
		UE_LOG(LogClass, Log, TEXT("SERVER: *** ACTIVATED PLAYER POWERUP ***"));
	}
}

void AGACharacter::DeactivatePowerUp(){
	if (Role < ROLE_Authority){
		ServerDeactivatePowerUp();
	}
	else {
		IsPowerUpActive = false;
		IsInvulnerable = false;
		IsOneAttackKill = false;
		CharacterDeactivatedPowerUp();
		UE_LOG(LogClass, Log, TEXT("SERVER: *** DEACTIVATED PLAYER POWERUP ***"));
	}
}

void AGACharacter::ReducePowerUpDuration(float DeltaTime){
	if (Role == ROLE_Authority){
		if (IsPowerUpActive){
			PowerUpDuration -= DeltaTime;
			if (PowerUpDuration <= 0){
				DeactivatePowerUp();
			}
		}
	}
}

void AGACharacter::OnRep_IsPowerUpActive(){
	if (IsPowerUpActive){
		CharacterActivatedPowerUp(ActivePowerUp);
		UE_LOG(LogClass, Log, TEXT("SERVER: *** ACTIVATED PLAYER POWERUP ***"));
	}
	else {
		CharacterDeactivatedPowerUp();
		UE_LOG(LogClass, Log, TEXT("SERVER: *** DEACTIVATED PLAYER POWERUP ***"));
	}
}

void AGACharacter::OnRep_HasBeenHealed(){
	CharacterStartedRegeneration();
}

bool AGACharacter::ServerHealPlayer_Validate(float HealAmount){return true;}
void AGACharacter::ServerHealPlayer_Implementation(float HealAmount){HealPlayer(HealAmount);}

bool AGACharacter::ServerActivatePowerUp_Validate(EGAPowerUp::Type PowerUpType, float EffectDuration){return true;}
void AGACharacter::ServerActivatePowerUp_Implementation(EGAPowerUp::Type PowerUpType, float EffectDuration){ActivatePowerUp(PowerUpType, EffectDuration);}

bool AGACharacter::ServerDeactivatePowerUp_Validate(){return true;}
void AGACharacter::ServerDeactivatePowerUp_Implementation(){DeactivatePowerUp();}


#pragma endregion

#pragma region Shard Usage

void AGACharacter::StartShardChanneling(){
	if (SpecialAttackIsCharging){
		return;
	}
	if (Role < ROLE_Authority){
		ServerStartShardChanneling();
	}
	else{
		if (ShardAvailable){
			CharacterStartedChannelingShard();
			for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				ActorItr->CharacterStartedShardChanneling(this);
				ActorItr->FadeOutBattle();
			}
			IsAllowedToChannelShard = true;
			UE_LOG(LogClass, Log, TEXT("*** STARTING CHANNELING SHARD"));
		}
	}
}


bool AGACharacter::ServerStartShardChanneling_Validate(){ return true; }
void AGACharacter::ServerStartShardChanneling_Implementation(){ StartShardChanneling(); }

void AGACharacter::ChannelShard(float DeltaTime){
	if (Role == ROLE_Authority){
		if (IsAllowedToChannelShard){
			ShardTimer += DeltaTime;
			if (ShardTimer >= ShardActivationTime){
				IsAllowedToChannelShard = false;
				ActivateShard();
			}
		}
	}
}

void AGACharacter::OnRep_IsAllowedToChannelShard(){
	if (IsAllowedToChannelShard){
		CharacterStartedChannelingShard();
		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			ActorItr->CharacterStartedShardChanneling(this);
			ActorItr->FadeOutBattle();
		}
	}
	else{
		CharacterActivatedShard();
	}
}

void AGACharacter::ActivateShard(){
	if (Role == ROLE_Authority){
		ShardAvailable = false;
		CharacterActivatedShard();
		for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			if (ActorItr->IsAlive){
				ActorItr->TakeDamageByEnemy(-1);
			}
		}
	}
}

#pragma endregion

#pragma region Username

void AGACharacter::LoadUserNameFromData(){
	FString tmpName;
	bool IsRead = false;
	IsRead = FFileHelper::LoadFileToString(tmpName, TEXT("UserData.txt"));
	if (IsRead){
		UE_LOG(LogClass, Log, TEXT("USERNAME WAS LOADED FROM FILE UserData"));
	}
	SetGAUsername(tmpName);
}

void AGACharacter::SaveUserNameFromData(){
	bool IsWritten = false;
	IsWritten = FFileHelper::SaveStringToFile(GAUserName, TEXT("UserData.txt"));
	if (IsWritten){
		UE_LOG(LogClass, Log, TEXT("USERNAME SAVED TO FILE UserData"));
	}
}

void AGACharacter::SetLocalGAUsername(const FString& Username){
	if (Role < ROLE_Authority){
		ServerSetLocalGAUsername(Username);
	}
	else {
		GAUserName = Username;
	}
}

bool AGACharacter::ServerSetLocalGAUsername_Validate(const FString& Username){ return true; }
void AGACharacter::ServerSetLocalGAUsername_Implementation(const FString& Username){ SetLocalGAUsername(Username); }

void AGACharacter::SetGAUsername(const FString& Username){
	SaveUserNameFromData();
	SetLocalGAUsername(Username);
	ServerSetGAUsername(Username);
}

bool AGACharacter::ServerSetGAUsername_Validate(const FString& Username){ return true; }
void AGACharacter::ServerSetGAUsername_Implementation(const FString& Username){
	GetWorld()->GetAuthGameMode()->Broadcast(this, Username, "Username");
}

#pragma endregion

// Replicates All Replicated Properties
void AGACharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGACharacter, GAUserName);

	// Spectating
	DOREPLIFETIME(AGACharacter, CurrentPlayers);

	// Shard
	DOREPLIFETIME(AGACharacter, IsAllowedToChannelShard);
	DOREPLIFETIME(AGACharacter, ShardAvailable);
	DOREPLIFETIME(AGACharacter, ShardTimer);
	DOREPLIFETIME(AGACharacter, ShardActivationTime);

	// PowerUp
	DOREPLIFETIME(AGACharacter, IsInvulnerable);
	DOREPLIFETIME(AGACharacter, IsOneAttackKill);

	DOREPLIFETIME(AGACharacter, IsPowerUpActive);
	DOREPLIFETIME(AGACharacter, ActivePowerUp);
	DOREPLIFETIME(AGACharacter, PowerUpDuration);

	// Aura
	DOREPLIFETIME(AGACharacter, HasActivatedAura);

	// Armor
	DOREPLIFETIME(AGACharacter, Armor);
	DOREPLIFETIME(AGACharacter, ArmorResetValue);

	// Simple Attack
	DOREPLIFETIME(AGACharacter, SimpleAttackDamage);
	DOREPLIFETIME(AGACharacter, SimpleAttackCoolDown);
	DOREPLIFETIME(AGACharacter, SimpleAttackCoolDownResetValue);
	DOREPLIFETIME(AGACharacter, SimpleAttackOnCoolDown);

	// Special Attack
	DOREPLIFETIME(AGACharacter, SpecialAttackChargeTimer);
	DOREPLIFETIME(AGACharacter, SpecialAttackCoolDown);
	DOREPLIFETIME(AGACharacter, SpecialAttackTimesCharged);
	DOREPLIFETIME(AGACharacter, SpecialAttackMaxCharges);
	DOREPLIFETIME(AGACharacter, SpecialAttackCoolDownResetValue);
	DOREPLIFETIME(AGACharacter, SpecialAttackOnCoolDown);
	DOREPLIFETIME(AGACharacter, SpecialAttackIsCharging);

	// Player Stats
	DOREPLIFETIME(AGACharacter, HealthPoints);
	DOREPLIFETIME(AGACharacter, HealthResetValue);
	DOREPLIFETIME(AGACharacter, OutOfCombatTime);
	DOREPLIFETIME(AGACharacter, RegenerationAmount);
	DOREPLIFETIME(AGACharacter, RegenerationRate);
	DOREPLIFETIME(AGACharacter, AllowedToRegenerate);
	DOREPLIFETIME(AGACharacter, RegenerationTimer);
	DOREPLIFETIME(AGACharacter, MaxHealth);
	DOREPLIFETIME(AGACharacter, RegenerationTime);
	DOREPLIFETIME(AGACharacter, HasTookDamage);
	DOREPLIFETIME(AGACharacter, HasDied);

	// Shop
	DOREPLIFETIME(AGACharacter, Shop);
	
	// Items
	DOREPLIFETIME(AGACharacter, WeaponActor);
	DOREPLIFETIME(AGACharacter, HasBeenHealed);
	DOREPLIFETIME(AGACharacter, HasPickedUpItem);
	DOREPLIFETIME(AGACharacter, HasEquipedItem);
	DOREPLIFETIME(AGACharacter, EquipItems);
	DOREPLIFETIME(AGACharacter, InventoryItems);
	DOREPLIFETIME(AGACharacter, InventorySlots);
	DOREPLIFETIME(AGACharacter, TouchedItem);

	// Ressource
	DOREPLIFETIME(AGACharacter, Ressource);
}

// Sets the Weapon Actor
void AGACharacter::SetWeaponActor(AGAWeapon* Weapon){
	if (Role < ROLE_Authority){
		ServerSetWeaponActor(Weapon);
	}
	else{
		WeaponActor = Weapon;
	}
}

bool AGACharacter::ServerSetWeaponActor_Validate(AGAWeapon* Weapon){ return true; }
void AGACharacter::ServerSetWeaponActor_Implementation(AGAWeapon* Weapon){ SetWeaponActor(Weapon); }

/************************************************************************/
/*  BELOW DEPRECATED                                                    */
/************************************************************************/

#pragma region Regeneration

// Function Will Regenerate Health - Called By Tick
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
			if (RegenerationTime >= RegenerationRate && HealthPoints < MaxHealth){
				HealthPoints = (HealthPoints + RegenerationAmount > MaxHealth ? MaxHealth : HealthPoints + RegenerationAmount);
				RegenerationTime = 0;
			}
		}
	}
}

#pragma endregion

#pragma region Items

// Function Will Buy A Randomed Item From The Shop
void AGACharacter::BuyItem(){
	if (Role < ROLE_Authority){
		ServerBuyItem();
	}
	else{
		if (Ressource >= Shop->ItemCost){
			AGAItem* ShopItem = Shop->BuyItem();
			Ressource -= Shop->ItemCost;
			PickUpItem(ShopItem);
		}
	}
}

// *** TEMPORARY DUE TO NO UI ***
void AGACharacter::SellLastItem(){
	if (InventoryItems.Num() > 0){
		SellItem(InventoryItems.Last());
	}
}

// Function Will Sell The Given Item
void AGACharacter::SellItem(AGAItem* item){
	if (Role < ROLE_Authority){
		ServerSellItem(item);
	}
	else{
		Ressource += item->Value;

		// Safe Remove
		if (item == EquipItems.Chest) EquipItems.Chest = nullptr;
		else if (item == EquipItems.Trinket00) EquipItems.Trinket00 = nullptr;
		else if (item == EquipItems.Trinket01) EquipItems.Trinket01 = nullptr;
		else if (item == EquipItems.Weapon) EquipItems.Weapon = nullptr;
		if (InventoryItems.Contains(item)) InventoryItems.Remove(item);

		item->Destroy();
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: SOLD ITEM ***"));
	}
}

// Function Will Calculate All Equiped Items And Given Stats
void AGACharacter::CalculateItems(){
	if (Role < ROLE_Authority){
		ServerCalculateItems();
	}
	else {
		FPlayerAura PercentBonus;
		FStats		ItemStatsBonus;
		TArray <AGAItem*> EquipedItems;
		if (EquipItems.Head) EquipedItems.Add(EquipItems.Head);
		if (EquipItems.Chest) EquipedItems.Add(EquipItems.Chest);
		if (EquipItems.Weapon) EquipedItems.Add(EquipItems.Weapon);
		if (EquipItems.Trinket00) EquipedItems.Add(EquipItems.Trinket00);
		if (EquipItems.Trinket01) EquipedItems.Add(EquipItems.Trinket01);

		// Gather All Bonus
		for (int i = 0; i < EquipedItems.Num(); i++){
			// Gather Player Percentages
			PercentBonus.PercentDamage += EquipedItems[i]->AuraPlayer.PercentDamage;
			PercentBonus.PercentArmor += EquipedItems[i]->AuraPlayer.PercentArmor;
			PercentBonus.PercentAttackSpeed += EquipedItems[i]->AuraPlayer.PercentAttackSpeed;
			PercentBonus.PercentMovementSpeed += EquipedItems[i]->AuraPlayer.PercentMovementSpeed;
			PercentBonus.PercentHealth += EquipedItems[i]->AuraPlayer.PercentHealth;

			// Gather Stats
			ItemStatsBonus.Attack += EquipedItems[i]->ItemStats.Attack;
			ItemStatsBonus.Armor += EquipedItems[i]->ItemStats.Armor;
			ItemStatsBonus.Health += EquipedItems[i]->ItemStats.Health;
			ItemStatsBonus.CriticalInPercent += EquipedItems[i]->ItemStats.CriticalInPercent;
			ItemStatsBonus.AttackSpeedInPercent += EquipedItems[i]->ItemStats.AttackSpeedInPercent;
			ItemStatsBonus.MovementInPercent += EquipedItems[i]->ItemStats.MovementInPercent;
		}

		// Damage Bonus
		ItemDamage = ItemStatsBonus.Attack + ItemStatsBonus.Attack * (PercentBonus.PercentDamage + AuraBonus.PercentDamage + OtherPlayerAura.PercentDamage) / 100;

		// Attack Speed
		AttackSpeed = 1 + 1 * (PercentBonus.PercentAttackSpeed + ItemStatsBonus.AttackSpeedInPercent + AuraBonus.PercentAttackSpeed + OtherPlayerAura.PercentAttackSpeed) / 100;
		SimpleAttackCoolDown = SimpleAttackCoolDownResetValue / AttackSpeed;

		// Critical Chance
		Critical = ItemStatsBonus.CriticalInPercent;

		// Armor
		Armor = (ArmorResetValue + ItemStatsBonus.Armor) + (ArmorResetValue + ItemStatsBonus.Armor) *
			(PercentBonus.PercentArmor + AuraBonus.PercentArmor + OtherPlayerAura.PercentArmor) / 100;
		ArmorReduction = Armor * ArmorReductionPercent;

		// Movement Speed
		CharacterMovement->MaxWalkSpeed = BaseMovementSpeed + BaseMovementSpeed *
			(PercentBonus.PercentMovementSpeed + ItemStatsBonus.MovementInPercent + AuraBonus.PercentMovementSpeed + OtherPlayerAura.PercentMovementSpeed) / 100;

		// Health
		ItemHealth = ItemStatsBonus.Health + (HealthPoints + ItemStatsBonus.Health) *
			(PercentBonus.PercentHealth + AuraBonus.PercentHealth + OtherPlayerAura.PercentHealth) / 100;
		MaxHealth = HealthResetValue + ItemHealth;
		if (MaxHealth < HealthPoints) HealthPoints = MaxHealth;

		UE_LOG(LogClass, Log, TEXT("*** SERVER :: CALCULATED EQUIPED ITEMS ***"));
	}
}

void AGACharacter::SetItemSlot(int32 row, int32 col, AGAItem* item) {
	inventory.setItem(row, col, item);
}

bool AGACharacter::ClearItemSlot(int32 row, int32 col) {
	return inventory.clearElement(row, col);
}

void AGACharacter::UnequipItem(EGASlot::Type itemType){
	switch (itemType) {
	case EGASlot::GAHead:
		EquipItems.Head = nullptr;
		break;
	case EGASlot::GAChest:
		EquipItems.Chest = nullptr;
		break;
	case EGASlot::GATrinket:
		EquipItems.Trinket00 = nullptr;
		break;
	case EGASlot::GAWeapon:
		EquipItems.Weapon = nullptr;
		break;
	}
	CalculateAura();
	CalculateItems();
}


// Function Will Equip The Given Item And Unequip If Slot Is Not Empty
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
		CalculateAura();
		CalculateItems();
		HasEquipedItem = true;
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: EQUIPED ITEM ***"));
	}
}


// Function Will Place The Given Item In Inventory If Possible
void AGACharacter::PickUpItem(AGAItem* item){
	if (Role < ROLE_Authority){
		ServerPickUpItem(item);
	}
	if (InventoryItems.Num() < InventorySlots && item->finishedDropAnimation){
		TouchedItem = item;
		HasPickedUpItem = true;
		// If Item Is Money
		if (item->Slot == EGASlot::GAMoney){
			Ressource += item->Value;
			item->DestroyConstructedComponents();
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: PICKED UP %f Money (%f) ***"), item->Value, Ressource);
		}
		// If Item is Equipable Item
		else{
			inventory.registerElement(item);
			InventoryItems.Add(item);
			item->DestroyConstructedComponents();
			UE_LOG(LogClass, Log, TEXT("*** SERVER :: PICKED UP ITEM ***"));
		}
	}
	else UE_LOG(LogClass, Log, TEXT("*** SERVER :: INVENTORY IS FULL ***"));
}

// If Player Overlaps With Another Actor *** OVERRIDE - Checks Actor With Tag "Item" ***
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

#pragma region Aura

// Function Activates Aura Effect
void AGACharacter::ActivateAura(){
	if (Role < ROLE_Authority){
		ServerActivateAura();
	}
	else{
		HasActivatedAura = true;
		CharacterActivatedAura();
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: ACTIVATED AURA ***"));
	}
}

// Function Deactivates Aura Effect
void AGACharacter::DeactivateAura(){
	if (Role < ROLE_Authority){
		ServerDeactivateAura();
	}
	else{
		HasActivatedAura = false;
		CharacterDeactivatedAura();
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: DECTIVATED AURA ***"));
	}
}

// Function Calculates Equiped Item Aura
void AGACharacter::CalculateAura(){
	if (Role < ROLE_Authority){
		ServerCalculateAura();
	}
	else {
		if (EquipItems.Head){
			AuraBonus.PercentDamage = EquipItems.Head->AuraGroup.PercentDamage;
			AuraBonus.PercentArmor = EquipItems.Head->AuraGroup.PercentArmor;
			AuraBonus.PercentAttackSpeed = EquipItems.Head->AuraGroup.PercentAttackSpeed;
			AuraBonus.PercentMovementSpeed = EquipItems.Head->AuraGroup.PercentMovementSpeed;
			AuraBonus.PercentHealth = EquipItems.Head->AuraGroup.PercentHealth;
			AuraBonus.EffectRadius = EquipItems.Head->AuraGroup.EffectRadius;
		}

		// Check if Aura Already Active Or Not
		if (AuraBonus.hasAura() && !HasActivatedAura){
			ActivateAura();
		}
		else{
			DeactivateAura();
		}
	}
}

// Function Checks If Player Are In Range To Receive Their Aura
void AGACharacter::CheckPlayerInAuraRange(){
	if (Role < ROLE_Authority){
		ServerCheckPlayerInAuraRange();
	}
	else {
		// Find Player Near By
		for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			// If In Range Add All Aura Effects
			if (IsInRange(*ActorItr, ActorItr->AuraBonus.EffectRadius) && *ActorItr != this && !AuraPlayer.Contains(*ActorItr)){
				OtherPlayerAura.PercentDamage += ActorItr->AuraBonus.PercentDamage;
				OtherPlayerAura.PercentArmor += ActorItr->AuraBonus.PercentArmor;
				OtherPlayerAura.PercentHealth += ActorItr->AuraBonus.PercentHealth;
				OtherPlayerAura.PercentMovementSpeed += ActorItr->AuraBonus.PercentMovementSpeed;
				OtherPlayerAura.PercentAttackSpeed += ActorItr->AuraBonus.PercentAttackSpeed;

				AuraPlayer.Add(*ActorItr);								// AuraPlayer Contains Every Player In Aura Range
				if (!HasActivatedAura && OtherPlayerAura.hasAura()) ActivateAura();
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: PLAYER JOINED AURA RANGE ***"));
				if (OtherPlayerAura.hasAura()) CalculateItems();		// Calculate The New Attributes With The new Given Aura
			}
			// If Not In Range But Was In Range Before Remove Aura
			else if (!IsInRange(*ActorItr, ActorItr->AuraBonus.EffectRadius) && AuraPlayer.Contains(*ActorItr)){
				OtherPlayerAura.PercentDamage -= ActorItr->AuraBonus.PercentDamage;
				OtherPlayerAura.PercentArmor -= ActorItr->AuraBonus.PercentArmor;
				OtherPlayerAura.PercentHealth -= ActorItr->AuraBonus.PercentHealth;
				OtherPlayerAura.PercentMovementSpeed -= ActorItr->AuraBonus.PercentMovementSpeed;
				OtherPlayerAura.PercentAttackSpeed -= ActorItr->AuraBonus.PercentAttackSpeed;

				AuraPlayer.Remove(*ActorItr);							// AuraPlayer Contains Every Player In Aura Range
				if (HasActivatedAura && !AuraBonus.hasAura() && !OtherPlayerAura.hasAura()) DeactivateAura();
				UE_LOG(LogClass, Log, TEXT("*** SERVER :: PLAYER LEFT AURA RANGE ***"));
				if (OtherPlayerAura.hasAura()) CalculateItems();		// Calculate The New Attributes With The new Given Aura
			}
		}
	}
}

#pragma endregion

#pragma  region Network - Regeneration

// Client Reaction On Replication Notification - Differs 2 Event Calls Due To Stop And Start Regeneration
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

bool AGACharacter::ServerRegenerateHealth_Validate(float Delta){ return true; }
void AGACharacter::ServerRegenerateHealth_Implementation(float Delta){ RegenerateHealth(Delta); }

#pragma endregion

#pragma region Network - Items

// Client Reaction On Replication Notification - Only Reacts If True
void AGACharacter::OnRep_HasPickedUpItem(){
	if (HasPickedUpItem){
		// If Money
		if (TouchedItem->Slot = EGASlot::GAMoney){
			Ressource += TouchedItem->Value;
			TouchedItem->DestroyConstructedComponents();
			UE_LOG(LogClass, Log, TEXT("*** CLIENT :: PICKED UP %f Money (%f) ***"), TouchedItem->Value, Ressource);
		}
		// If Equipable Item
		else{
			TouchedItem->DestroyConstructedComponents();
			CharacterPickedUpItem();
			ServerResetHasPickedUpItem();
			UE_LOG(LogClass, Log, TEXT("*** CLIENT :: PICKED UP ITEM ***"));
		}
	}
}

// Client Reaction On Replication Notification - Only Reacts If True
void AGACharacter::OnRep_HasEquipedItem(){
	if (HasEquipedItem){
		CharacterEquipedItem();
		ServerResetHasEquipedItem();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: EQUIPED ITEM ***"));
	}
}

bool AGACharacter::ServerBuyItem_Validate(){ return true; }
void AGACharacter::ServerBuyItem_Implementation(){ BuyItem(); }

bool AGACharacter::ServerSellItem_Validate(AGAItem* item){ return true; }
void AGACharacter::ServerSellItem_Implementation(AGAItem* item){ SellItem(item); }

bool AGACharacter::ServerPickUpItem_Validate(AGAItem* item){ return true; }
void AGACharacter::ServerPickUpItem_Implementation(AGAItem* item){ PickUpItem(item); }

bool AGACharacter::ServerEquipItem_Validate(AGAItem* item){ return true; }
void AGACharacter::ServerEquipItem_Implementation(AGAItem* item){ EquipItem(item); }

bool AGACharacter::ServerCalculateItems_Validate(){ return true; }
void AGACharacter::ServerCalculateItems_Implementation(){ CalculateItems(); }

bool AGACharacter::ServerResetHasPickedUpItem_Validate(){ return true; }
void AGACharacter::ServerResetHasPickedUpItem_Implementation(){ HasPickedUpItem = false; }

bool AGACharacter::ServerResetHasEquipedItem_Validate(){ return true; }
void AGACharacter::ServerResetHasEquipedItem_Implementation(){ HasEquipedItem = false; }

#pragma endregion

#pragma region Network - Aura

// Client Reaction On Replication Notification - Differs 2 Event Calls Due To Activation And Deactivation Of Aura
void AGACharacter::OnRep_HasActivatedAura(){
	if (HasActivatedAura){
		CharacterActivatedAura();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: ACTIVATED AURA ***"));
	}
	else{
		CharacterDeactivatedAura();
		UE_LOG(LogClass, Log, TEXT("*** CLIENT :: DECTIVATED AURA ***"));
	}
}

bool AGACharacter::ServerCheckPlayerInAuraRange_Validate(){ return true; }
void AGACharacter::ServerCheckPlayerInAuraRange_Implementation(){ CheckPlayerInAuraRange(); }

bool AGACharacter::ServerCalculateAura_Validate(){ return true; }
void AGACharacter::ServerCalculateAura_Implementation(){ CalculateAura(); }

bool AGACharacter::ServerActivateAura_Validate(){ return true; }
void AGACharacter::ServerActivateAura_Implementation(){ ActivateAura(); }

bool AGACharacter::ServerDeactivateAura_Validate(){ return true; }
void AGACharacter::ServerDeactivateAura_Implementation(){ DeactivateAura(); }

#pragma endregion