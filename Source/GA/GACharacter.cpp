// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
#include "GAEnemy.h"
#include "GAPlayerController.h"

//////////////////////////////////////////////////////////////////////////
// AGACharacter

AGACharacter::AGACharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Inventory
	InventorySlots = 10;

	// Simple Attack
	SimpleAttackDamage = 25;
	SimpleAttackCoolDown = 0.75;

	// Special Attack
	SpecialAttackBaseDamage = 25;
	SpecialAttackMaxDamage = 100;
	SpecialAttackChargeInterval = 0.2;
	SpecialAttackCoolDown = 5;

	// Player Stats
	HealthPoints = 100;
	OutOfCombatTime = 5;
	RegenerationRate = 1;
	RegenerationAmount = 5;

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

void AGACharacter::AttackSimple(){
	((AGAPlayerController*)Controller)->AttackSimple();
}
void AGACharacter::AttackSpecial(){
	((AGAPlayerController*)Controller)->AttackSpecial();
}
void AGACharacter::ChargeSpecial(){
	((AGAPlayerController*)Controller)->ChargeSpecial();
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
	if (Controller != NULL && (Value != 0.0f) && ((AGAPlayerController*)Controller)->isAllowedToMove())
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
	if ((Controller != NULL) && (Value != 0.0f) && ((AGAPlayerController*)Controller)->isAllowedToMove())
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


void AGACharacter::ReceiveActorBeginOverlap(class AActor* OtherActor){
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Item")){
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: TOUCHED ITEM ***"));
		((AGAPlayerController*)Controller)->PickUpItem((AGAItem*)OtherActor);
	}
}